#ifndef __SEARCHTRANSFORMED_H__
#define __SEARCHTRANSFORMED_H__

#include "ObjectManagement/ManagedObject.h"

#include "TypeDefs.h"
#include "Search.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class SearchTransformed : public Search
      {
      public:
         typedef ManagedAutoPointer<SearchTransformed> P;

      protected:
         SearchTransformed( Search::P search, Transformation t );
         virtual ~SearchTransformed( );

      public:
         static P getInstance( Search::P search, Transformation t );
         virtual std::string getClassName( ) const;

         void setTransformation( const Transformation& t );
         Transformation getTransformation( ) const;

         Search::P getSearch( );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __SEARCHTRANSFORMED_H__