#ifndef __FRAGMENT_H__
#define __FRAGMENT_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/SamplePoints.h"
#include "Mesh/Pose.h"
#include "Mesh/NormalCalculator.h"
#include "Mesh/SampledMesh.h"
#include "NonRigidAlignment/CorrespondenceFinder.h"

#include "vnl/algo/vnl_svd.h"

namespace AWT
{
   template <class T>
   class Fragment : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<Fragment<T>> P;

   protected:
      Fragment( typename Mesh<T>::P mesh, SamplePoints::P sampPts )
      {
         m_Mesh = mesh;
         m_SamplePoints = sampPts;

         m_NormalCalculator = AreaAveragedNormalCalculator<T>::getInstance( );

         // Make sure that we've got normals
         if ( *mesh->getVertexNormals( ) == 0 || *mesh->getFaceNormals( ) == 0 )
            m_NormalCalculator->calculateNormalsAndSet( mesh );

         m_InitialAugmented.set_identity( );
         m_Augmented.set_identity( );

         m_InitialPoseSet = false;

         T vtx[3];
         m_OriginalSum[0] = 0;
         m_OriginalSum[1] = 0;
         m_OriginalSum[2] = 0;

         //for ( MeshIndex v = 0; v < mesh->getNumberOfVertices( ); ++v )
         MESH_EACHVERTEX( mesh, v )
         {
            mesh->getVertex( v, vtx );

            m_OriginalSum[0] += vtx[0];
            m_OriginalSum[1] += vtx[1];
            m_OriginalSum[2] += vtx[2];
         }
      }

      virtual ~Fragment( )
      {
      }

   public:
      static P getInstance( typename Mesh<T>::P mesh, SamplePoints::P sampPts );

      virtual std::string getClassName( ) const;

      void getPose( typename Pose<T>::P pose ) const
      {
         (*pose)->setMatrix( m_Augmented );
      }

      bool isInitialPoseSet( ) const
      {
         return m_InitialPoseSet;
      }

      void setInitialPose( typename Pose<T>::P pose )
      {
         typename Pose<T>::PoseMatrix matx = pose->getMatrix( );
         
         for ( int r = 0; r < 3; ++r )
            for ( int c = 0; c < 4; ++c )
               m_InitialAugmented(r,c) = matx(r,c);

         // This brings it back into the untransformed frame, 
         // then transforms it into the new frame
         typename Pose<T>::PoseMatrix transform = m_InitialAugmented * vnl_svd<T>( m_Augmented ).inverse( );

         // This is checking to see whether we are sufficiently away from
         // identity to actually bother transforming
         bool flag = false;
         for ( int r = 0; r < 4 && !flag; ++r )
            for ( int c = 0; c < 4 && !flag; ++c )
               flag = abs( transform[r][c] - ( (r==c) ? 1 : 0 ) ) > 1e-6;

         if ( flag )
         {
            Pose<T>::P pose = Pose<T>::getInstance( );
            pose->setMatrix( transform );
            applyTransformation( pose );
         }

         m_Augmented = m_InitialAugmented;

         m_InitialPoseSet = true;
      }

      void getInitialPose( typename Pose<T>::P pose )
      {
         (*pose)->setMatrix( m_InitialAugmented );
      }

      typename Mesh<T>::P getMesh( )
      {
         return m_Mesh;
      }

      void createCorrespondenceFinder( Mesh<T>* referenceMesh )
      {
         SampledMesh<T>::P sampMesh = SampledMesh<T>::getInstance( m_Mesh, m_SamplePoints );
         
         // Shouldn't really need to do this - should just be able to transform normals by rotation
         // Mind you, it doesn't take very long compared to other stuff, so I'm not really going to worry.
         m_NormalCalculator->calculateNormalsAndSet( *m_Mesh );
         
         CorrespondenceFinder<T>::P corrFinder = CorrespondenceFinder<T>::getInstance( sampMesh, referenceMesh );
         corrFinder->update( );

         m_CorrFinder = corrFinder;
      }

      CorrespondenceFinder<T>* getCorrespondenceFinder( )
      {
         return *m_CorrFinder;
      }

      void updateRigid( )
      {
         typename Pose<T>::P pose = Pose<T>::getInstance( );
         m_CorrFinder->getPose( false, *pose );

         applyTransformation( pose );
      }

      static void calculateRegistrationError( typename Fragment<T>::P fragA, typename Fragment<T>::P fragB, T& angularError, T& translationalError )
      {
         Pose<T>::P poseA = Pose<T>::getInstance( ), poseB = Pose<T>::getInstance( );

         fragA->getPose( poseA );
         fragB->getPose( poseB );

         // Get the relative pose between fragments
         typename Pose<T>::PoseMatrix relPose = poseA->getRelativePose( poseB )->getMatrix( );

         // Tr = 1 + 2*cos(theta)
         angularError = acos( ( relPose[0][0] + relPose[1][1] + relPose[2][2] - 1 ) / 2 );

         // Now to get the translational error, calculate the mean of the original frag positions, project for each
         // and calculate the error - this assumes that the two fragments were loaded in alignment, which is true
         // in synthetic cases
         T mean[3];

         for ( int i = 0; i < 3; ++i )
         {
            mean[i] = ( fragA->m_OriginalSum[i] + fragB->m_OriginalSum[i] ) 
                     / ( fragA->getMesh( )->getNumberOfVertices( ) + fragB->getMesh( )->getNumberOfVertices( ) );
         }

         T projA[3], projB[3];
         poseA->project( mean, projA );
         poseB->project( mean, projB );

         T tmp = 0;
         translationalError = 0;
         for ( int i = 0; i < 3; ++i )
         {
            tmp = projA[i] - projB[i];
            translationalError += tmp*tmp;
         }

         translationalError = sqrt( translationalError );
      }

      void applyTransformation( const typename Pose<T>::P pose )
      {
         //AWT::Drawable::acquireMutex( this );

         vnl_vector<T> pntOrig( 3 );
         T arrOrig[3];
         T arrNew[3];

         Mesh<T>::P mesh = getMesh( );

         // Update the correspondence finder
         // Need to do this first or it messes up the lazy evaluation
         if ( *m_CorrFinder != 0 )
         {
            MeshIndex idx;

            T sourceVtx[3];
            T targetVtx[3];

            T projSourceVtx[3];

            for ( MeshIndex v = 0; v < m_SamplePoints->getNumberOfSamples( ); ++v )
            {
               m_CorrFinder->getCorrespondence( v, sourceVtx, targetVtx, idx );
               mesh->getVertex( m_SamplePoints->getSampleIndex( v ), sourceVtx );
               pose->project( sourceVtx, projSourceVtx, 1 );
               m_CorrFinder->setCorrespondence( v, projSourceVtx, targetVtx, idx );
            }
         }

         if ( !mesh->hasVertexNormals( ) || !mesh->hasFaceNormals( ) )
         {
            m_NormalCalculator->calculateNormalsAndSet( mesh );
         }

         Tuples<T>::P vertexNormals = mesh->getVertexNormals( );
         Tuples<T>::P faceNormals   = mesh->getFaceNormals( );

         //for ( MeshIndex v = 0; v < mesh->getNumberOfVertices( ); ++v )
         MESH_EACHVERTEX( mesh, v )
         {
            mesh->getVertex( v, arrOrig );
            pose->project( arrOrig, arrNew, 1 );
            
            mesh->setVertex( v, arrNew );
         }

         // Update the vertex normals
         //for ( MeshIndex v = 0; v < mesh->getNumberOfVertices( ); ++v )
         MESH_EACHVERTEX( mesh, v )
         {
            vertexNormals->getPoint( v, arrOrig );
            pose->project( arrOrig, arrNew, 0 );

            vertexNormals->setPoint( v, arrNew );
         }

         // Update the face normals
         //for ( MeshIndex f = 0; f < mesh->getNumberOfFaces( ); ++f )
         MESH_EACHFACE( mesh, f )
         {
            faceNormals->getPoint( f, arrOrig );
            pose->project( arrOrig, arrNew, 0 );

            faceNormals->setPoint( f, arrNew );
         }

         // Accumulate the rotation
         m_Augmented = pose->getMatrix( ) * m_Augmented;

         //AWT::Drawable::releaseMutex( this );
      }

   private:
      vnl_matrix_fixed<T,4,4> m_Augmented;
      vnl_matrix_fixed<T,4,4> m_InitialAugmented;

      T                                   m_OriginalSum[3];

      bool                                m_InitialPoseSet;

      typename Mesh<T>::P                 m_Mesh;
      SamplePoints::P                     m_SamplePoints;
      typename CorrespondenceFinder<T>::P m_CorrFinder;
      typename NormalCalculator<T>::P     m_NormalCalculator;
   };
}

#endif // __FRAGMENT_H__