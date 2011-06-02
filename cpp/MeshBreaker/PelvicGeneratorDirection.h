#ifndef __PELVICGENERATORDIRECTION_H__
#define __PELVICGENERATORDIRECTION_H__

#include "GeneratorProvider.h"

#include "HalfEdge.h"
#include "HalfEdgeVertex.h"

namespace AWT
{
   template <class T>
   class PelvicGeneratorDirection : public GeneratorProvider<T>
   {
   public:
      typedef ManagedAutoPointer<PelvicGeneratorDirection> P;

   protected:
      PelvicGeneratorDirection( typename GeneratorProvider<T>::P fg, const bool destructive );
      virtual ~PelvicGeneratorDirection( );

   public:
      static P getInstance( typename GeneratorProvider<T>::P fg, const bool destructive );
      virtual std::string getClassName( ) const;

      void getBasis( T* origin, T* x, T* y, T* z ) const;

      void normalizeMesh( int desiredSide, int desiredObGeneratorDirection, int desiredPubGeneratorDirection );

      typename Mesh<T>::P getMesh( );
      const std::vector< HalfEdge* >& getObturatorGenerator( );
      const std::vector< HalfEdge* >& getPubicGenerator( );

      const std::vector< HalfEdge* >& getGenerator( const unsigned int pair, const unsigned int i );
      unsigned int                    getNumberOfGeneratorPairs( ) const;
      HalfEdgeMesh::P                 getHalfEdgeMesh( );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __PELVICGENERATORDIRECTION_H__