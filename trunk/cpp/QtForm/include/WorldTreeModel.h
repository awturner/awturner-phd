#ifndef __WorldTreeModel_H__
#define __WorldTreeModel_H__

#include "DrawableFramework/DrawableAssembly.h"

#include <QAbstractItemModel>

namespace AWT
{
   class WorldTreeModel : public QAbstractItemModel
   {
   public:
      WorldTreeModel( AWT::DrawableAssembly* assm, QObject *parent = 0);
      ~WorldTreeModel( );

      QVariant data(const QModelIndex &index, int role) const;
      Qt::ItemFlags flags(const QModelIndex &index) const;
      QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
      QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
      QModelIndex parent(const QModelIndex &index) const;
      int rowCount(const QModelIndex &parent = QModelIndex()) const;
      int columnCount(const QModelIndex &parent = QModelIndex()) const;

      bool setData( const QModelIndex& index, const QVariant& value, int role );

   private:
      struct D;
      D* m_D;
   };
}

#endif // __WorldTreeModel_H__