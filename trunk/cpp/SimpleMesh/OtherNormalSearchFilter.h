#ifndef __OTHERNORMALSEARCHFILTER_H__
#define __OTHERNORMALSEARCHFILTER_H__

#include "SearchFilter.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class OtherNormalSearchFilter : public SearchFilter
      {
      protected:
         OtherNormalSearchFilter( Mesh::P mesh );
         virtual ~OtherNormalSearchFilter( );

      public:
         typedef ManagedAutoPointer<OtherNormalSearchFilter> P;

         static P getInstance( Mesh::P mesh );
         std::string getClassName( ) const;

         void setTestPoint( const Point p );
         Point getTestPoint( ) const;

         Mesh::P getTestMesh( ) const;

         void setCosineThreshold( const double v );
         double getCosineThreshold( ) const;

      public:
         virtual bool check( const Point p, const Index i ) const;

      protected:
         struct D;
         D* m_D;
      };
   }
}

#endif // __OTHERNORMALSEARCHFILTER_H__