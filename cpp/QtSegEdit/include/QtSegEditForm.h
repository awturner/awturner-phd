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
#ifndef __QTSEGEDITFORM_H__
#define __QTSEGEDITFORM_H__

#include <QMainWindow>
#include <QFileDialog>
#include <QScrollBar>

#include "ui_QtSegEditForm.h"

class QtSegEditForm : public QMainWindow, public Ui::QtSegEditForm
{
   Q_OBJECT

public:
   QtSegEditForm(QWidget* in_Parent = 0, Qt::WFlags in_Fl = 0)
   : QMainWindow(in_Parent, in_Fl)
   {   
      setupUi(this);

      scrollArea->setBackgroundRole(QPalette::Dark);
      scrollArea->setWidgetResizable(true);

      connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));
      actionOpen->setShortcut(tr("Ctrl+O"));

      connect(actionZoomIn,  SIGNAL(triggered()), this, SLOT(zoomIn() ));
      actionZoomIn->setShortcut(QKeySequence::ZoomIn);
      connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
      actionZoomOut->setShortcut(QKeySequence::ZoomOut );

      connect(actionNormalSize, SIGNAL(triggered()), this, SLOT(zoomNormal()));
      actionNormalSize->setShortcut(tr("Ctrl+0"));

      setZoomActionState();
   }

private slots:
   void open()
   {
      QString filename = QFileDialog::getOpenFileName(this,
         tr("Open File"), QDir::homePath());

      if (!filename.isEmpty())
      {
         image->open(filename);

         setZoomActionState();
      }
   }

   void setZoomActionState()
   {
      if (image->hasData())
      {
         actionZoomIn->setEnabled(image->getScaleFactor() < 3.0);
         actionZoomOut->setEnabled(image->getScaleFactor() > 1.0/3.0);
         actionNormalSize->setEnabled(true);
      }
      else
      {
         actionZoomIn->setEnabled(false);
         actionZoomOut->setEnabled(false);
         actionNormalSize->setEnabled(false);
      }
   }

   void zoomIn()
   {
      image->zoomBy (1.25);
      setZoomActionState();      
   }

   void zoomOut()
   {
      image->zoomBy (0.8);
      setZoomActionState();
   }

   void zoomNormal()
   {
      image->zoom(1.0);
      setZoomActionState();
   }

   void adjustScrollBar(QScrollBar* scrollBar, double factor)
   {
      scrollBar->setValue(int(factor * scrollBar->value()
                             + ((factor - 1) * scrollBar->pageStep()/2)));

   }
private:
};

#endif // __QTSEGEDITFORM_H__