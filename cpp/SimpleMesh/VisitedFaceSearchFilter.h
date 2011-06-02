#ifndef __VISITEDFACESEARCHFILTER_H__
#define __VISITEDFACESEARCHFILTER_H__

#include "SearchFilter.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class VisitedFaceSearchFilter : public SearchFilter
      {
      public:
         typedef ManagedAutoPointer<VisitedFaceSearchFilter> P;

      protected:
         VisitedFaceSearchFilter( Mesh::P );
         virtual ~VisitedFaceSearchFilter( );

      public:
         static P getInstance( Mesh::P );
         std::string getClassName( ) const;

         void reset( );

         void markFaces( );

         virtual bool check( const Point p, const Index i ) const;
         virtual void accept( const Point p, const Index i );

         void writeUnvisitedMesh( const char* filename );

      protected:
         struct D;
         D* m_D;
      };
   }
}

#endif // __VISITEDFACESEARCHFILTER_H__