#ifndef __VANILLAICPSTATEMACHINE_H__
#define __VANILLAICPSTATEMACHINE_H__

#include "StateMachine/SaveableStateMachine.h"

#include "Mesh/Mesh.h"
#include "DeformableMesh/DeformableMesh.h"

#include "StateMachine/StateMachineStack.h"

namespace AWT
{
   template <class T>
   class VanillaICPStateMachine : public SaveableStateMachine
   {
   public:
      typedef ManagedAutoPointer<VanillaICPStateMachine<T>> P;

   protected:
      VanillaICPStateMachine( StateMachineStack::P stack );
      virtual ~VanillaICPStateMachine( );

   public:
      static P getInstance( StateMachineStack::P stack );
      virtual std::string getClassName( ) const;

      virtual StateMachineStack* getStack( );

      virtual bool step( );

      virtual bool isRunning( ) const;

      virtual bool isError( ) const;

      virtual std::string getLastError( ) const;

      virtual std::string getStateName( ) const;

      virtual void addFragment( typename Mesh<T>::P mesh );
      virtual typename Mesh<T>::P getFragment( MeshIndex i );

      virtual void setDeformableMesh( typename DeformableMesh<T>::P mesh );
      virtual typename DeformableMesh<T>::P getDeformableMesh( );

      virtual typename Mesh<T>::P getRemainingMesh( );

      virtual typename Tuples<T>::P getFragmentPoints( MeshIndex i );
      virtual typename Tuples<T>::P getIntactPoints( MeshIndex i );

      virtual vnl_matrix_fixed<T,4,4> getTransformation( ) const;
      virtual vnl_vector<T> getModeWeights( ) const;

      virtual T getRms( ) const;
      virtual T getMaxParallelDistance( ) const;
      virtual T getMaxPerpendicularDistance( ) const;

      virtual std::string getMagic( ) const { return ""; }

      virtual bool load( const std::string& /*filename*/ ) { return false; }

      virtual void save( const std::string& /*filename*/ ) { }
   protected:
      struct D;
      D* m_D;

   };
}

#endif // __VANILLAICPSTATEMACHINE_H__