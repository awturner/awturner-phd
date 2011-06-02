#ifndef __DEFORMABLEMESHLOADER_H__
#define __DEFORMABLEMESHLOADER_H__

#include "DeformableMesh.h"

namespace AWT
{
   template <class T>
   class DeformableMeshLoader
   {
   public:
      static typename DeformableMesh<T>::P load( const char* filename );
   };
}

#endif // __DEFORMABLEMESHLOADER_H__