#include "SmoothingCallback.h"

#include "MeshSearching/FacesTreeData.h"
#include "MeshSearching/FaceQueries.h"
#include "MeshSearching/PointQueries.h"

#include "Useful/ArrayVectorFunctions.h"

template <class T>
struct AWT::SmoothingCallback<T>::D
{
   T                      m_TestPoint[3];  
                        
   T                      m_SpatialSigma;  
   T                      m_InfluenceSigma;
                        
   T                      m_Prediction[3]; 
   T                      m_PredDenom;     
                        
   bool                   m_Mollifying;    
                        
   typename Mesh<T>::P    m_Mesh;          
   typename Tuples<T>::P  m_Normals;       

};

template <class T>
AWT::SmoothingCallback<T>::SmoothingCallback( )
{
   m_D = new D;

   m_D->m_SpatialSigma    =  1;      
   m_D->m_InfluenceSigma  =  1;      
   m_D->m_Mollifying      =  false;

}

template <class T>
AWT::SmoothingCallback<T>::~SmoothingCallback( )
{
   delete m_D;
}

template <class T>
typename AWT::SmoothingCallback<T>::P AWT::SmoothingCallback<T>::getInstance( )
{
   AUTOGETINSTANCE( AWT::SmoothingCallback<T>, ( ) );
}

template <class T>
GETNAMEMACRO( AWT::SmoothingCallback<T> );

template <class T>
void AWT::SmoothingCallback<T>::reset( )
{
   m_D->m_Prediction[0] = 0;
   m_D->m_Prediction[1] = 0;
   m_D->m_Prediction[2] = 0;
   m_D->m_PredDenom     = 0;
}

#include "vnl/algo/vnl_svd.h"

template <class T>
void AWT::SmoothingCallback<T>::faceFound( const typename AWT::OEKDTree::OEKDTreeData<T,3>::P in_Data, const AWT::OEKDTree::ObjectIndex index )
{
   if ( *m_D->m_Mesh == 0 )
      return;

   const FacesTreeData<T>* data = reinterpret_cast<const FacesTreeData<T>*>( in_Data.getDataConst( ) );

   T pntA[3], pntB[3], pntC[3];
   data->getVertex( index, 0, pntA );
   data->getVertex( index, 1, pntB );
   data->getVertex( index, 2, pntC );

   for ( int i = 0; i < 3; ++i )
   {
      pntB[i] -= pntA[i];
      pntC[i] -= pntA[i];
   }

   cross( pntB, pntC, pntA );
   T area = normalize( pntA, 3 ) / 2;

   Tuples<T>::P normals = m_D->m_Mesh->getFaceNormals( );

   T nml[3];
   T centroid[3];

   normals->getPoint( index, nml );
   FaceQueries<T>::getCentroid( in_Data, index, centroid );

   // Get the plane equation into normal form
   normalize( nml, 3 );
   T d = -dot( centroid, nml, 3 );
   
   T pntPred[3];

   T dot_x_nml = dot( nml, m_D->m_TestPoint, 3 );

   for ( int i = 0; i < 3; ++i )
      pntPred[i] = m_D->m_TestPoint[i] - ( dot_x_nml + d ) * nml[i];

   T weight = 1;

   if ( !isMollifying( ) )
   {
      T xx = 0;

      for ( int i = 0; i < 3; ++i )
      {
         T tmp = ( pntPred[i] - m_D->m_TestPoint[i] );
         xx += tmp*tmp;
      }

      weight *= exp( -xx / (2*(m_D->m_InfluenceSigma*m_D->m_InfluenceSigma)) );
   }

   {
      T xx = 0;

      for ( int i = 0; i < 3; ++i )
      {
         T tmp = ( centroid[i] - m_D->m_TestPoint[i] );
         xx += tmp*tmp;
      }

      weight *= exp( -xx / (2*( m_D->m_SpatialSigma * m_D->m_SpatialSigma ) ) );
   }

   // weight *= area;

   // TODO No area weighting!!

   for ( int i = 0; i < 3; ++i )
      m_D->m_Prediction[i] += weight * pntPred[i];

   m_D->m_PredDenom += weight;

   //PRINTVBL( m_D->m_PredDenom );
}

template <class T>
void AWT::SmoothingCallback<T>::getPredictedPoint( T* out_PredPoint ) const
{
   for ( int i = 0; i < 3; ++i )
      out_PredPoint[i] = m_D->m_Prediction[i] / m_D->m_PredDenom;
}

template <class T>
typename AWT::Tuples<T>::P AWT::SmoothingCallback<T>::getNormals( )
{
   return m_D->m_Normals;
}

template <class T>
void AWT::SmoothingCallback<T>::setNormals( typename AWT::Tuples<T>::P normals )
{
   m_D->m_Normals = normals;
}

template <class T>
typename AWT::Mesh<T>::P AWT::SmoothingCallback<T>::getMesh( )
{
   return m_D->m_Mesh;
}

template <class T>
void AWT::SmoothingCallback<T>::setMesh( typename AWT::Mesh<T>::P mesh )
{
   m_D->m_Mesh = mesh;
}

template <class T>
void AWT::SmoothingCallback<T>::setTestPoint( const T* in_TestPoint )
{
   for ( int i = 0; i < 3; ++i )
      m_D->m_TestPoint[i] = in_TestPoint[i];
}

template <class T>
void AWT::SmoothingCallback<T>::getTestPoint( T* out_TestPoint ) const
{
   for ( int i = 0; i < 3; ++i )
      out_TestPoint[i] = m_D->m_TestPoint[i];
}

template <class T>
T AWT::SmoothingCallback<T>::getSpatialSigma( ) const
{
   return m_D->m_SpatialSigma;
}

template <class T>
void AWT::SmoothingCallback<T>::setSpatialSigma( const T sigma )
{
   m_D->m_SpatialSigma = sigma;
}

template <class T>
T AWT::SmoothingCallback<T>::getInfluenceSigma( ) const
{
   return m_D->m_InfluenceSigma;
}

template <class T>
void AWT::SmoothingCallback<T>::setInfluenceSigma( const T sigma )
{
   m_D->m_InfluenceSigma = sigma;
}

template <class T>
bool AWT::SmoothingCallback<T>::isMollifying( ) const
{
   return m_D->m_Mollifying;
}

template <class T>
void AWT::SmoothingCallback<T>::setMollifying( const bool m )
{
   m_D->m_Mollifying = m;
}

template class AWT::SmoothingCallback<double>;
template class AWT::SmoothingCallback<float>;