#include "MultiSearchFilter.h"

AWT::SimpleMesh::MultiSearchFilter::MultiSearchFilter( const unsigned int alloc )
{
   filters.reserve( alloc );
}

AWT::SimpleMesh::MultiSearchFilter::~MultiSearchFilter( )
{
}

AWT::SimpleMesh::MultiSearchFilter::P AWT::SimpleMesh::MultiSearchFilter::getInstance( const unsigned int alloc )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::MultiSearchFilter, ( alloc ) );
}

GETNAMEMACRO( AWT::SimpleMesh::MultiSearchFilter );

bool AWT::SimpleMesh::MultiSearchFilter::check( const Point p, const Index i ) const
{
   typedef std::vector<SearchFilter::P> Filters;

   for ( Filters::const_iterator it = filters.begin(), en = filters.end(); it != en; ++it )
   {
      if ( !(*it)->check( p, i ) )
         return false;
   }

   return true;
}

void AWT::SimpleMesh::MultiSearchFilter::accept( const Point p, const Index i )
{
   typedef std::vector<SearchFilter::P> Filters;

   for ( Filters::iterator it = filters.begin(), en = filters.end(); it != en; ++it )
   {
      (*it)->accept( p, i );
   }
}

void AWT::SimpleMesh::MultiSearchFilter::addFilter( SearchFilter::P filter )
{
   filters.push_back( filter );
}