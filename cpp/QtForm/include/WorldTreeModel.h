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
#ifndef __WorldTreeModel_H__
#define __WorldTreeModel_H__

#include "DrawableFramework/DrawableAssembly.h"

#include <QAbstractItemModel>

namespace AWT
{
   class WorldTreeModel : public QAbstractItemModel
   {
   public:
      WorldTreeModel(AWT::DrawableAssembly* assm, QObject *parent = 0);
      ~WorldTreeModel();

      QVariant data(const QModelIndex &index, int role) const;
      Qt::ItemFlags flags(const QModelIndex &index) const;
      QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
      QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
      QModelIndex parent(const QModelIndex &index) const;
      int rowCount(const QModelIndex &parent = QModelIndex()) const;
      int columnCount(const QModelIndex &parent = QModelIndex()) const;

      bool setData(const QModelIndex& index, const QVariant& value, int role);

   private:
      struct D;
      D* m_D;
   };
}

#endif // __WorldTreeModel_H__