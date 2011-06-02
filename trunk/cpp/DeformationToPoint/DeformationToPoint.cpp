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

#include "Mesh/Mesh.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"

#include "NonRigidAlignment/ControlPoint.h"
#include "NonRigidAlignment/ControlPointSet.h"
#include "NonRigidAlignment/MeshAlignmentData.h"
#include "NonRigidAlignment/NonRigidAlignment.h"

#include "Useful/PrintMacros.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace AWT;

ControlPointSet<double>::P getControlPointSet( const char* filename )
{
   std::ifstream is( filename );

   double scale, pos[3];

   ControlPointSet<double>::P ret = ControlPointSet<double>::getInstance( );

   while ( true )
   {
      is >> pos[0] >> pos[1] >> pos[2] >> scale;

      if ( is.fail( ) )
         break;

      ControlPoint<double>::P cp = ControlPoint<double>::getInstance(pos, scale);
      ret->add( cp );
   }

   is.close( );

   PRINTVBL( ret->getNumberOfObjects( ) );
   return ret;
}

std::vector<ControlPointSet<double>::P> getModes( const char* filename, const unsigned int nmodes, const ControlPointSet<double>::P refCps )
{
   std::vector<ControlPointSet<double>::P> ret;

   // This creates the empty modes
   double pos[] = { 0, 0, 0 };
   double val[] = { 0, 0, 0 };
   for ( unsigned int i = 0; i < nmodes; ++i )
   {
      ControlPointSet<double>::P cps = ControlPointSet<double>::getInstance( );

      for ( unsigned int j = 0; j < refCps->getNumberOfObjects( ); ++j )
      {
         ControlPoint<double>::P cp = ControlPoint<double>::getInstance( refCps->getControlPointConst( j ) );
         cps->add( cp );
      }

      ret.push_back( cps );
   }

   std::ifstream is( filename );

   double value;

   for ( unsigned int i = 0; i < refCps->getNumberOfObjects( ); ++i )
   {
      for ( unsigned int axis = 0; axis < 3; ++axis )
      {
         for ( unsigned int m = 0; m < nmodes; ++m )
         {
            is >> value;

            ret[m]->getControlPoint( i )->setValue( axis, value );
         }
      }
   }

   is.close( );

   return ret;
}

int main( int argc, char** argv )
{
   const unsigned int nmodes         = atoi( argv[4] );

   Mesh<double>::P            mesh     = VTKMeshLoader<double>::load( argv[1] );
   ControlPointSet<double>::P cps    = getControlPointSet( argv[2] );

   std::vector<ControlPointSet<double>::P> modes = getModes( argv[3], nmodes, cps );

   NonRigidAlignment<double>::P nra = NonRigidAlignment<double>::getInstance( );

   nra->setSourceMesh( mesh );
   
   nra->setControlPoints( cps );

   // Firstly, set the mesh into its mean position
   double val[3];
   
   for ( MeshIndex v = 0; v < cps->getNumberOfObjects( ); ++v )
   {
      modes[0]->getControlPoint( v )->getValue( val );
      cps->getControlPoint( v )->setValue( val );
   }

   // This puts the points in the position of the mean
   nra->updateSourcePoints( );

   // This is a copy of the mesh in its mean position
   Mesh<double>::P meshCopy = MeshImpl<double>::getInstance( nra->getMeshAlignment( )->getFixedPoints( ) );

   VTKMeshWriter<double>::write( meshCopy, "meanshape.vtk" );

   double val2[3];
   for ( unsigned int m = 1; m < nmodes; ++m )
   {
      for ( MeshIndex v = 0; v < cps->getNumberOfObjects( ); ++v )
      {
         modes[0]->getControlPoint( v )->getValue( val );
         modes[m]->getControlPoint( v )->getValue( val2 );

         val[0] += val2[0];
         val[1] += val2[1];
         val[2] += val2[2];

         cps->getControlPoint( v )->setValue( val );
      }

      nra->updateSourcePoints( );

      Mesh<double>::P modeMesh = nra->getMeshAlignment( )->getFixedPoints( );

      std::stringstream fnBuffer;
      fnBuffer << "mode.";
      fnBuffer << std::right << std::setfill( '0' ) << std::setw( 3 ) << m;
      fnBuffer << ".dat";
      std::ofstream os( fnBuffer.str( ).c_str( ) );

      DEBUGMACRO( "Writing mode " << m << " to " << fnBuffer.str( ) );
      //for ( MeshIndex v = 0; v < modeMesh->getNumberOfVertices( ); ++v )
      MESH_EACHVERTEX( modeMesh, v )
      {
         // Work out how each point has moved, and write the delta as the mode information
         meshCopy->getVertex( v, val2 );
         modeMesh->getVertex( v, val );

         val[0] -= val2[0];
         val[1] -= val2[1];
         val[2] -= val2[2];

         os << val[0] << " " << val[1] << " " << val[2] << std::endl;
      }

      os.close( );
   }

   AWT::ReferenceCountedObject::report( );
}