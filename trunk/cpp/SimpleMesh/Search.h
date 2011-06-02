#ifndef __SIMPLEMESHSEARCH_H__
#define __SIMPLEMESHSEARCH_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh.h"
#include "SearchFilter.h"

#include "SearchAgent.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class Search : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<Search> P;

      protected:
         Search( Mesh::P mesh, const bool useFaces );
         virtual ~Search( );

      public:
         static P getInstance( Mesh::P mesh, const bool useFaces );
         virtual std::string getClassName( ) const;

         Mesh::P getMesh( );

         virtual void search( SearchAgent::P agent );
         virtual PointIndexWeights search( const Point testPoint, const Point testNormal, SearchFilter::P filter );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __SIMPLEMESHSEARCH_H__