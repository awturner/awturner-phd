#ifndef __SURFACEMESH_SURFACESEARCHER_H__
#define __SURFACEMESH_SURFACESEARCHER_H__

#include "ObjectEKDTree/OEKDNearestSearcher.h"

namespace AWT
{
   template <class T>
   class MeshSearch : public OEKDTree::OEKDNearestSearcher<T,3>
   {
      // Stub class, just to slightly obscure the fact that we are using an EKD Tree here
      // (user doesn't need to know this)
   };
}

#endif // __SURFACEMESH_SURFACESEARCHER_H__