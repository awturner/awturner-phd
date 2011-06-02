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
/*! \file
\brief Calculates Moghari's attribute vector (2nd Order)

Takes a mesh file and calculates Moghari's 9-point attribute vector
for each mesh vertex, as described in "Global registration of multiple 
bone fragments using statistical atlas models: feasibility experiments.", 
2008, Moghari MH, Abolmaesumi P.

The attribute vector is written to standard output.

*/

#include "Mesh/VTKMeshLoader.h"

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"
#include "DeformableMeshFitting/MoghariMeasure.h"

using namespace AWT;

typedef double T;

int main( int argc, char** argv )
{
   if ( argc < 3 )
   {
      std::cerr << "Usage: CalculateMoghari <input model filename> <radius>" << std::endl;
      DEBUGLINEANDEXIT( 1 );
   }

   T radius = static_cast<T>( atof( argv[2] ) );

   Mesh<T>::P           mesh    = VTKMeshLoader<T>::load( argv[1] );
   MoghariMeasure<T>::P mogMeas = MoghariMeasure<T>::getInstance( );
   
   Tuples<T>::P         as      = mogMeas->calculate( mesh, radius );

   T tuple[9];
   for ( MeshIndex v = 0; v < as->getNumberOfPoints( ); ++v )
   {
      as->getPoint( v, tuple );

      for ( int i = 0; i < 9; ++i )
         std::cout << tuple[i] << " ";

      std::cout << std::endl;
   }
}