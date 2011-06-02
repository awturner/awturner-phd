#ifndef __NONRIGIDALIGNMENTLISTENER_H__
#define __NONRIGIDALIGNMENTLISTENER_H__

#include "ObjectManagement/ManagedObject.h"

#include "NonRigidAlignment/NonRigidAlignment.h"

namespace AWT
{
   template <class T>
   class NonRigidAlignment;

   template <class T>
   class NonRigidAlignmentListener : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<NonRigidAlignmentListener<T>> P;

   public:
      // Fired when the fixed mesh is set
      virtual void fixedMeshSet( NonRigidAlignment<T>* nra ) = 0;

      // Fired when the moving mesh is set
      virtual void movingMeshSet( NonRigidAlignment<T>* nra ) = 0;

      // Fired when the set of correspondences between the fixed and moving meshes is found
      virtual void correspondencesFound( NonRigidAlignment<T>* nra ) = 0;

      // Fired when there is an update of the rigid parameters
      virtual void rigidParameterUpdate( NonRigidAlignment<T>* nra ) = 0;

      // Fired when there is an update of the non-rigid parameters
      virtual void nonRigidParameterUpdate( NonRigidAlignment<T>* nra ) = 0;

      // Fired when the control points change
      virtual void controlPointsChanged( NonRigidAlignment<T>* nra ) = 0;

      // Fired when the alignment starts
      virtual void alignmentStart( NonRigidAlignment<T>* nra ) = 0;

      // Fired when the alignment completes
      virtual void alignmentComplete( NonRigidAlignment<T>* nra ) = 0;
   };
}

//#include "ObjectManagement/ManagedObjectSet.h"
//namespace AWT
//{
//   template <class T>
//   class CompositeNonRigidAlignmentListener : public CompositeListener<typename NonRigidAlignmentListener<T>,NonRigidAlignment<T>>
//   {
//   public:
//      virtual std::string getClassName( ) const { return "AWT::CompositeNonRigidAlignmentListener<T>"; }
//   };
//}

#endif // __NONRIGIDALIGNMENTLISTENER_H__