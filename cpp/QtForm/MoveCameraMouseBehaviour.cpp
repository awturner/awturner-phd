#include "include/MoveCameraMouseBehaviour.h"

#include "include/OpenGLWidget.h"

#include <cmath>

struct AWT::MoveCameraMouseBehaviour::D
{
};

AWT::MoveCameraMouseBehaviour::MoveCameraMouseBehaviour()
{
   m_D = new D();
}

AWT::MoveCameraMouseBehaviour::~MoveCameraMouseBehaviour()
{
   delete m_D;
}

AWT::MoveCameraMouseBehaviour::P AWT::MoveCameraMouseBehaviour::getInstance( )
{
   AUTOGETINSTANCE( AWT::MoveCameraMouseBehaviour, ( ) );
}

GETNAMEMACRO( AWT::MoveCameraMouseBehaviour );

bool AWT::MoveCameraMouseBehaviour::mouseMoved( OpenGLWidget* widget, int x, int y, int /*button*/, int keys )
{
   bool dorepaint     = false;

   const SavedState* state = getSavedState( );

   switch ( state->button )
   {
   case 1:
      {
         double newRotation[3];

         newRotation[0] = state->rotation[0] + ( y - state->mouse[1] );
         newRotation[1] = state->rotation[1];
         newRotation[2] = state->rotation[2] + ( x - state->mouse[0] );
         
         if ( keys == 0x2 )
         {
            newRotation[0] = (floor( newRotation[0] / 15 + 0.5 )) * 15;
            newRotation[1] = (floor( newRotation[1] / 15 + 0.5 )) * 15;
            newRotation[2] = (floor( newRotation[2] / 15 + 0.5 )) * 15;
         }

         // Wrap into the [-180..180) range
         for ( int i = 0; i < 3; ++i )
         {
            const float tmp = (newRotation[i] + 180.0)/360.0;
            newRotation[i] = ( tmp - floor( tmp ) - 0.5 ) * 360.0;
         }

         widget->setRotation( newRotation );
         dorepaint = true;
      }
      break;
   case 2:
      {
         //PRINTVBL( state->distance );
         //const double logDistance = log10( state->distance );
         //const double newDistance = pow( std::min<double>( logDistance + ( y - state->mouse[1] ) / 1000.0, 4 ), 10 );
         //PRINTVBL( newDistance );

         //widget->setDistance( newDistance );
         dorepaint = true;
      }
      break;
   case 4:
      {
         // This is the middle button translation
         double matx[16];
         double newOrbit[3];
         glGetDoublev( GL_MODELVIEW_MATRIX, matx );

         double dx = -( x - state->mouse[0] ) * state->distance / 1000;
         double dy =  ( y - state->mouse[1] ) * state->distance / 1000;

         newOrbit[0] = state->orbit[0] + matx[ 0]*dx + matx[ 1]*dy;
         newOrbit[1] = state->orbit[1] + matx[ 4]*dx + matx[ 5]*dy;
         newOrbit[2] = state->orbit[2] + matx[ 8]*dx + matx[ 9]*dy;

         widget->setOrbitPoint( newOrbit );
         dorepaint = true;
         break;
      }
   }

   //bool m = widget->updateMousePosition( x, y );
   //bool e = widget->updateEyePosition( );

   //dorepaint |= m || e;

   //if ( dorepaint )
   //   widget->repaint( );

   return dorepaint;
}