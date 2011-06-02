#include "include/OpenGLWidgetMouseBehaviour.h"

#include "include/OpenGLWidget.h"

#include "Useful/PrintMacros.h"

AWT::OpenGLWidgetMouseBehaviour::OpenGLWidgetMouseBehaviour( )
{
   m_D = new SavedState;

   for ( unsigned int i = 0; i < 3; ++i )
      m_D->rotation[i] = m_D->orbit[i] = 0;

   m_D->mouse[0] = m_D->mouse[1] = 0;

   m_D->button = 0;
}

AWT::OpenGLWidgetMouseBehaviour::~OpenGLWidgetMouseBehaviour( )
{
   delete m_D;
}

const AWT::OpenGLWidgetMouseBehaviour::SavedState* AWT::OpenGLWidgetMouseBehaviour::getSavedState()
{
   return m_D;
}

bool AWT::OpenGLWidgetMouseBehaviour::mousePressed( OpenGLWidget* widget, int x, int y, int button, int keys )
{
   double rot[3], orb[3];

   widget->getRotation( rot );
   widget->getOrbitPoint( orb );
   for ( unsigned int i = 0; i < 3; ++i )
   {
      m_D->rotation[i] = rot[i];
      m_D->orbit[i] = orb[i];
   }

   m_D->mouse[0] = x;
   m_D->mouse[1] = y;

   m_D->distance = widget->getDistance( );

   m_D->button |= button;

   return false;
}

bool AWT::OpenGLWidgetMouseBehaviour::mouseReleased( OpenGLWidget* /*widget*/, int /*x*/, int /*y*/, int button, int keys )
{
   m_D->button &= ~button;

   return false;
}
