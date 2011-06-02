#ifndef __POINTLISTMODEL_H__
#define __POINTLISTMODEL_H__

#include <QAbstractItemModel>
#include "include/PickedPoint.h"

class PointListModel : public QAbstractItemModel
{
public:
   PointListModel( PickedPoints& points );

   QVariant data(const QModelIndex &index, int role) const;
   Qt::ItemFlags flags(const QModelIndex &index) const;
   QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
   QModelIndex parent(const QModelIndex &index) const;
   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   int columnCount(const QModelIndex &parent = QModelIndex()) const;

   void notifyOfChange( );

protected:
   PickedPoints& m_Points;
};

#endif // __POINTLISTMODEL_H__