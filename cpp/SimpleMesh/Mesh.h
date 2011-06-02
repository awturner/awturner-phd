#ifndef __SIMPLEMESH_H__
#define __SIMPLEMESH_H__

#include "ObjectManagement/ManagedObject.h"

#include "TypeDefs.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class Mesh : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<Mesh> P;

      protected:
         Mesh( Index nverts, Index nf );
         Mesh( const Points& verts, const Faces& faces );
         virtual ~Mesh( );

      public:
         static P getInstance( Index nverts, Index nf );
         static P getInstance( const Points& verts, const Faces& faces );

         virtual std::string getClassName( ) const;

         const Index nv;
         const Index nf;

         Point getVertex( const Index i ) const;
         Face  getFace( const Index i ) const;

         Points& getVertices( ) const;
         TexCoords& getTexCoords( ) const;

         Faces&  getFaces( ) const;

      protected:
         struct D;
         D* m_D;
      };
   }
}

#endif // __SIMPLEMESH_H__