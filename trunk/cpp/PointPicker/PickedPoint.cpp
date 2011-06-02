#include "include/PickedPoint.h"

#include <limits>

PickedPoint::PickedPoint( )
{
   setName( QObject::tr( "Unnamed Point" ) );
   const double pnt[] = { std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity() };
   setPoint( pnt );
}

PickedPoint::PickedPoint( const PickedPoint& other )
{
   setName( other.getName() );

   double pnt[3];
   other.getPoint( pnt );
   setPoint( pnt );
}

PickedPoint::PickedPoint( const QString& name )
{
   setName( name );

   const double pnt[] = { std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity() };
   setPoint( pnt );
}

PickedPoint::PickedPoint( const QString& name, const double pnt[3] )
{
   setName( name );
   setPoint( pnt );
}

PickedPoint::~PickedPoint( )
{
}

void PickedPoint::getPoint( double pnt[3] ) const
{
   for ( unsigned int i = 0; i < 3; ++i )
      pnt[i] = m_Pnt[i];
}

void PickedPoint::setPoint( const double pnt[3] )
{
   for ( unsigned int i = 0; i < 3; ++i )
      m_Pnt[i] = pnt[i];
}

QString PickedPoint::getName( ) const
{
   return m_Name;
}

void PickedPoint::setName( const QString& name )
{
   m_Name = name;
}