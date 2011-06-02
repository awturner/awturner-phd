#ifndef __FACESINRADIUSCALLBACK_H__
#define __FACESINRADIUSCALLBACK_H__

#include "ObjectEKDTree/OEKDTypeDefs.h"
#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   namespace OEKDTree
   {
      template <class T, unsigned char K>
      class OEKDTreeData;
   }

   template <class T>
   class FacesInRadiusCallback : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer< FacesInRadiusCallback<T> > P;

   public:
      virtual void faceFound( const typename OEKDTree::OEKDTreeData<T,3>::P data, const OEKDTree::ObjectIndex index ) = 0;
   };
}

#endif // __FACESINRADIUSCALLBACK_H__