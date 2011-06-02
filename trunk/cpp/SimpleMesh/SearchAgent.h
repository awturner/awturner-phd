#ifndef __SEARCHAGENT_H__
#define __SEARCHAGENT_H__

#include "ObjectManagement/ManagedObject.h"
#include "ObjectEKDTree/OEKDNearestSearcher.h"

#include "Mesh.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class SearchAgent : public OEKDNearestSearcher<double,3>
      {
      public:
         typedef ManagedAutoPointer<SearchAgent> P;

      public:
         Mesh::P getMesh( );
         void setMesh( Mesh::P mesh );

      protected:
         Mesh::P mesh;

      };
   }
}

#endif // __SEARCHAGENT_H__