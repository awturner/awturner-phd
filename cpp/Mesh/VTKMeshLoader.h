#ifndef __VTKMESHLOADER_H__
#define __VTKMESHLOADER_H__

class vtkAlgorithm;
class vtkPolyData;

#include "Mesh.h"

namespace AWT
{
   template <class T>
   class VTKMeshLoader
   {
   public:
      static typename Mesh<T>::P load( const char* filename, bool doClean = true );
      static typename Mesh<T>::P convert( vtkPolyData* polyAlg );

   protected:
      struct D;

      VTKMeshLoader( );
   };
}

#endif // __VTKMESHLOADER_H__