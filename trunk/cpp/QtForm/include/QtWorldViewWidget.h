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
#ifndef __QTWORLDVIEWWIDGET_H__
#define __QTWORLDVIEWWIDGET_H__

#include <QSplitter>

#include "DrawableFramework/DrawableWorld.h"
#include "WorldViewer.h"
#include "WorldTreeModel.h"

class QMutex;

class QtWorldViewWidget : public QSplitter
{
   Q_OBJECT

public:
   QtWorldViewWidget(QWidget* parent = 0);
   virtual ~QtWorldViewWidget();

   void initialize(const unsigned int r = 1, const unsigned int c = 1, QMutex* mutex = 0);

   AWT::DrawableAssembly::P getAllWorlds();

   unsigned int getNumberOfViewers() const;

   AWT::WorldViewer::P getViewer(const unsigned int i);

   

   AWT::WorldTreeModel* getWorldTreeModel();

public slots:
   void repaintViews();
   void repaintView(const unsigned int i);

   void centreViews();
   void centreView(const unsigned int i);

protected:
   struct D;
   D* m_D;
};

#endif // __QTWORLDVIEWWIDGET_H__