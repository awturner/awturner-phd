#ifndef __POSITIONHISTORY_H__
#define __POSITIONHISTORY_H__

#include <QVector>
#include <QObject>

class PositionHistory : public QObject
{
   Q_OBJECT;

public:
   PositionHistory( );
   virtual ~PositionHistory( );

   unsigned int getPointCount( ) const;

   void getPoint( const unsigned int i, double& time, double pos[3], double joints[6] ) const;
   void addPoint( const double time, const double pos[3], const double joints[6] );

   void getBounds( double bounds[6] ) const;

   void clear( );

signals:
   void pointAdded( const double time, const double x, const double y, const double z );

protected:
   struct timedouble3
   {
      double time;
      double x;
      double y;
      double z;

      double joints[6];
   };
   typedef QVector<timedouble3> PointVector;

   PointVector m_History;

   double m_Bounds[6];
};

#endif // __POSITIONHISTORY_H__