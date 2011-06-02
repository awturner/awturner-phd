#ifndef __MESHIO_H__
#define __MESHIO_H__

#include "Mesh.h"
#include "ShapeModel.h"

#include <iostream>

namespace AWT
{
   namespace SimpleMesh
   {
      class MeshIO
      {
      protected:
         MeshIO( );
         virtual ~MeshIO( );
      public:
         static Mesh::P loadMesh( std::istream& is );
         static ShapeModel::P loadModel( std::istream& is );

         static void saveMesh( std::ostream& os, Mesh::P mesh );

      protected:
         struct D;
         D* m_D;
      };
   }
}

#endif // __MESHIO_H__