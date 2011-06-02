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
#include "SimpleMesh/ShapeModel.h"

#include <fstream>

using namespace AWT::SimpleMesh;

void readVector( int& i, char** argv, Vector& params )
{
   for ( Index m = 0; m < params.size(); ++m )
   {
      const char* modeweightStr = argv[++i];

      if ( strcmp( modeweightStr, "-" ) == 0 )
         break;

      params[m] = atof( modeweightStr );
   }

   PRINTVBL( params );
}

int main( int argc, char** argv )
{
   int i = 0;

   ShapeModel::P model = MeshIO::loadModel( std::ifstream( argv[++i] ) );

   Vector vec( model->getNumberOfModes() );
   vec.fill( 0 );
   vec[0] = 1;
   readVector( i, argv, vec );

   Mesh::P mesh = model->getMesh( vec );

   MeshIO::saveMesh( std::ofstream( argv[++i] ), mesh );
}