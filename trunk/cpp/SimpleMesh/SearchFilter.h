#ifndef __SEARCHFILTER_H__
#define __SEARCHFILTER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh.h"
#include <vector>

namespace AWT
{
   namespace SimpleMesh
   {
      class SearchFilter : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<SearchFilter> P;

      protected:
         SearchFilter( );
         virtual ~SearchFilter( );

      public:
         static P getInstance( );
         std::string getClassName( ) const;

         bool handle( const Point p, const Index i );

      public:
         virtual bool check( const Point p, const Index i ) const;
         virtual void accept( const Point p, const Index i );
      };
   }
}

#endif // __SEARCHFILTER_H__