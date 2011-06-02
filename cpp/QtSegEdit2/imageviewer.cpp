/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "SliceDisplayWidget.h"

#include "imageviewer.h"

#include "Useful/PrintMacros.h"

#include "EditWidget.h"

#include "SegmentationModel.h"

ImageViewer::ImageViewer()
{
   setCentralWidget( editWidget = new EditWidget( this ) );

   this->scaleFactor = editWidget->getScale( );

   createActions();
   createMenus();

   setWindowTitle(tr("Segmentation Editor"));
}

void ImageViewer::open()
{
   QString fileName = QFileDialog::getOpenFileName(this,
      tr("Open File"), QDir::currentPath());
   if (!fileName.isEmpty()) 
   {
      this->load( fileName );
   }
}

void ImageViewer::load( const QString& filename )
{
   if ( editWidget == 0 )
      return;

   if ( editWidget->load( filename ) )
   {
      QFileInfo fi( filename );

      this->setWindowTitle( tr("Segmentation Editor - %1").arg( fi.canonicalFilePath() ) );
   }

   updateActions( );
}

void ImageViewer::zoomIn()
{
   if ( this->scaleFactor < 1 )
      scaleImage( this->scaleFactor + 0.25 );
   else
      scaleImage( this->scaleFactor + 1 );
}

void ImageViewer::zoomOut()
{
   if ( this->scaleFactor <= 1 )
      scaleImage( this->scaleFactor - 0.25 );
   else
      scaleImage( this->scaleFactor - 1 );
}

void ImageViewer::normalSize()
{
   scaleImage( 1.0 );
}

void ImageViewer::scaleImage(double factor)
{
   this->editWidget->setScale( factor );

   scaleFactor = this->editWidget->getScale( );

   updateActions( );
}

void ImageViewer::fitToWindow()
{
   this->editWidget->setFitToContainer( fitToWindowAct->isChecked( ) );
   updateActions();
}

void ImageViewer::about()
{
   QMessageBox::about(this, tr("About Image Viewer"),
      tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
      "and QScrollArea to display an image. QLabel is typically used "
      "for displaying a text, but it can also display an image. "
      "QScrollArea provides a scrolling view around another widget. "
      "If the child widget exceeds the size of the frame, QScrollArea "
      "automatically provides scroll bars. </p><p>The example "
      "demonstrates how QLabel's ability to scale its contents "
      "(QLabel::scaledContents), and QScrollArea's ability to "
      "automatically resize its contents "
      "(QScrollArea::widgetResizable), can be used to implement "
      "zooming and scaling features. </p><p>In addition the example "
      "shows how to use QPainter to print an image.</p>"));
}

void ImageViewer::createActions()
{
   openAct = new QAction(tr("&Open..."), this);
   openAct->setShortcut(tr("Ctrl+O"));
   connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

   exitAct = new QAction(tr("E&xit"), this);
   exitAct->setShortcut(tr("Ctrl+Q"));
   connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

   zoomInAct = new QAction(tr("Zoom &In"), this);
   zoomInAct->setShortcut(tr("Ctrl++"));
   zoomInAct->setEnabled(false);
   connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

   zoomOutAct = new QAction(tr("Zoom &Out"), this);
   zoomOutAct->setShortcut(tr("Ctrl+-"));
   zoomOutAct->setEnabled(false);
   connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

   normalSizeAct = new QAction(tr("&Normal Size"), this);
   normalSizeAct->setShortcut(tr("Ctrl+0"));
   normalSizeAct->setEnabled(false);
   connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

   fitToWindowAct = new QAction(tr("&Fit to Window"), this);
   fitToWindowAct->setEnabled(false);
   fitToWindowAct->setCheckable(true);
   fitToWindowAct->setShortcut(tr("Ctrl+F"));
   connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

   showSegAct = new QAction( tr( "Show segmentation" ), this );
   showSegAct->setEnabled( true );
   showSegAct->setCheckable( true );
   showSegAct->setShortcut( tr( "Space" ) );
   connect( showSegAct, SIGNAL( toggled( bool ) ), editWidget, SLOT( showSegmentation( bool ) ) );

   aboutAct = new QAction(tr("&About"), this);
   connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

   aboutQtAct = new QAction(tr("About &Qt"), this);
   connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void ImageViewer::createMenus()
{
   fileMenu = new QMenu(tr("&File"), this);
   fileMenu->addAction(openAct);
   fileMenu->addSeparator();
   fileMenu->addAction(exitAct);

   viewMenu = new QMenu(tr("&View"), this);
   viewMenu->addAction(zoomInAct);
   viewMenu->addAction(zoomOutAct);
   viewMenu->addAction(normalSizeAct);
   viewMenu->addSeparator();
   viewMenu->addAction(fitToWindowAct);
   viewMenu->addSeparator();
   viewMenu->addAction(showSegAct);

   helpMenu = new QMenu(tr("&Help"), this);
   helpMenu->addAction(aboutAct);
   helpMenu->addAction(aboutQtAct);

   menuBar()->addMenu(fileMenu);
   menuBar()->addMenu(viewMenu);
   menuBar()->addMenu(helpMenu);
}

void ImageViewer::updateActions()
{
   const bool hasImage = this->editWidget->isLoaded( );

   zoomInAct->setEnabled(   hasImage && !fitToWindowAct->isChecked() && scaleFactor < 4.0  );
   zoomOutAct->setEnabled(  hasImage && !fitToWindowAct->isChecked() && scaleFactor > 0.5 );
   normalSizeAct->setEnabled( hasImage && !fitToWindowAct->isChecked() && scaleFactor != 1 );

   fitToWindowAct->setEnabled( hasImage );

   showSegAct->setEnabled( hasImage );
   showSegAct->setChecked( editWidget->isShowSegmentation( ) );
}

