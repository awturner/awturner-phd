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

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_matrix_inverse.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_det.h"

#include "Mesh/VTKMeshLoader.h"
#include "Mesh/Mesh.h"
#include "Mesh/MeshFunctions.h"

#include "Useful/Noise.h"
#include "Useful/ArrayVectorFunctions.h"

#include "Useful/PrintMacros.h"

typedef double T;
typedef vnl_matrix_fixed<T,4,4> TMatrix;
typedef vnl_vector_fixed<T,4>   TVector;

using namespace AWT;

const T PIBY180 = atan( 1.0 ) * 4 / 180.0;

TMatrix randomTransformation( Mesh<T>::P mesh, T thetaMax, T traMax )
{
   TMatrix R;
   R.fill( 0 );

   const T theta = Noise<T>::randu( 0, thetaMax ) * PIBY180;
   PRINTVBL( theta / PIBY180 );

   T axisRot[3] = { 
      Noise<T>::randu( -1, 1 ),
      Noise<T>::randu( -1, 1 ),
      Noise<T>::randu( -1, 1 )
   };
   T axisTra[3] = {
      Noise<T>::randu( -1, 1 ),
      Noise<T>::randu( -1, 1 ),
      Noise<T>::randu( -1, 1 )
   };
   normalize( axisRot, 3 );

   normalize( axisTra, 3 );
   scale<T>( axisTra, traMax /*Noise<T>::randu( 0, 100 )*/, 3 );

   const T c   = cos(theta); 
   const T s   = sin(theta); 
   const T C   = 1-c;
   const T xs  = axisRot[0]*s;   
   const T ys  = axisRot[1]*s;   
   const T zs  = axisRot[2]*s;
   const T xC  = axisRot[0]*C;   
   const T yC  = axisRot[1]*C;   
   const T zC  = axisRot[2]*C;
   const T xyC = axisRot[0]*yC; 
   const T yzC = axisRot[1]*zC; 
   const T zxC = axisRot[2]*xC;

   R(0,0) = axisRot[0]*xC+c;
   R(0,1) = xyC-zs;
   R(0,2) = zxC+ys;

   R(1,0) = xyC+zs;
   R(1,1) = axisRot[1]*yC+c;
   R(1,2) = yzC-xs;

   R(2,0) = zxC-ys;
   R(2,1) = yzC+xs;
   R(2,2) = axisRot[2]*zC+c;

   R(3,3) = 1;
   PRINTVBL( vnl_det( R ) );

   // Rotate around the centroid
   T centroid[3];
   MeshFunctions<T>::calculateFaceCentroid( mesh, centroid );
   for ( int ax = 0; ax < 3; ++ax )
      R(ax,3) = axisTra[ax] + centroid[ax] - R(ax,0)*centroid[0] - R(ax,1)*centroid[1] - R(ax,2)*centroid[2];

   return R;
}

#include "VanillaICPStateMachine.h"

#if 0
int main( int argc, char** argv )
{
   Mesh<T>::P sourceMesh = VTKMeshLoader<T>::load( argv[1] );
   Mesh<T>::P targetMesh = VTKMeshLoader<T>::load( argv[2] );

   Noise<T>::timeSeed( );

   TMatrix R = randomTransformation( sourceMesh, 5, 0 );

   // Recover the angle
   const T trace = R(0,0) + R(1,1) + R(2,2);
   const T theta = acos( ( trace - 1 ) / 2 );

   PRINTVBL( theta / PIBY180 );

   // Calculate the location of the centroid of the source mesh
   T sourceCentroid[3];
   MeshFunctions<T>::calculateFaceCentroid( sourceMesh, sourceCentroid );

   MeshFunctions<T>::transformMesh( sourceMesh, R );

   StateMachineStack::P sms = StateMachineStack::getInstance( );
   VanillaICPStateMachine<T>::P sm = VanillaICPStateMachine<T>::getInstance( sms );

   sm->setFragmentMesh( sourceMesh );
   sm->setIntactMesh( targetMesh );

   /*
   while ( sm->isRunning( ) && !sm->isError( ) )
   {
      PRINTVBL( sm->getStateName( ) );
      sm->step( );

      {
         vnl_matrix_fixed<T,4,4> delta = vnl_matrix_inverse<T>( R ).inverse( ) * sm->getTransformation( );
         const T trace = delta(0,0) + delta(1,1) + delta(2,2);
         
         T calcCentroid[3];
         MeshFunctions<T>::calculateFaceCentroid( sourceMesh, calcCentroid );

         const T deltaTrans = deltaNorm( calcCentroid, sourceCentroid, 3 );
         const T deltaTheta = acos( ( trace - 1 ) / 2 );

         PRINTVBL( deltaTrans );
         PRINTVBL( deltaTheta );
      }
   }

   if ( sm->isError( ) )
   {
      PRINTVBL( sm->getStateName( ) );
      PRINTVBL( sm->getLastError( ) );
   }

   */  
}
#endif