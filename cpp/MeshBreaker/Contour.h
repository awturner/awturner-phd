/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
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