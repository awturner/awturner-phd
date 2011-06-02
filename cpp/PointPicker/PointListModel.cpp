#include "include/PointListModel.h"

#include "Useful/PrintMacros.h"

#include <limits>

PointListModel::PointListModel( PickedPoints& points )
: m_Points( points )
{
}

QVariant PointListModel::data(const QModelIndex &index, int role) const
{
   if (!index.isValid())
      return QVariant();

   if ( role == Qt::DisplayRole )
   {
      PickedPoint pp = m_Points[index.row()];

      double pnt[3];
      pp.getPoint( pnt );

      if ( index.column() == 0 )
      {
         return pp.getName( );
      }
      else if ( index.column( ) < 4 )
      {
         double v = pnt[ index.column() - 1 ];

         if ( v == std::numeric_limits<double>::infinity( ) )
            return tr( "-" );
         else
            return v;
      }
   }

   return QVariant( );
}

Qt::ItemFlags PointListModel::flags(const QModelIndex &index) const
{
   if (!index.isValid())
      return 0;

   if ( index.column( ) == 0 )
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

   return Qt::ItemIsEnabled;
}

QVariant PointListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
   {
      switch ( section )
      {
      case 0:
         return tr( "Name" );
      case 1:
         return tr( "X" );
      case 2:
         return tr( "Y" );
      case 3:
         return tr( "Z" );
      }
   }

   return QVariant();
}

QModelIndex PointListModel::index(int row, int column, const QModelIndex &parent) const
{
   if (!hasIndex(row, column, parent))
   {
      return QModelIndex();
   }

   if ( !parent.isValid() )
   {
      return createIndex( row, column, &m_Points[ row ] );
   }
   else
   {
      return QModelIndex( );
   }

   return createIndex( row, column, &m_Points[row] );
}

QModelIndex PointListModel::parent(const QModelIndex &index) const
{
   if (!index.isValid())
      return QModelIndex();

   return createIndex( index.row( ), 0, &m_Points );
}

int PointListModel::rowCount(const QModelIndex &parent) const
{
   return m_Points.size( );

   if ( parent.internalPointer( ) == 0 )
      return m_Points.size( );
   else
      return 0;
}

int PointListModel::columnCount(const QModelIndex &parent) const
{
   return 4;

   if ( parent.internalPointer( ) == &m_Points )
      return 4;
   else
      return 0;
}

void PointListModel::notifyOfChange( )
{
   emit dataChanged( createIndex( 0, 0, &m_Points[0] ), createIndex( m_Points.size()-1, 4, &m_Points[m_Points.size()-1] ) );
}