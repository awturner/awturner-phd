#ifndef __VTKMESHWRITER_H__
#define __VTKMESHWRITER_H__

#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class Mesh;

   template <class T>
   class VTKMeshWriter
   {
   public:
      static void write( typename Mesh<T>::P mesh, const char* filename );
   };
}

#endif // __VTKMESHWRITER_H__