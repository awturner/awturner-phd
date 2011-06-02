#define NOMINMAX

#include "include/DrawPositionWidget.h"

#include "include/PositionHistory.h"

#include <QPaintEvent>
#include <QPainter>
#include <QBrush>

#include <algorithm>
#include <limits>

#include "Useful/PrintMacros.h"

DrawPositionWidget::DrawPositionWidget( QWidget* in_Parent, Qt::WFlags in_Fl )
: QWidget( in_Parent, in_Fl ), m_History( 0 )
{
}

DrawPositionWidget::~DrawPositionWidget( )
{
}

void DrawPositionWidget::setPositionHistory( PositionHistory* ph )
{
   if ( ph != this->m_History )
   {
      DEBUGLINE;
      this->m_History = ph;
      repaint( );
   }
}

PositionHistory* DrawPositionWidget::getPositionHistory( )
{
   return this->m_History;
}

void DrawPositionWidget::sometimesRepaint( )
{
   static int i = 0;

   if ( ++i % 200 == 0 )
   {
      repaint( );
   }
}

void DrawPositionWidget::paintEvent( QPaintEvent* ev )
{
   QPainter painter( this );

   painter.eraseRect( this->rect( ) );

   if ( m_History == 0 || m_History->getPointCount( ) < 2 )
      return;

   double bounds[6];
   m_History->getBounds( bounds );

   // What is the biggest of the spans?
   double span[3];
   double maxSpan = -std::numeric_limits<double>::infinity( );
   for ( int i = 0; i < 3; ++i )
   {
      span[i] = bounds[2*i+1] - bounds[2*i+0];
      maxSpan = std::max( maxSpan, span[i] );
   }

   if ( maxSpan <= 0 )
      return;

   const int minDim = std::min( width()-1, height()-1 );
   
   // Draw the x-y plane
   {
      painter.setPen( QColor( 255, 0, 0 ) );

      const unsigned int starti = ( m_History->getPointCount( ) > 200 ) ? ( m_History->getPointCount( ) - 200 ) : 0;
      const unsigned int endi   = ( m_History->getPointCount( ) > 200 ) ? ( starti + 200 ) : m_History->getPointCount( );

      const double leftX = ( width()  - span[0] / maxSpan * minDim ) / 2;
      const double topY  = ( height() - span[1] / maxSpan * minDim ) / 2;

      double pos[3], joints[6], time;
      m_History->getPoint( starti, time, pos, joints );

      double lastX = pos[0], lastY = pos[1];

      lastX = leftX + ( lastX - bounds[0] ) / maxSpan * minDim;
      lastY = topY  + ( bounds[3] - lastY ) / maxSpan * minDim;

      for ( unsigned int i = starti + 1; i < endi; ++i )
      {
         m_History->getPoint( i, time, pos, joints );

         const double thisX = leftX + ( pos[0]    - bounds[0] ) / maxSpan * minDim;
         const double thisY = topY  + ( bounds[3] - pos[1]    ) / maxSpan * minDim;

         painter.drawLine( lastX, lastY, thisX, thisY );

         lastX = thisX;
         lastY = thisY;
      }
   }

}