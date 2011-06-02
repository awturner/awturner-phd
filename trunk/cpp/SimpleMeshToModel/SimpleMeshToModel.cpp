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
#include "SimpleMesh/Mesh.h"
#include "SimpleMesh/MeshIO.h"
#include "SimpleMesh/MeshFunctions.h"

#include <vector>
#include <fstream>
#include "vnl/algo/vnl_svd.h"

using namespace AWT::SimpleMesh;

void translatePoints( Points& vs, const Point t )
{
   for ( Index i = 0; i < vs.cols(); ++i )
      vs.set_column( i, vs.get_column( i ) + t );
}

int main( int argc, char** argv )
{
   typedef std::vector<Mesh::P> Meshes;
   Meshes meshes;

   std::string outFilename( argv[1] );

   DEBUGMACRO( "Loading meshes" );

   for ( int i = 2; i < argc; ++i )
   {
      Mesh::P mesh = MeshIO::loadMesh( std::ifstream( argv[i] ) );

      if ( !meshes.empty() && mesh->getFaces() != meshes[0]->getFaces() )
         throw "Different connectivity!";

      meshes.push_back( mesh );
   }
   
   const Faces& meanFaces = meshes[0]->getFaces();

   // Calculate the mean shape
   Points meanPoints( 4, meshes[0]->nv );
   meanPoints.fill( 0 );

   DEBUGMACRO( "Calculating mean mesh" );

   // Accumulate the vertices
   for ( Meshes::iterator it = meshes.begin(), en = meshes.end(); it != en; ++it )
   {
      meanPoints += (*it)->getVertices();
   }

   meanPoints /= meshes.size();

   /*
   // Translate so that the centroid of the model is at the origin
   Point minusmeanCentroid = -calculateCentroid( Mesh::getInstance( meanPoints, meanFaces ) );
   translatePoints( meanPoints, minusmeanCentroid );

   for ( Meshes::iterator it = meshes.begin(), en = meshes.end(); it != en; ++it )
   {
      Matrix& verts = (*it)->getVertices();
      translatePoints( verts, minusmeanCentroid );
   }
   */

   DEBUGMACRO( "Calculating mean-shifted shape vector" );

   // Calculate the mean-centred matrix
   Matrix Y( meanPoints.cols() * 4, static_cast<Index>( meshes.size() ) );
   for ( Index i = 0; i < meshes.size(); ++i )
   {
      Matrix p = meshes[i]->getVertices() - meanPoints;

      // Doesn't matter which order this is, provided data_block is used to unfold it again!
      Y.set_column( i, p.data_block() );
   }

   DEBUGMACRO( "Extracting modes" );

   vnl_svd<double> svd( Y );

   Matrix U = svd.U();
   Matrix S = svd.W();

   Matrix modes = U * S;

   DEBUGMACRO( "Model constructed.  Writing to " << outFilename );

   Vector s = svd.W().diagonal();
   const double totalS = s.sum();

   double sumS = 0;
   unsigned int takeM = 0;
   while ( takeM < S.cols() )
   {
      sumS += s(takeM++);

      if ( sumS > 0.97*totalS )
         break;
   }

   DEBUGMACRO( "Only taking " << takeM << " extra modes for 97%.  (Discarding " << (S.cols()-1-takeM) << ")" );

   std::ofstream os( outFilename.c_str() );

   DEBUGMACRO( "\t" << "Writing mean mode" );
   os << "v" << "\t" << meanPoints.cols() << std::endl;
   for ( Index i = 0; i < meanPoints.cols(); ++i )
      os << meanPoints(0,i) << "\t" << meanPoints(1,i) << "\t" << meanPoints(2,i) << std::endl;

   // Now write the modes
   Matrix mode( 4, meanPoints.cols() );
   for ( Index m = 0; m < takeM; ++m )
   {
      DEBUGMACRO( "\t" << "Writing mode " << (m+1) );
      mode.set( modes.get_column(m).data_block() );

      os << "v" << "\t" << mode.cols() << std::endl;
      for ( Index i = 0; i < mode.cols(); ++i )
         os << mode(0,i) << "\t" << mode(1,i) << "\t" << mode(2,i) << std::endl;
   }

   DEBUGMACRO( "\t" << "Writing meanFaces" );
   os << "f" << "\t" << meanFaces.cols() << std::endl;
   for ( Index i = 0; i < meanFaces.cols(); ++i )
      os << meanFaces(0,i) << "\t" << meanFaces(1,i) << "\t" << meanFaces(2,i) << std::endl;

   DEBUGMACRO( "Done." );
}