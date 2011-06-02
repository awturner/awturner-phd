#ifndef __MULTISEARCHFILTER_H__
#define __MULTISEARCHFILTER_H__

#include "SearchFilter.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class MultiSearchFilter : public SearchFilter
      {
      public:
         typedef ManagedAutoPointer<MultiSearchFilter> P;

      protected:
         MultiSearchFilter( const unsigned int alloc );
         virtual ~MultiSearchFilter( );

      public:
         static P getInstance( const unsigned int alloc = 2 );
         std::string getClassName( ) const;
         
         virtual void addFilter( SearchFilter::P filter );

      public:
         virtual bool check( const Point p, const Index i ) const;
         virtual void accept( const Point p, const Index i );

         std::vector<SearchFilter::P> filters;
      };
   }
}

#endif // __MULTISEARCHFILTER_H__