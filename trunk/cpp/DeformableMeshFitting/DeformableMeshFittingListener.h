#ifndef __DEFORMABLEMESHFITTINGLISTENER_H__
#define __DEFORMABLEMESHFITTINGLISTENER_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   template <class T>
   class DeformableMeshFitting;

   template <class T>
   class DeformableMeshFittingListener : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer< DeformableMeshFittingListener<T> > P;

   public:
      // Fired when the set of correspondences between the fixed and moving meshes is found
      virtual void correspondencesFound( typename DeformableMeshFitting<T>::P nra ) = 0;
   };
}

//#include "ObjectManagement/ManagedObjectSet.h"
//namespace AWT
//{
//   template <class T>
//   class CompositeDeformableMeshFittingListener : public CompositeListener<DeformableMeshFittingListener<T>,DeformableMeshFitting<T>>
//   {
//   public:
//      typedef ManagedAutoPointer< CompositeDeformableMeshFittingListener<T> > P;
//
//   public:
//      virtual std::string getClassName( ) const { return "AWT::CompositeDeformableMeshFittingListener<T>"; }
//   };
//}

#endif // __DEFORMABLEMESHFITTINGLISTENER_H__
