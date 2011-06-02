#ifndef __DEFORMABLEMESHFITTING_H__
#define __DEFORMABLEMESHFITTING_H__

#include "ObjectManagement/ManagedObject.h"

#include "DeformableMeshFittingTypeDefs.h"

#include "Mesh/Mesh.h"
#include "DeformableMesh/DeformableMesh.h"
#include "NonRigidAlignment/CorrespondenceFinder.h"
#include "DeformableMeshFittingListener.h"
#include "Mesh/Pose.h"
#include "Mesh/SamplePoints.h"
#include "Fragment.h"

namespace Mimlab
{
   namespace Matrices
   {
      template <class T>
      class Matrix;
   }
}

namespace AWT
{
   template <class T>
   class DeformableMeshFitting : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer< DeformableMeshFitting<T> > P;

   protected:
      DeformableMeshFitting( );
      virtual ~DeformableMeshFitting( );

   public:
      static P getInstance( );

      void setDeformableMesh( typename DeformableMesh<T>::P defMesh );
      
      void addTargetMesh( typename Mesh<T>::P mesh, SamplePoints::P sampPts );

      void getInitialPose( const TargetIndex i, typename Pose<T>::P pose );
      void setInitialPose( const TargetIndex i, typename Pose<T>::P pose );

      typename CorrespondenceFinder<T>::P getCorrespondences( const TargetIndex i );

      void calculateRegistrationError( const TargetIndex a, const TargetIndex b, T& angularError, T& translationalError );

      void setInitialParameters( const T* params );

      TargetIndex getNumberOfTargetMeshes( ) const;
      typename Fragment<T>::P getFragment( TargetIndex i );
      typename DeformableMesh<T>::P getDeformableMesh( );

      void updateCorrespondences( );

      void stepRigidUpdate( );

      void stepParameterUpdate( );

      virtual std::string getClassName( ) const;

      T getRmsError( ) const;
      T getMaxError( ) const;

      void getPose( const TargetIndex i, typename Pose<T>::P pose ) const;
      
      void addListener( typename DeformableMeshFittingListener<T>::P nral );
      void removeListener( typename DeformableMeshFittingListener<T>::P nral );

      void setLoading( const bool loading );

   protected:
      struct D;
      D* m_D;
   };
}

#endif //__DEFORMABLEMESHFITTING_H__