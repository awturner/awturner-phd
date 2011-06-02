#ifndef __PICKEDPOINT_H__
#define __PICKEDPOINT_H__

#include <QObject>
#include <QString>

class PickedPoint : public QObject
{
   Q_OBJECT

public:
   PickedPoint( );
   PickedPoint( const PickedPoint& other );
   PickedPoint( const QString& name );
   PickedPoint( const QString& name, const double pnt[3] );

   PickedPoint& operator=( const PickedPoint& other );

   virtual ~PickedPoint( );

   void getPoint( double pnt[3] ) const;
   void setPoint( const double pnt[3] );

   QString getName( ) const;
   void setName( const QString& name );

protected:
   QString m_Name;
   double m_Pnt[3];
};

#include <QVector>

typedef QVector<PickedPoint> PickedPoints;

#endif // __PICKEDPOINT_H__