#include "SimpleVTKAppFramework.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleSwitch.h"

namespace AWT
{
   SimpleVTKAppFramework::SimpleVTKAppFramework( )
   {
      m_Renderer                 = vtkRenderer::New( );
	   m_RenderWindow             = vtkRenderWindow::New( );
	   m_RenderWindowInteractor   = vtkRenderWindowInteractor::New( );

	   m_RenderWindow->AddRenderer(m_Renderer);
	   m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

      vtkInteractorStyleSwitch* iastyle = dynamic_cast<vtkInteractorStyleSwitch*>( m_RenderWindowInteractor->GetInteractorStyle( ) );
      if ( iastyle != 0 )
      {
         iastyle->SetCurrentStyleToTrackballCamera( );
      }
   }

   SimpleVTKAppFramework::~SimpleVTKAppFramework( )
   {
      m_RenderWindowInteractor->Delete( );
      m_RenderWindow->Delete( );
      m_Renderer->Delete( );
   }

   void SimpleVTKAppFramework::setWindowSize( int w, int h )
   {
      m_RenderWindow->SetSize( w, h );
   }

   vtkRenderer* SimpleVTKAppFramework::getRenderer( )
   {
      return m_Renderer;
   }

   vtkRenderWindow* SimpleVTKAppFramework::getRenderWindow( )
   {
      return m_RenderWindow;
   }

   vtkRenderWindowInteractor* SimpleVTKAppFramework::getRenderWindowInteractor( )
   {
      return m_RenderWindowInteractor;
   }

   void SimpleVTKAppFramework::start( )
   {
      m_RenderWindowInteractor->Initialize( );

      m_RenderWindowInteractor->Start( );
   }
}