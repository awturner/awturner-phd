#ifndef __MESHALIGNMENTDATALISTENER_H__
#define __MESHALIGNMENTDATALISTENER_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   template <class T>
   class MeshAlignmentData;

   template <class T>
   class MeshAlignmentDataListener : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshAlignmentDataListener<T>> P;

   public:
      virtual void fixedPointsSet( typename MeshAlignmentData<T>::P mad ) = 0;
      virtual void movingPointsSet( typename MeshAlignmentData<T>::P mad ) = 0;
   };
}

//#include "ObjectManagement/ManagedObjectSet.h"
//namespace AWT
//{
//   template <class T>
//   class CompositeMeshAlignmentDataListener : public CompositeListener<typename MeshAlignmentDataListener<T>,MeshAlignmentData<T>>
//   {
//      virtual std::string getClassName( ) const { return "AWT::CompositeMeshAlignmentDataListener<T>"; }
//   };
//}

#endif // __MESHALIGNMENTDATALISTENER_H__