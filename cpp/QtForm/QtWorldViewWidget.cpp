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
#include "include/QtWorldViewWidget.h"

#include "include/OpenGLWidget.h"
#include "include/QSplitterEx.h"
#include "include/QSplitterGrid.h"

#include "include/MoveCameraMouseBehaviour.h"

#include "OpenGLDrawables/OpenGLDrawableWorld.h"

#include <QPointer>
#include <QSplitter>
#include <QMutex>

using namespace AWT;

struct QtWorldViewWidget::D
{
   unsigned int            m_WidgetCount;
   QPointer<OpenGLWidget>* m_Widgets;
   DrawableAssembly::P     m_Galaxy;
   WorldTreeModel*         m_WorldTreeModel;
};

QtWorldViewWidget::QtWorldViewWidget( QWidget* parent )
: QSplitter( Qt::Vertical, parent )
{
   m_D = new D;
}

void QtWorldViewWidget::initialize(const unsigned int rows, const unsigned int cols, QMutex* mutex )
{
   m_D->m_Galaxy = DrawableAssembly::getInstance( );

   m_D->m_WidgetCount = rows*cols;
   m_D->m_Widgets = new QPointer<OpenGLWidget>[rows*cols];

   QSplitter* worldSplitter = new QSplitter( Qt::Vertical, this );
   worldSplitter->setOpaqueResize( false );
   this->addWidget( worldSplitter );

   int widgetsCreated = 0;

   QSplitterEx** splitters = new QSplitterEx*[rows];
   QSplitterGrid* m_SplitGrid = new QSplitterGrid( );
   char buffer[256];

   for ( unsigned int i = 0; i < rows; ++i )
   {
      splitters[i] = new QSplitterEx( worldSplitter );
      worldSplitter->addWidget( splitters[i] );
      splitters[i]->setOpaqueResize( false );

      for ( unsigned int j = 0; j < cols; ++j )
      {
         m_D->m_Widgets[ widgetsCreated ] = new OpenGLWidget( splitters[i], mutex );
         splitters[i]->addWidget( m_D->m_Widgets[ widgetsCreated ] );

         m_D->m_Widgets[ widgetsCreated ]->setMouseBehaviour( MoveCameraMouseBehaviour::getInstance( ) );

         sprintf_s( buffer, "View %d", widgetsCreated+1 );
         m_D->m_Galaxy->add( std::string( buffer ), m_D->m_Widgets[ widgetsCreated ]->getDrawableWorld( ) );

         ++widgetsCreated;
      }
      
      m_SplitGrid->addSplitterEx( splitters[i] );
   }

   m_D->m_WorldTreeModel = new WorldTreeModel( *m_D->m_Galaxy );

   for ( unsigned int i = 0; i < m_D->m_WidgetCount; ++i )
      connect( m_D->m_WorldTreeModel, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), m_D->m_Widgets[i], SLOT( updateGL( ) ) );
}

QtWorldViewWidget::~QtWorldViewWidget()
{
   for ( unsigned int i = 0; i < m_D->m_WidgetCount; ++i )
   {
      /*
      if ( m_D->m_Widgets[i].data( ) != 0 )
         m_D->m_Widgets[i]->release( );
      else
         DEBUGMACRO( "Widget " << i << " has been deleted by Qt" );
      */
   }

   delete [] m_D->m_Widgets;

   delete m_D;
}

AWT::DrawableAssembly::P QtWorldViewWidget::getAllWorlds( )
{
   return m_D->m_Galaxy;
}

unsigned int QtWorldViewWidget::getNumberOfViewers( ) const
{
   return m_D->m_WidgetCount;
}

AWT::WorldViewer::P QtWorldViewWidget::getViewer( const unsigned int i )
{
   return AWT::WorldViewer::P( m_D->m_Widgets[i].data( ) );
}

AWT::WorldTreeModel* QtWorldViewWidget::getWorldTreeModel( )
{
   return m_D->m_WorldTreeModel;
}

void QtWorldViewWidget::centreView(const unsigned int i)
{
   m_D->m_Widgets[i]->resetView( );
}

void QtWorldViewWidget::centreViews( )
{
   for ( unsigned int i = 0; i < getNumberOfViewers( ); ++i )
      centreView( i );
}

void QtWorldViewWidget::repaintView(const unsigned int i)
{
   m_D->m_Widgets[i]->updateGL( );
}

void QtWorldViewWidget::repaintViews( )
{
   for ( unsigned int i = 0; i < getNumberOfViewers( ); ++i )
      repaintView( i );
}