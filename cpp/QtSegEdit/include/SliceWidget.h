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
#ifndef __SLICEWIDGET_H__
#define __SLICEWIDGET_H__

#include <QLabel>
#include <QMessageBox>
#include <QPainter>

#include "cimg.h"

#include <iostream>

using namespace cimg_library;

typedef CImg<unsigned char> ScanType;

class SliceWidget : public QLabel
{
   Q_OBJECT

public:
   SliceWidget(QWidget* parent = 0, Qt::WindowFlags flags = 0)
      : QLabel(parent, flags)
   {
      scaleFactor = 1;
      
      setBackgroundRole(QPalette::Light);
      setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
      setScaledContents(true);
   }

public slots:
   void open(const QString& filename)
   {
      QImage image(filename);
      if (image.isNull())
      {
         QMessageBox::information(this, tr("Data Loading"), tr("Cannot load %1!").arg(filename));
         return;
      }

      this->setPixmap(QPixmap::fromImage(image));
      
      scaleFactor = 1;

      this->adjustSize();
   }

   void zoomBy(double factor)
   {
      if (!hasData())
         return;

      this->adjustSize();
      scaleFactor *= factor;

      scaleImage();
   }

   void zoom(double factor)
   {
      if (!hasData())
         return;

      this->adjustSize();
      scaleFactor = factor;

      scaleImage();
   }

public:
   double getScaleFactor() { return scaleFactor; }

   bool hasData() const
   {
      return pixmap() != 0;
   }

protected:
   void scaleImage()
   {
      if (!hasData())
         return;

      resize(scaleFactor * pixmap()->size());

   }

   double scaleFactor;
};

#endif // __SLICEWIDGET_H__