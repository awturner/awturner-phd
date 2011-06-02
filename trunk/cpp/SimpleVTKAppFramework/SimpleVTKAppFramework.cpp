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