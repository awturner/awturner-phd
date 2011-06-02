/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#include "CorrespondenceFinder.h"

#include "Mesh/AreaAveragedNormalCalculator.h"
#include "Mesh/Mesh.h"
#include "Mesh/MeshConnectivity.h"
#include "Mesh/Pose.h"
#include "Mesh/TuplesImpl.h"

#include "MeshSearching/FacesNearestPointSearch.h"
#include "MeshSearching/FacesRayIntersectionSearch.h"

#include "Useful/ProfTimer.h"

#include "vnl/algo/vnl_determinant.h"
#include "vnl/algo/vnl_svd.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

#include <limits>

template <class T>
struct AWT::CorrespondenceFinder<T>::D
{
   typename Mesh<T>::P             m_Target;
   typename Mesh<T>::P             m_Source;
              
   typename Tuples<T>::P           m_Correspondences;
   typename Tuples<MeshIndex>::P   m_MeshIndices;
         
   typename MeshConnectivity<T>::P m_MeshConnectivity;
    
   T                               m_MaxError;
   T                               m_RmsError;
   T                               m_SumOfSquaredErrors;

                                 
   vnl_matrix<T>                   m_Covariance;
   vnl_vector<T>                   m_TargetSum;
   vnl_vector<T>                   m_SourceSum;
           
   vnl_matrix<T>                   m_Rotation;
            
   T                               m_TargetVar;
   T                               m_SourceVar;
                                
   bool                            m_IgnoreEdges;
                               
   T                               m_CompatCosine;

   static MeshIndex                ms_InvalidCorrespondenceIndex;

   bool                            m_BothWays;
};

template <class T>
AWT::MeshIndex AWT::CorrespondenceFinder<T>::D::ms_InvalidCorrespondenceIndex = std::numeric_limits<AWT::MeshIndex>::max( );

template <class T>
AWT::CorrespondenceFinder<T>::CorrespondenceFinder( typename Mesh<T>::P source, typename Mesh<T>::P target, const bool bothWays )
{
   m_D = new D;

   m_D->m_BothWays = bothWays;

   m_D->m_CompatCosine = 0.7;

   m_D->m_IgnoreEdges = false;

   m_D->m_Target = target;
   m_D->m_Source = source;

   m_D->m_MeshConnectivity = MeshConnectivity<T>::getInstance( target );

   m_D->m_MaxError           = std::numeric_limits<T>::infinity( );
   m_D->m_RmsError           = std::numeric_limits<T>::infinity( );
   m_D->m_SumOfSquaredErrors = std::numeric_limits<T>::infinity( );

   m_D->m_Covariance = vnl_matrix<T>( 3, 3 );
   m_D->m_Rotation   = vnl_matrix<T>( 3, 3 );
   m_D->m_TargetSum  = vnl_vector<T>( 3 );
   m_D->m_SourceSum  = vnl_vector<T>( 3 );
}

template <class T>
AWT::CorrespondenceFinder<T>::~CorrespondenceFinder( )
{
   delete m_D;
}

template <class T>
GETNAMEMACRO( AWT::CorrespondenceFinder<T> );

template <class T>
typename AWT::CorrespondenceFinder<T>::P AWT::CorrespondenceFinder<T>::getInstance( typename Mesh<T>::P source, typename Mesh<T>::P target, const bool bothWays )
{
   AUTOGETINSTANCE( AWT::CorrespondenceFinder<T>, ( source, target, bothWays ) );
}

template <class T>
void AWT::CorrespondenceFinder<T>::reset( )
{
   // Create new holders for the data
   T def6[] = { 0, 0, 0, 0, 0, 0 };
   Tuples<T>::P corrs = TuplesImpl<T>::getInstance( 6, def6, m_D->m_Target->getNumberOfVertices( ) );

   MeshIndex def = 0;
   Tuples<MeshIndex>::P idxs = TuplesImpl<MeshIndex>::getInstance( 1, &def, m_D->m_Target->getNumberOfVertices( ) );

   // ... and swap them into the place holders
   m_D->m_Correspondences = corrs;
   m_D->m_MeshIndices = idxs;

   m_D->m_Covariance.fill( 0 );
   m_D->m_TargetSum.fill( 0 );
   m_D->m_SourceSum.fill( 0 );

   m_D->m_TargetVar = m_D->m_SourceVar = 0;

   m_D->m_Rotation.fill( std::numeric_limits<T>::quiet_NaN( ) );

   m_D->m_MaxError           = std::numeric_limits<T>::infinity( );
   m_D->m_RmsError           = std::numeric_limits<T>::infinity( );
   m_D->m_SumOfSquaredErrors = std::numeric_limits<T>::infinity( );
}

template <class T>
unsigned int AWT::CorrespondenceFinder<T>::getNumberOfCorrespondences( ) const
{
   if ( *m_D->m_Correspondences == 0 )
      return 0;

   return m_D->m_Correspondences->getNumberOfPoints( );
}

template <class T>
AWT::ModifiedTime AWT::CorrespondenceFinder<T>::getModifiedTime( ) const
{
   ModifiedTime ret = ManagedObject::getModifiedTime( );

   if ( *m_D->m_Correspondences != 0 )
      ret = std::max<ModifiedTime>( ret, m_D->m_Correspondences->getModifiedTime( ) );

   return ret;
}

template <class T>
void AWT::CorrespondenceFinder<T>::update( )
{
   reset( );

   T testPoint[3];
   T testNormal[3];
   T nearestPoint[3];

   ProfilingTimer timer;
   timer.start( );
   m_D->m_Target->prepareToSearchFaces( );
   timer.stop( );
   //PRINTVBL2( "time to prepareToSearchFaces", timer.getDurationInSecs( ) );

   T rms = 0;
   T maxLen = 0;

   vnl_vector<T> vtxSource(3);
   vnl_vector<T> vtxTarget(3);

   T corr[6];
   MeshIndex nGoodCorrespondences = 0;
   const MeshIndex nverts = m_D->m_Source->getNumberOfVertices( );

   m_D->m_MeshIndices->clear( );
   m_D->m_Correspondences->clear( );

   timer.start( );

   int idx, i, r, c;
   T len;
   T tmp;

   FacesNearestPointSearch<T>::P searcher;

   {
      for ( MeshIndex v = 0; v < nverts; ++v )
      {
         searcher = FacesNearestPointSearch<T>::getInstance( );
         searcher->setCompatibilityCosine( m_D->m_CompatCosine );

         m_D->m_Source->getVertex( v, testPoint );

         m_D->m_Source->getVertexNormals( )->getPoint( v, testNormal );

         searcher->reset( );
         searcher->setTestPoint( testPoint );
         searcher->setTestNormal( testNormal );

         m_D->m_Target->searchFaces( searcher );

         idx = searcher->getNearestPoint( nearestPoint );

         if ( idx != -1 && ( !m_D->m_IgnoreEdges || !m_D->m_MeshConnectivity->isFaceOnBoundary( idx ) ) )
         {
            // Count the number of valid correspondences
            ++nGoodCorrespondences;

            m_D->m_MeshIndices->setPointElement( v, 0, idx );

            for ( i = 0; i < 3; ++i )
            {
               corr[i]      = testPoint[i];
               corr[3+i]    = nearestPoint[i];

               vtxSource[i] = testPoint[i];
               vtxTarget[i] = nearestPoint[i];
            }
    
            for ( r = 0; r < 3; ++r )
            {
               for ( c = 0; c < 3; ++c )
                  m_D->m_Covariance(r,c) += testPoint[r]*nearestPoint[c];

               m_D->m_SourceSum[r] += testPoint[r];
               m_D->m_TargetSum[r] += nearestPoint[r];

               m_D->m_SourceVar    += testPoint[r]*testPoint[r];
               m_D->m_TargetVar    += nearestPoint[r]*nearestPoint[r];
            }

            len = 0;
            for ( i = 0; i < 3; ++i )
            {
               tmp = nearestPoint[i] - testPoint[i];
               len += tmp*tmp;
            }

            rms += len;

            maxLen = std::max<T>( maxLen, len );
         }
         else
         {
            // This is the case where the correspondence isn't valid... but still record the 
            // fact that there is no correspondence
            m_D->m_MeshIndices->setPointElement( v, 0, D::ms_InvalidCorrespondenceIndex );

            for ( i = 0; i < 6; ++i )
            {
               corr[i] = std::numeric_limits<T>::quiet_NaN( );
            }
         }

         m_D->m_Correspondences->setPoint( v, corr );
      }
   }

   timer.stop( );
   //PRINTVBL2( "total time", timer.getDurationInSecs( ) );
   //PRINTVBL2( "time per c", timer.getDurationInSecs( ) / v );

   if ( nGoodCorrespondences != 0 )
   {
      // Now calculate the rotation, scaling and translation
      m_D->m_SourceSum /= static_cast<T>( nGoodCorrespondences );
      m_D->m_TargetSum /= static_cast<T>( nGoodCorrespondences );

      for ( int r = 0; r < 3; ++r )
         for ( int c = 0; c < 3; ++c )
            m_D->m_Covariance(r,c) -= m_D->m_SourceSum[r] * m_D->m_TargetSum[c] * nGoodCorrespondences;

      m_D->m_SourceVar -= dot_product( m_D->m_SourceSum, m_D->m_SourceSum ) * nGoodCorrespondences;
      m_D->m_TargetVar -= dot_product( m_D->m_TargetSum, m_D->m_TargetSum ) * nGoodCorrespondences;

      bool flag = false;
      for ( int r = 0; r < 3 && !flag; ++r )
         for ( int c = 0; c < 3 && !flag; ++c )
            if ( m_D->m_Covariance[r][c] != m_D->m_Covariance[r][c] )
               flag = true;

      if ( flag )
      {
         PRINTVBL( nGoodCorrespondences );
         PRINTVBL( m_D->m_SourceSum );
         PRINTVBL( m_D->m_TargetSum );
         PRINTVBL2( covar, m_D->m_Covariance );
         PAUSE;
      }

      vnl_svd<T> svd( m_D->m_Covariance );
      vnl_matrix<T> R = svd.V( ) * svd.U( ).transpose( );

      T det = vnl_determinant( R );

      // Flip if it is a reflection
      if ( det < 0 )
      {
         vnl_matrix<T> eye(3,3);
         eye.set_identity( );
         eye[2][2] = -1;

         R = svd.V( ) * eye * svd.U( ).transpose( );
      }

      m_D->m_Rotation = R;

      m_D->m_SumOfSquaredErrors = rms;

      rms = sqrt( rms / m_D->m_Source->getNumberOfVertices( ) );
      maxLen = sqrt( maxLen );

      m_D->m_MaxError = maxLen;
      m_D->m_RmsError = rms;
   }      

   modified( );

#undef USERAY

}

template <class T>
void AWT::CorrespondenceFinder<T>::getPose(const bool scaled, AWT::Pose<T> *pose) const
{
   Pose<T>::PoseMatrix mat;
   Pose<T>::PoseVector trans;

   getRotation( mat );
   getTranslation( trans, scaled );

   for ( unsigned int i = 0; i < 3; ++i )
      mat( i, 3 ) = trans( i );

   pose->setMatrix( mat );
}

template <class T>
void AWT::CorrespondenceFinder<T>::getRotation( typename AWT::Pose<T>::PoseMatrix& R ) const
{
   R.set_identity( );
   for ( int r = 0; r < 3; ++r )
      for ( int c = 0; c < 3; ++c )
         R( r, c ) = m_D->m_Rotation(r,c);
}

template <class T>
T AWT::CorrespondenceFinder<T>::getScale( ) const
{
   return sqrt( m_D->m_TargetVar / m_D->m_SourceVar );
}

template <class T>
void AWT::CorrespondenceFinder<T>::getTranslation( typename AWT::Pose<T>::PoseVector& t, bool isScaled ) const
{
   const T sc = isScaled ? getScale( ) : 1;
   
   vnl_vector<T> tt = m_D->m_TargetSum - sc * m_D->m_Rotation * m_D->m_SourceSum;

   for ( int i = 0; i < 3; ++i )
      t(i) = tt(i);

   t(3) = 0;
}

template <class T>
T AWT::CorrespondenceFinder<T>::getMaxError( ) const
{
   return m_D->m_MaxError;
}

template <class T>
T AWT::CorrespondenceFinder<T>::getRmsError( ) const
{
   return m_D->m_RmsError;
}

template <class T>
T AWT::CorrespondenceFinder<T>::getSumOfSquaredErrors( ) const
{
   return m_D->m_SumOfSquaredErrors;
}

template <class T>
void AWT::CorrespondenceFinder<T>::setCorrespondence( const unsigned int v, const T* sourceVtx, const T* targetVtx, const unsigned int idx )
{
   // Set the index
   m_D->m_MeshIndices->setPointElement( v, 0, idx );

   T pnt[6];

   for ( int i = 0; i < 3; ++i )
   {
      pnt[0+i] = sourceVtx[i];
      pnt[3+i] = targetVtx[i];
   }

   m_D->m_Correspondences->setPoint( v, pnt );
   modified( );
}

template <class T>
bool AWT::CorrespondenceFinder<T>::getCorrespondence( const unsigned int v, T* sourceVtx, T* targetVtx, unsigned int& idx ) const
{
   T pnt[6];

   idx = m_D->m_MeshIndices->getPointElement( v, 0 );
   
   m_D->m_Correspondences->getPoint( v, pnt );

   for ( int i = 0; i < 3; ++i )
   {
      sourceVtx[i] = pnt[0+i];
      targetVtx[i] = pnt[3+i];
   }

   return idx != D::ms_InvalidCorrespondenceIndex;
}

template <class T>
bool AWT::CorrespondenceFinder<T>::isIgnoreEdges( ) const
{
   return m_D->m_IgnoreEdges;
}

template <class T>
void AWT::CorrespondenceFinder<T>::setIgnoreEdges( const bool v )
{
   if ( v != m_D->m_IgnoreEdges )
   {
      m_D->m_IgnoreEdges = v;
      modified( );
   }
}


template <class T>
void AWT::CorrespondenceFinder<T>::setCompatibilityCosine( const T v )
{
   if ( v != m_D->m_CompatCosine )
   {
      m_D->m_CompatCosine = v;
      modified( );
   }
}

template <class T>
T AWT::CorrespondenceFinder<T>::getCompatibilityCosine( ) const
{
   return m_D->m_CompatCosine;
}


template class AWT::CorrespondenceFinder<double>;
template class AWT::CorrespondenceFinder<float>;