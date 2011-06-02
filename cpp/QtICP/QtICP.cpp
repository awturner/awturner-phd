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
#include "QtForm/include/QtForm.h"

#include "DrawableFramework/DrawableFactory.h"
#include "Useful/HueWheelSequence.h"

#include "GenericICP/VanillaICPStateMachine.h"

#include "Mesh/Mesh.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/TuplesFunctions.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/AreaAveragedNormalCalculator.h"

#include "Useful/ArrayVectorFunctions.h"
#include "Useful/Noise.h"
#include "Useful/PrintMacros.h"

#include "vnl/algo/vnl_matrix_inverse.h"

#include "vnl/vnl_det.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector_fixed.h"

#include <QApplication.h>

#include "DeformableMesh/DeformableMeshImpl.h"


typedef double T;
typedef vnl_matrix_fixed<T,4,4> TMatrix;
typedef vnl_vector_fixed<T,4>   TVector;

using namespace AWT;

const T PIBY180 = atan( 1.0 ) * 4 / 180.0;

TMatrix randomTransformation( Mesh<T>::P mesh, T thetaMax, T traMax )
{
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

   // Rotate around the centroid
   T centroid[3];
   MeshFunctions<T>::calculateFaceCentroid( mesh, centroid );

   return TuplesFunctions<T>::rotationTranslationAboutPoint( axisRot, theta, centroid, axisTra );
}

using namespace AWT;
typedef double T;

// Build with the GUI.  Comment out oth.
#define USEQT

int main( int argc, char** argv )
{
   try
   {
#ifdef USEQT
      QApplication app( argc, argv );
      QtForm::P mainForm = QtForm::getInstance( 1, 1 );
      DrawableWorld::P world = mainForm->getViewer( 0 )->getDrawableWorld( );
      DrawableFactory<T>::P fact = world->getFactory( );
#endif

      StateMachineStack::P sms = StateMachineStack::getInstance( );
      VanillaICPStateMachine<T>::P sm = VanillaICPStateMachine<T>::getInstance( sms );

#ifdef USEQT
      mainForm->setStateMachine( sm );
#endif

      const int STATE_FRAGMENT = 1;
      const int STATE_MODE = 2;

      int state = STATE_FRAGMENT;

      int nfrags = 0;
      DeformableMeshImpl<T>::P defMesh = DeformableMeshImpl<T>::P( 0 );

      Noise<T>::timeSeed( );

      std::vector< DrawableMesh<T>::P > drawables;

      for ( int i = 1; i < argc; ++i )
      {
         if ( strcmp( argv[i], "-frag" ) == 0 )
         {
            state = STATE_FRAGMENT;
         }
         else if ( strcmp( argv[i], "-mode" ) == 0 )
         {
            state = STATE_MODE;
         }
         else
         {
            switch ( state )
            {
            case STATE_FRAGMENT:
               {
                  DEBUGMACRO( "Loading fragment from " << argv[i] );

                  Mesh<T>::P sourceMesh = VTKMeshLoader<T>::load( argv[i], true );
                  PRINTVBL( sourceMesh->getNumberOfVertices( ) );

#ifdef USEQT
                  double centroid[3];
                  double vtx[3];
                  MeshFunctions<T>::calculateFaceCentroid( sourceMesh, centroid );
                  MESH_EACHVERTEX( sourceMesh, v )
                  {
                     sourceMesh->getVertex( v, vtx );

                     for ( int ax = 0; ax < 3; ++ax )
                        vtx[ax] -= centroid[ax];

                     sourceMesh->setVertex( v, vtx );
                  }
#endif

                  AreaAveragedNormalCalculator<T>::getInstance( )->calculateNormalsAndSet( sourceMesh );

                  TMatrix R = randomTransformation( sourceMesh, 10, 0 );
                  MeshFunctions<T>::transformMesh( sourceMesh, R );

                  // Add this fragment into the state machine
                  sm->addFragment( sourceMesh );

                  // Recover the angle
                  const T trace = R(0,0) + R(1,1) + R(2,2);
                  const T theta = acos( ( trace - 1 ) / 2 );

                  PRINTVBL( theta / PIBY180 );

#ifdef USEQT
                  DEBUGLINE;
                  DrawableMesh<T>::P dfrag = fact->createMesh( sourceMesh );
                  dfrag->setMaterial( fact->createColour( .5f, 1.f, .5f, 1.f, true ) );
                  drawables.push_back( dfrag );

                  char buffer[256];
                  sprintf_s( buffer, "Fragment Mesh %d", ++nfrags );

                  world->add( buffer, dfrag );
#endif
               }
               break;

            case STATE_MODE:
               {
                  if ( *defMesh == 0 )
                  {
                     DEBUGMACRO( "Loading mean shape from " << argv[i] );
                     // If this is the first specified mode, this is the mean shape
                     Mesh<T>::P mode0 = VTKMeshLoader<T>::load( argv[i], false );
                     AreaAveragedNormalCalculator<T>::getInstance( )->calculateNormalsAndSet( mode0 );

                     defMesh = DeformableMeshImpl<T>::getInstance( mode0 );
                     sm->setDeformableMesh( defMesh );
                  }
                  else
                  {
                     DEBUGMACRO( "Adding a mode from " << argv[i] );
                     // Otherwise, it's just a mode
                     Mesh<T>::P modeMesh = VTKMeshLoader<T>::load( argv[i], false );
                     defMesh->addMode( modeMesh->getVertices( ) );
                  }
               }
               break;
            }
         }
      }

#ifndef USEQT
      while ( sm->isRunning( ) && !sm->isError( ) )
         sm->step( );

#else

      int ret;
      {
         DrawableMesh<T>::P dintact = fact->createMesh( defMesh );
         dintact->setMaterial( fact->createColour( 1.f, .5f, .5f, 1.f, true ) );
         world->add( "Intact Mesh", dintact );
         drawables.push_back( dintact );

         dintact = fact->createMesh( sm->getRemainingMesh( ) );
         world->add( "Remaining Mesh", dintact );
         drawables.push_back( dintact );
      }

      HueWheelSequence::P hws = HueWheelSequence::getInstance( drawables.size( ) );
      float col[4];
      for ( int i = 0; i < drawables.size( ); ++i )
      {
         hws->nextColour( col );

         if ( i == drawables.size()-1 )
            col[3] = 0.5f;

         drawables[i]->setMaterial( fact->createColour( col[0], col[1], col[2], col[3], true ) );
      }

      mainForm->showMinimized( );
    
      // Set the ball rolling
      mainForm->actionStartTriggered( );

      ret = app.exec( );

      PRINTVBL( ret );

      return ret;
#endif
   }
   catch ( std::exception ex )
   {
      PRINTVBL( ex.what() );
      throw ex;
   }
}