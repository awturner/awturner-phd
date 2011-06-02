#ifndef __EDGE_H__
#define __EDGE_H__

#include "Mesh/MeshTypeDefs.h"
#include "ObjectManagement/ManagedObject.h"

#include "EdgeBuilder.h"

namespace AWT
{
   class Edge : public ManagedObject
   {
      friend class EdgeBuilder;

   public:
      typedef ManagedAutoPointer<Edge> P;

   protected:
      Edge( EdgeBuilder::P eb, const MeshIndex vertex );
      virtual ~Edge( );
   
   private:
      static P getInstance( EdgeBuilder::P eb, const MeshIndex vertex );

   public:
      const MeshIndex getVertex( ) const;

      std::string getClassName( ) const;

      Edge* getPrevious( );
      void setPrevious( Edge* e );

      Edge* getNext( );
      void setNext( Edge* e );

      void destroy( );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __EDGE_H__