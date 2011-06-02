#include "MeshSimilarity.h"

#include "Mesh.h"
#include "MeshFunctions.h"
#include "NonRigidAlignment/CorrespondenceFinder.h"
#include "MeshSearching/PointQueries.h"
#include "MeshConnectivity.h"
#include "Mesh/AreaAveragedNormalCalculator.h"

using namespace AWT;

template <class T>
struct AWT::MeshSimilarity<T>::D
{
   T m_MeanDistance;
   T m_RmsDistance;
   T m_MaxDistance;
   T m_AreaOfDeviation;
   T m_AreaOfDeviationThreshold;

   unsigned int m_NumberOfSubdivisions;

   struct
   {
      T numeratorMean;
      T numeratorRms;
      T numeratorAOD;
      T maxDistance;

      T denominator;
   } intvals;

   void handleFace( const T vtxA[3][3], const T vtxB[3][3], const unsigned int levels, const T faceArea )
   {
      if ( levels == 0 )
      {
         T centroidA[] = { 0, 0, 0 };
         T centroidB[] = { 0, 0, 0 };

         for ( int i = 0; i < 3; ++i )
         {
            for ( int j = 0; j < 3; ++j )
            {
               centroidA[j] += vtxA[i][j];
               centroidB[j] += vtxB[i][j];
            }
         }

         for ( int j = 0; j < 3; ++j )
         {
            centroidA[j] /= 3;
            centroidB[j] /= 3;
         }

         const T centroidDistanceSquared = PointQueries<T>::getDistanceSquared( centroidA, centroidB );
         const T centroidDistance        = sqrt( centroidDistanceSquared );

         intvals.numeratorMean += faceArea * centroidDistance;
         intvals.numeratorRms  += faceArea * centroidDistanceSquared;
         intvals.numeratorAOD  += ( centroidDistance > m_AreaOfDeviationThreshold ) ? faceArea : 0;
         
         intvals.denominator   += faceArea;

         intvals.maxDistance    = std::max<T>( intvals.maxDistance, centroidDistance );
      }
      else
      {
         T subA[3][3];
         T subB[3][3];

         for ( unsigned int ax = 0; ax < 3; ++ax )
         {
            // Subdivide the triangle along the 1-2 edge
            subA[0][ax] = (vtxA[1][ax]+vtxA[2][ax])/2;
            subB[0][ax] = (vtxB[1][ax]+vtxB[2][ax])/2;

            // The zero-th vertex is present in both cases
            subA[1][ax] = vtxA[0][ax];
            subB[1][ax] = vtxB[0][ax];
         }

         for ( unsigned int i = 1; i <= 2; ++i )
         {
            for ( unsigned int ax = 0; ax < 3; ++ax )
            {
               subA[2][ax] = vtxA[i][ax];
               subB[2][ax] = vtxB[i][ax];
            }

            handleFace( subA, subB, levels-1, faceArea / 2 );
         }
      }
   }
};

template <class T>
AWT::MeshSimilarity<T>::MeshSimilarity( typename Mesh<T>::P meshA, typename Mesh<T>::P meshB, const T aodThresh, const unsigned int subdivs, const bool inCorr )
{
   m_D = new D;

   m_D->m_AreaOfDeviationThreshold = aodThresh;
   m_D->m_NumberOfSubdivisions     = subdivs;

   Mesh<T>::P meshes[] = { meshA, meshB };

   // Structure to hold the intermediate values needed
   m_D->intvals.numeratorMean = 0;
   m_D->intvals.numeratorRms  = 0;
   m_D->intvals.numeratorAOD  = 0;
   m_D->intvals.denominator   = 0;

   m_D->intvals.maxDistance = 0;

   NormalCalculator<T>::P nc = AreaAveragedNormalCalculator<T>::getInstance( );
   nc->calculateNormalsAndSet( meshA );
   nc->calculateNormalsAndSet( meshB );

   // Calculate the measures in both directions to make symmetrical
   for ( unsigned int mi = 0; mi < 2; ++mi )
   {
      // Find the corresponding points between the meshes
      CorrespondenceFinder<T>::P corrFinder = CorrespondenceFinder<T>::getInstance( meshes[mi], meshes[1-mi] );
      corrFinder->setCompatibilityCosine( -2 );

      corrFinder->update( );

      MeshIndex vs[3], idx;

      T vtxsSource[3][3];
      T vtxsTarget[3][3];

      // We will use this to determine if correspondence faces are on the fracture edges
      MeshConnectivity<T>::P meshConn;
      
      if ( !inCorr )
         meshConn = MeshConnectivity<T>::getInstance( meshes[1-mi] );

      //for ( MeshIndex f = 0; f < meshes[mi]->getNumberOfFaces( ); ++f )
      MESH_EACHFACE( meshes[mi], f )
      {
         // This approximates Lamecker's measures by assuming that the distance to
         // the "correspondence face" is the distance between the triangle centroids

         // We need all of the correspondences to be valid in order to count a face
         bool allCorrsValid = true;

         if ( inCorr )
         {
            meshes[mi]->getFace( f, vtxsSource[0], vtxsSource[1], vtxsSource[2] );
            meshes[1-mi]->getFace( f, vtxsTarget[0], vtxsTarget[1], vtxsTarget[2] );
         }
         else
         {
            // Get the vertex indices of the triangle
            meshes[mi]->getFaceIndices( f, vs );

            for ( int i = 0; i < 3 && allCorrsValid; ++i )
            {
               if ( !corrFinder->getCorrespondence( vs[i], vtxsSource[i], vtxsTarget[i], idx ) )
               {
                  allCorrsValid = false;
               }
               else if ( meshConn->isFaceOnBoundary( idx ) )
               {
                  // Reject the correspondences if the face is on the bounary
                  allCorrsValid = false;
               }
            }
         }

         // If any of the correspondences are invalid, we don't want to count this
         // correspondence face
         if ( allCorrsValid )
         {
            const T faceArea = MeshFunctions<T>::getFaceArea( meshes[mi], f );

            m_D->handleFace( vtxsSource, vtxsTarget, subdivs, faceArea );
         }
      }
   }

   m_D->m_MeanDistance    = m_D->intvals.numeratorMean / m_D->intvals.denominator;
   m_D->m_RmsDistance     = m_D->intvals.numeratorRms  / m_D->intvals.denominator;
   m_D->m_AreaOfDeviation = m_D->intvals.numeratorAOD  / m_D->intvals.denominator;
   m_D->m_MaxDistance     = m_D->intvals.maxDistance;

   m_D->m_RmsDistance = sqrt( m_D->m_RmsDistance );
}

template <class T>
AWT::MeshSimilarity<T>::~MeshSimilarity( )
{
   delete m_D;
}

template <class T>
typename AWT::MeshSimilarity<T>::P AWT::MeshSimilarity<T>::getInstance( typename Mesh<T>::P meshA, typename Mesh<T>::P meshB, const T aodThresh, 
                                                                       const unsigned int subdivs, const bool inCorr )
{
   AUTOGETINSTANCE( AWT::MeshSimilarity<T>, ( meshA, meshB, aodThresh, subdivs, inCorr ) );
}

template <class T>
GETNAMEMACRO( AWT::MeshSimilarity<T> );

template <class T>
T AWT::MeshSimilarity<T>::getMeanDistance( )    const
{
   return m_D->m_MeanDistance;
}

template <class T>
T AWT::MeshSimilarity<T>::getRmsDistance( )     const
{
   return m_D->m_RmsDistance;
}

template <class T>
T AWT::MeshSimilarity<T>::getMaximumDistance( ) const
{
   return m_D->m_MaxDistance;
}

template <class T>
T AWT::MeshSimilarity<T>::getAreaOfDeviation( ) const
{
   return m_D->m_AreaOfDeviation;
}

template <class T>
T AWT::MeshSimilarity<T>::getAreaOfDeviationThreshold( ) const
{
   return m_D->m_AreaOfDeviationThreshold;
}

template <class T>
unsigned int AWT::MeshSimilarity<T>::getNumberOfSubdivisions( ) const
{
   return m_D->m_NumberOfSubdivisions;
}

template class AWT::MeshSimilarity<double>;
template class AWT::MeshSimilarity<float>;