#ifndef __MESHEDGESEARCHFILTER_H__
#define __MESHEDGESEARCHFILTER_H__

#include "SearchFilter.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class MeshEdgeSearchFilter : public SearchFilter
      {
      public:
         typedef ManagedAutoPointer<MeshEdgeSearchFilter> P;

      protected:
         MeshEdgeSearchFilter( Mesh::P );
         virtual ~MeshEdgeSearchFilter( );

      public:
         static P getInstance( Mesh::P );
         std::string getClassName( ) const;

         virtual bool check( const Point p, const Index i ) const;
         
      protected:
         struct D;
         D* m_D;
      };
   }
}

#endif // __MESHEDGESEARCHFILTER_H__