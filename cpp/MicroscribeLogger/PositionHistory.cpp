#include "include/PositionHistory.h"

#include <limits>

PositionHistory::PositionHistory( )
{
   clear( );
   m_History.reserve( 65536 );
}

PositionHistory::~PositionHistory( )
{

}

unsigned int PositionHistory::getPointCount( ) const
{
   return m_History.size( );
}

void PositionHistory::getPoint( const unsigned int i, double& time, double pos[3], double joints[6] ) const
{
   const timedouble3 pt = m_History.at( i );

   time = pt.time;
   pos[0] = pt.x;
   pos[1] = pt.y;
   pos[2] = pt.z;

   for ( int i = 0; i < 6; ++i )
     joints[i] = pt.joints[i];
}

#include "Useful/PrintMacros.h"

void PositionHistory::addPoint( const double time, const double pos[3], const double joints[6] )
{
   timedouble3 pt;
   pt.time = time;
   pt.x = pos[0];
   pt.y = pos[1];
   pt.z = pos[2];

  for ( int i = 0; i < 6; ++i )
    pt.joints[i] = joints[i];

   //PRINTVEC( pos, 3 );

   m_History.push_back( pt );

   for ( unsigned int ax = 0; ax < 3; ++ax )
   {
      m_Bounds[2*ax+0] = std::min( m_Bounds[2*ax+0], pos[ax] );
      m_Bounds[2*ax+1] = std::max( m_Bounds[2*ax+1], pos[ax] );
   }

   emit pointAdded( time, pos[0], pos[1], pos[2] );
}

void PositionHistory::getBounds( double bounds[6] ) const
{
   for ( unsigned int i = 0; i < 6; ++i )
      bounds[i] = m_Bounds[i];
}

void PositionHistory::clear( )
{
   m_History.clear( );

   m_Bounds[0] = m_Bounds[2] = m_Bounds[4] =  std::numeric_limits<double>::infinity( );
   m_Bounds[1] = m_Bounds[3] = m_Bounds[5] = -std::numeric_limits<double>::infinity( );
}