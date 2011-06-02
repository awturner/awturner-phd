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
#include "VTKAppFramework.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleSwitch.h"

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"

AWT::VTKAppFramework::VTKAppFramework( )
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

AWT::VTKAppFramework::~VTKAppFramework( )
{
   m_RenderWindowInteractor->Delete( );
   m_RenderWindow->Delete( );
   m_Renderer->Delete( );
}

void AWT::VTKAppFramework::setWindowSize( int w, int h )
{
   m_RenderWindow->SetSize( w, h );
}

vtkRenderer* AWT::VTKAppFramework::getRenderer( )
{
   return m_Renderer;
}

vtkRenderWindow* AWT::VTKAppFramework::getRenderWindow( )
{
   return m_RenderWindow;
}

vtkRenderWindowInteractor* AWT::VTKAppFramework::getRenderWindowInteractor( )
{
   return m_RenderWindowInteractor;
}

void AWT::VTKAppFramework::start( )
{
   m_RenderWindowInteractor->Initialize( );

   m_RenderWindowInteractor->Start( );
}

vtkActor* AWT::VTKAppFramework::addPolyData( vtkPolyData* poly )
{
   vtkPolyDataMapper* mapper = vtkPolyDataMapper::New( );
   vtkActor* actor = vtkActor::New( );

   mapper->SetInput( poly );
   actor->SetMapper( mapper );

   getRenderer( )->AddActor( actor );

   mapper->Delete( );

   return actor;
}

vtkActor* AWT::VTKAppFramework::addPolyData( vtkPolyDataAlgorithm* poly )
{
   vtkPolyDataMapper* mapper = vtkPolyDataMapper::New( );
   vtkActor* actor = vtkActor::New( );

   mapper->SetInputConnection( poly->GetOutputPort( ) );
   actor->SetMapper( mapper );

   getRenderer( )->AddActor( actor );

   mapper->Delete( );

   return actor;
}

vtkActor* AWT::VTKAppFramework::addPolyData( vtkPolyDataReader* poly )
{
   vtkPolyDataMapper* mapper = vtkPolyDataMapper::New( );
   vtkActor* actor = vtkActor::New( );

   mapper->SetInputConnection( poly->GetOutputPort( ) );
   actor->SetMapper( mapper );

   getRenderer( )->AddActor( actor );

   mapper->Delete( );

   return actor;
}