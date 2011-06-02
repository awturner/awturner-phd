#ifndef __EDGEBUILDER_H__
#define __EDGEBUILDER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/MeshTypeDefs.h"

namespace AWT
{
   class Edge;

   class EdgeBuilder : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<EdgeBuilder> P;

   protected:
      EdgeBuilder( );
      virtual ~EdgeBuilder( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      Edge* createEdge( const MeshIndex v );

      void destroy( Edge* edge );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __EDGEBUILDER_H__