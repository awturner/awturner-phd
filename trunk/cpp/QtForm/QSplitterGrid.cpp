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
#include "include/QSplitterGrid.h"

#include "include/QSplitterEx.h"

#include <QList>

#include "Useful/PrintMacros.h"

struct QSplitterGrid::D
{
   bool                m_Busy;
   QList<QSplitterEx*> m_List;
};

QSplitterGrid::QSplitterGrid()
{
   m_D = new D;

   m_D->m_Busy = false;
}

QSplitterGrid::~QSplitterGrid()
{
   delete m_D;
}

void QSplitterGrid::addSplitterEx(QSplitterEx* splitter)
{
   m_D->m_List.append(splitter);

   connect(splitter, SIGNAL(widthsChanged(QSplitterEx*)), this, SLOT(splitterResize(QSplitterEx*)));
}

void QSplitterGrid::removeSplitterEx(QSplitterEx* splitter)
{
   disconnect(splitter, SIGNAL(widthsChanged(QSplitterEx*)), this, SLOT(splitterResize(QSplitterEx*)));
   m_D->m_List.removeAll(splitter);
}

void QSplitterGrid::splitterResize(QSplitterEx* splitter)
{
   if (m_D->m_Busy)
      return;

   m_D->m_Busy = true;

   QList<QSplitterEx*>::iterator it = m_D->m_List.begin();
   QList<QSplitterEx*>::iterator en = m_D->m_List.end();

   QList<int> sizes = splitter->sizes();
   for (int i = 0; i < sizes.size(); ++i)
      PRINTVBL(sizes[i]);

   while (it != en)
   {
      if (*it != splitter)
      {
         (*it)->setSizes(sizes);
      }
      ++it;
   }

   m_D->m_Busy = false;
}