#ifndef __CONTOUR_H__
#define __CONTOUR_H__

#include "ObjectManagement/ManagedObject.h"

#include "EdgeBuilder.h"

#include "Mesh/MeshTypeDefs.h"
#include "Mesh/Tuples.h"

#include <vector>
#include <ostream>

namespace AWT
{
   class Contour : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<Contour> P;

   protected:
      Contour( EdgeBuilder::P eb );

      virtual ~Contour( );

      static P getInstance( EdgeBuilder::P eb );

   public:
      static P getInstance( EdgeBuilder::P eb, const MeshIndex* vs, const unsigned int n = 3 );

      virtual std::string getClassName( ) const;

      // This method handles the "intelligent" insertion of the triangle into a
      // set of contours, i.e. it decides which is the active contour, as well as
      // handling splits and merges.  This assumes that the vector of contours
      // passed in is "well-behaved", i.e. essentially that it was created by
      // repeatedly calling this method, or that it is empty
      static void insert( std::vector<Contour::P>& contours, EdgeBuilder::P eb, MeshIndex* vs, bool isSplit );

      unsigned int getLength( ) const;

      Tuples<MeshIndex>::P getContour( );

      Edge* getEdge( );

      static void printVertices( std::ostream& os, std::vector<Contour::P>& contours );

      static void printContours( std::ostream& os, std::vector<Contour::P>& contours );

      static void renderGraph( const char* inputFilename, const char* outputFilename );

   protected:
      // Splices the passed contour into this contour
      void merge( P contour );

      bool getEdgeToVertex( MeshIndex v, Edge*& e );

      struct D;
      D* m_D;
   };
}

#endif // __CONTOUR_H__