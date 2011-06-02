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
#include "include/PointPickMouseBehaviour.h"

#include "include/OpenGLWidget.h"

#include "MeshSearching/FacesRayIntersectionSearch.h"

#include <cmath>

using namespace AWT;

struct AWT::PointPickMouseBehaviour::D
{
   FacesRayIntersectionSearch<T>::P m_Searcher;
   Mesh<T>::P m_Target;
   Notifiable* m_Not;
   double m_LastPicked[3];
};

AWT::PointPickMouseBehaviour::PointPickMouseBehaviour()
{
   m_D = new D();

   m_D->m_Not = 0;

   m_D->m_Searcher = FacesRayIntersectionSearch<T>::getInstance( );
}

AWT::PointPickMouseBehaviour::~PointPickMouseBehaviour()
{
   delete m_D;
}

AWT::PointPickMouseBehaviour::P AWT::PointPickMouseBehaviour::getInstance( )
{
   AUTOGETINSTANCE( AWT::PointPickMouseBehaviour, ( ) );
}

GETNAMEMACRO( AWT::PointPickMouseBehaviour );

bool AWT::PointPickMouseBehaviour::mousePressed( OpenGLWidget* widget, int x, int y, int button, int keys )
{
   OpenGLWidgetMouseBehaviour::mousePressed( widget, x, y, button, keys );

   if ( *m_D->m_Target == 0 )
      return false;

   double eye[3];
   double dir[3];

   widget->getEyePosition( eye );
   widget->getMouseDirection( dir );

   m_D->m_Searcher->reset( );
   m_D->m_Searcher->setTestPoint( eye );
   m_D->m_Searcher->setTestNormal( dir );

   m_D->m_Target->search( m_D->m_Searcher );
   
   if ( m_D->m_Searcher->getNearestPoint( dir ) != -1 )
   {
      for ( unsigned int i = 0; i < 3; ++i )
         m_D->m_LastPicked[i] = dir[i];

      if ( m_D->m_Not != 0 )
         m_D->m_Not->notify( );
   }

   return false;
}

bool AWT::PointPickMouseBehaviour::mouseMoved( OpenGLWidget* /*widget*/, int /*x*/, int /*y*/, int /*button*/, int /*keys*/ )
{
   return false;
}

void AWT::PointPickMouseBehaviour::setTarget( Mesh<double>::P mesh )
{
   m_D->m_Target = mesh;
}

Mesh<double>::P AWT::PointPickMouseBehaviour::getTarget( )
{
   return m_D->m_Target;
}

void AWT::PointPickMouseBehaviour::getLastPicked( double pnt[3] )
{
   for ( unsigned int i = 0; i < 3; ++i )
      pnt[i] = m_D->m_LastPicked[i];
}

void AWT::PointPickMouseBehaviour::setNotifiable(Notifiable *not)
{
   m_D->m_Not = not;
}