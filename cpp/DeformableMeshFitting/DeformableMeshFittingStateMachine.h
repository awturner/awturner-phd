#ifndef __DEFORMABLEMESHFITTINGSTATEMACHINE_H__
#define __DEFORMABLEMESHFITTINGSTATEMACHINE_H__

#include "StateMachine/SaveableStateMachine.h"

#include "DeformableMeshFittingTypeDefs.h"

#include "DeformableMeshFitting.h"
#include "Mesh/MeshSimilarity.h"
#include "Mesh/Pose.h"

#include "StateMachine/StateMachineStack.h"

namespace AWT
{
   template <class T>
   class DeformableMeshFittingStateMachine : public SaveableStateMachine
   {
   public:
      typedef ManagedAutoPointer< DeformableMeshFittingStateMachine<T> > P;

   protected:
      DeformableMeshFittingStateMachine( StateMachineStack::P stack );
      virtual ~DeformableMeshFittingStateMachine( );

   public:
      static P getInstance( StateMachineStack::P stack );

      virtual std::string getClassName( ) const;

      virtual StateMachineStack* getStack( );

      virtual bool step( );

      virtual std::string getStateName( ) const;

      virtual bool isRunning( ) const;

      virtual bool isError( ) const;

      virtual std::string getLastError( ) const;

      virtual void setMoghariFilename( const T radius, const std::string& filename );

      virtual typename DeformableMeshFitting<T>::P getDeformableMeshFitting( );

      virtual void setDeformableFilename( std::string& filename );

      virtual TargetIndex getNumberOfTargetFilenames( ) const;

      virtual void addTargetFilename( std::string& filename );

      virtual void setInitialPose( const TargetIndex i, typename Pose<T>::P pose );

      virtual void setInitialParameters( const T* params );

      virtual bool load( const std::string& filename );

      virtual void save( const std::string& filename );

      virtual std::string getMagic( ) const;

      virtual void evalOnly( );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __DEFORMABLEMESHFITTINGSTATEMACHINE_H__