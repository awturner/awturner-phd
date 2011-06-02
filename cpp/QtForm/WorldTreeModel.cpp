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
#include "include/WorldTreeModel.h"

#include <QStringList>

using namespace AWT;

struct AWT::WorldTreeModel::D
{
   unsigned int getChildRow(Drawable* d)
   {
      DrawableAssembly* parent = d->getParent();

      unsigned int row = 0;
      for (row = 0; row < parent->getNumberOfChildren(); ++row)
         if (*parent->getChild(row) == d)
            break;

      return row;
   }
   DrawableAssembly* m_RootItem;
};

AWT::WorldTreeModel::WorldTreeModel(AWT::DrawableAssembly* assm, QObject *parent) : QAbstractItemModel(parent)
{
   m_D = new D;
   m_D->m_RootItem = assm;
}

AWT::WorldTreeModel::~WorldTreeModel()
{
   delete m_D;
}

QModelIndex AWT::WorldTreeModel::index(int row, int column, const QModelIndex &parent) const
{
   if (!hasIndex(row, column, parent))
   {
      DEBUGLINE;
      return QModelIndex();
   }

   Drawable* parentItem;

   if (!parent.isValid())
      parentItem = m_D->m_RootItem;
   else
      parentItem = static_cast<Drawable*>(parent.internalPointer());

   DrawableAssembly* assm = dynamic_cast<DrawableAssembly*>(parentItem);

   // If we don't have an assembly, or the row requested is too large,
   // return the dummy model index
   if (assm == 0 || static_cast<unsigned int>(row) >= assm->getNumberOfChildren())
      return QModelIndex();

   return createIndex(row, column, *assm->getChild(row));
}


QModelIndex AWT::WorldTreeModel::parent(const QModelIndex &index) const
{
   if (!index.isValid())
      return QModelIndex();

   Drawable* childItem  = static_cast<Drawable*>(index.internalPointer());
   Drawable* parentItem = childItem->getParent();

   // There is no grandparent in this case
   if (parentItem == m_D->m_RootItem)
      return QModelIndex();

   unsigned int row = m_D->getChildRow(parentItem);
   if (row < parentItem->getParent()->getNumberOfChildren())
         return createIndex(row, 0, parentItem);

   return QModelIndex();
}

int AWT::WorldTreeModel::rowCount(const QModelIndex &parent) const
{
   if (parent.column() > 0)
      return 0;

   Drawable* parentItem;

   if (!parent.isValid())
      parentItem = m_D->m_RootItem;
   else
      parentItem = static_cast<Drawable*>(parent.internalPointer());

   DrawableAssembly* assm = dynamic_cast<DrawableAssembly*>(parentItem);

   if (assm != 0)
      return assm->getNumberOfChildren();

   return 0;
}

int AWT::WorldTreeModel::columnCount(const QModelIndex &/*parent*/) const
{
   return 2;
}

QVariant AWT::WorldTreeModel::data(const QModelIndex &index, int role) const
{
   if (!index.isValid())
      return QVariant();

   // Work out the index of the drawable
   Drawable*         item      = static_cast<Drawable*>(index.internalPointer());
   DrawableAssembly* parentItem = item->getParent();
   unsigned int row = m_D->getChildRow(item);

   if (row < parentItem->getNumberOfChildren())
   {
      if (role == Qt::CheckStateRole && index.column() == 1)
      {
         return item->isVisible() ? Qt::Checked : Qt::Unchecked;
      }
      else if (role == Qt::DisplayRole && index.column() == 0)
      {
         return tr(parentItem->getChildName(row).c_str());
      }
   }

   return QVariant();
}

QVariant AWT::WorldTreeModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
   {
      switch (section)
      {
      case 0:
         return tr("Name");
      case 1:
         return tr("Visible");
      }
   }

   return QVariant();
}

Qt::ItemFlags AWT::WorldTreeModel::flags(const QModelIndex &index) const
{
   if (!index.isValid())
      return 0;

   Qt::ItemFlags ret = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

   if (index.column() == 1)
      ret |= Qt::ItemIsUserCheckable;

   return ret;
}

bool AWT::WorldTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
   if (index.isValid())
   {
      if (index.column() == 1 && role == Qt::CheckStateRole)
      {
         Drawable* d = static_cast<Drawable*>(index.internalPointer());
         d->setVisible(value.toBool());

         emit dataChanged(index, index);
         return true;
      }
   }
   return false;
}

