#ifndef __GENERATORPROVIDER_H__
#define __GENERATORPROVIDER_H__

#include "ObjectManagement/ManagedObject.h"

#include "HalfEdge.h"
#include "HalfEdgeMesh.h"
#include <vector>
#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class GeneratorProvider : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<GeneratorProvider<T>> P;

      virtual unsigned int                  getNumberOfGeneratorPairs( ) const = 0;
      virtual const std::vector<HalfEdge*>& getGenerator( const unsigned int pair, const unsigned int i ) = 0;
      virtual typename Mesh<T>::P           getMesh( ) = 0;
      virtual HalfEdgeMesh::P               getHalfEdgeMesh( ) = 0;
   };
}

#endif // __GENERATORPROVIDER_H__