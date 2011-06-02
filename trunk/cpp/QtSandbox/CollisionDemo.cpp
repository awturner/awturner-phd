#include "CollisionDemo.h"

#include "DrawableFramework/DrawableWorld.h"
#include "DrawableFramework/DrawableFactory.h"
#include "ObjectEKDTree/AxisAlignedBoundingBox.h"

#include "MeshSearching/BoxQueries.h"

#include "QtForm/include/QtForm.h"

typedef double T;

using namespace AWT;

void AWT::collisionDemo( AWT::QtForm* form )
{
   DrawableWorld::P drawWorld = form->getViewer(0)->getDrawableWorld( );

   drawWorld->clear( );

   AWT::DrawableFactory<T>::P fact( drawWorld->getFactory( ) );

   AWT::AxisAlignedBoundingBoxImpl<T,3>::P boxA = AWT::AxisAlignedBoundingBoxImpl<T,3>::getInstance( );
   AWT::AxisAlignedBoundingBoxImpl<T,3>::P boxB = AWT::AxisAlignedBoundingBoxImpl<T,3>::getInstance( );

   boxA->setMinimumBound( 0, -10 ); boxA->setMinimumBound( 1, -20 ); boxA->setMinimumBound( 2, -5 );
   boxA->setMaximumBound( 0,  -5 ); boxA->setMaximumBound( 1,  20 ); boxA->setMaximumBound( 2,  5 );

   boxB->setMinimumBound( 0,   5 ); boxB->setMinimumBound( 1, -20 ); boxB->setMinimumBound( 2, -5 );
   boxB->setMaximumBound( 0,  10 ); boxB->setMaximumBound( 1,  20 ); boxB->setMaximumBound( 2,  5 );

   float rgba[] = { 1, 0, 0, 1 };
   AWT::DrawMaterial::P matA( *fact->createColour( rgba, true ) );

   rgba[1] = 1;
   AWT::DrawMaterial::P matB( *fact->createColour( rgba, true ) );

   AWT::DrawableImpl::P drawBoxA = fact->createAABB( *AWT::AxisAlignedBoundingBox<T,3>::P( *boxA ) ); drawBoxA->setMaterial( matA );
   AWT::DrawableImpl::P drawBoxB = fact->createAABB( *AWT::AxisAlignedBoundingBox<T,3>::P( *boxB ) ); drawBoxB->setMaterial( matB );

   Pose<T>::P poseB;

   T axisA[] = { 0, 0, 1 };
   T centreA[] = { -7.5, 0, 0 };

   T axisB[] = { 0, 0, 1 };
   T centreB[] = { 7.5, 0, 0 };

   {
      AWT::Transformation<T>::P transf = fact->createTransformation( Drawable::P( drawBoxB.getData( ) ) );
      transf->getPose( )->rotateDeg( 0, axisB, centreB );

      axisB[0] = 1; axisB[1] = 0; axisB[2] = 0;
      transf->getPose( )->rotateDeg( 45, axisB, centreB );
   
      poseB = transf->getPose( );

      drawWorld->add( "Transformed", Drawable::P( transf.getData( ) ) );
   }

   T thetaA = 0;
   T thetaB = 90;

   while ( true )
   {
      T thetaMid = (thetaA+thetaB)/2;
      PRINTVBL( thetaMid );

      Pose<T>::P poseA = Pose<T>::getInstance( );
      poseA->rotateDeg( thetaMid, axisA, centreA );

      Pose<T>::P poseR = poseA->getRelativePose( poseB );

      bool intersect = AWT::BoxQueries<T>::boxesIntersect( AWT::AxisAlignedBoundingBox<T,3>::P( *boxA ), poseR/*poseA*/, 
         AWT::AxisAlignedBoundingBox<T,3>::P( *boxB ), Pose<T>::getInstance( )/*poseB*/ );
      
      if ( intersect )
         thetaB = thetaMid;
      else
         thetaA = thetaMid;

      if ( thetaB - thetaA < 1e-4 )
      {
         AWT::Transformation<T>::P transf = fact->createTransformation( Drawable::P( drawBoxA.getData( ) ) );
         //transf->setPose( poseA );
         transf->getPose( )->rotateDeg( (thetaA+thetaB)/2, axisA, centreA );
         
         drawWorld->add( "Transformed", Drawable::P( transf.getData( ) ) );

         break;
      }

   }

   form->centreView( );
}