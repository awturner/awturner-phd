#ifndef __NORMALSEARCHFILTER_H__
#define __NORMALSEARCHFILTER_H__

#include "SearchFilter.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class NormalSearchFilter : public SearchFilter
      {
      protected:
         NormalSearchFilter( );
         virtual ~NormalSearchFilter( );

      public:
         typedef ManagedAutoPointer<NormalSearchFilter> P;

         static P getInstance( );
         std::string getClassName( ) const;

         void setTestPoint( const Point p );
         Point getTestPoint( ) const;

         void setTestNormal( const Point p );
         Point getTestNormal( ) const;

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

#endif // __NORMALSEARCHFILTER_H__