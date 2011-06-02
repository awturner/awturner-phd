#include "SearchFilter.h"

#include "MeshFunctions.h"

using namespace AWT::SimpleMesh;

AWT::SimpleMesh::SearchFilter::SearchFilter()
{
}

AWT::SimpleMesh::SearchFilter::~SearchFilter()
{
}

AWT::SimpleMesh::SearchFilter::P AWT::SimpleMesh::SearchFilter::getInstance( )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::SearchFilter, ( ) );
}

GETNAMEMACRO( AWT::SimpleMesh::SearchFilter );

bool AWT::SimpleMesh::SearchFilter::handle( Point p, Index i )
{
   if ( check( p, i ) )
   {
      accept( p, i );
      return true;
   }
   else
   {
      return false;
   }
}

bool AWT::SimpleMesh::SearchFilter::check( Point p, Index i ) const
{
   // Accept by default
   return true;
}

void AWT::SimpleMesh::SearchFilter::accept( Point p, Index i )
{
   // Do nothing by default
}
