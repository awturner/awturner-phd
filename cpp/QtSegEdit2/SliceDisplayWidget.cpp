#include "SliceDisplayWidget.h"

#include <QMouseEvent>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QBrush>
#include <QPaintEvent>

#include "SegmentationSlice.h"

#include "Useful/PrintMacros.h"


SliceDisplayWidget::SliceDisplayWidget( QWidget* parent, Qt::WindowFlags f )
   : QLabel( parent, f ), m_Scale( 1 ), m_Slice( 0 ), 
   m_ScanImage( 0 ), m_Segmentation( 0 ), m_SliceChanged( false ),
   m_ShowSegmentation( true )
{
   // Enable mouse tracking so that mouse move events are received
   // even when mouse isn't pressed
   this->setMouseTracking( true );
}

SliceDisplayWidget::~SliceDisplayWidget( )
{
   if ( m_Slice != 0 )
      delete m_Slice;
}

QMouseEvent SliceDisplayWidget::transformedMouseEvent( const QMouseEvent& ev )
{
   qreal scale;

   if ( width( ) == m_Scale * m_ScanImage->width( ) )
   {
      scale = m_Scale;
   }
   else
   {
      // This is the scale we're going to apply...
      scale = static_cast<qreal>( width( ) ) / m_ScanImage->width( );

      // ...but not if this puts the bottom of the image out of sight
      if ( m_ScanImage->height( ) * scale > height( ) )
         scale = static_cast<qreal>( height( ) ) / m_ScanImage->height( );
   }

   return QMouseEvent( ev.type( ), QPoint( ev.pos( ) / scale ), ev.button( ), ev.buttons( ), ev.modifiers( ) );
}

void SliceDisplayWidget::mouseMoveEvent( QMouseEvent* ev )
{
   emit mouseMoved( &transformedMouseEvent( *ev ) );
}

void SliceDisplayWidget::mousePressEvent( QMouseEvent* ev )
{
   emit mousePressed( &transformedMouseEvent( *ev ) );
}

void SliceDisplayWidget::mouseReleaseEvent( QMouseEvent* ev )
{
   emit mouseReleased( &transformedMouseEvent( *ev ) );
}

void SliceDisplayWidget::refreshScanImage( )
{
   const ImageType minn = m_Slice->getMin( );
   const ImageType maxx = m_Slice->getMax( );

   Coord x, y;

   ImageType* buffer = new ImageType[ m_Slice->getHeight( ) * m_Slice->getWidth( ) ];
   uchar* mapped     = m_ScanImage->bits( );

   m_Slice->getScan( buffer );

   const Coord ymax = m_Slice->getHeight( );
   const Coord xmax = m_Slice->getWidth( );

   const float mul = 255.f / ( maxx - minn );

   unsigned int i = 0;
   for ( y = 0; y < ymax; ++y )
   {
      for ( x = 0; x < xmax; ++x, ++i )
      {
         const unsigned int vv = static_cast<unsigned int>( mul * ( buffer[i] - minn ) );

         mapped[ 4*i + 3 ] = 255;
         mapped[ 4*i + 2 ] =  vv;
         mapped[ 4*i + 1 ] =  vv;
         mapped[ 4*i + 0 ] =  vv;
      }
   }

   delete [] buffer;
}

void SliceDisplayWidget::refreshSegmentation( )
{
   Coord x, y;

   LabelType* buffer = new LabelType[ m_Slice->getHeight( ) * m_Slice->getWidth( ) ];
   uchar* mapped     = m_Segmentation->bits( );

   m_Slice->getSegmentation( buffer );

   const Coord ymax = m_Slice->getHeight( );
   const Coord xmax = m_Slice->getWidth( );

   unsigned int i = 0;
   for ( y = 0; y < ymax; ++y )
   {
      for ( x = 0; x < xmax; ++x, ++i )
      {

         switch ( buffer[i] )
         {
         case 0:
            mapped[ 4*i + 3 ] = 0;
            mapped[ 4*i + 2 ] = 0;
            mapped[ 4*i + 1 ] = 0;
            mapped[ 4*i + 0 ] = 0;
            break;
         case 1:
            mapped[ 4*i + 3 ] = 127;
            mapped[ 4*i + 2 ] = 127;
            mapped[ 4*i + 1 ] =  0;
            mapped[ 4*i + 0 ] =  0;
            break;
         }
      }
   }

   delete [] buffer;
}

void SliceDisplayWidget::paintEvent( QPaintEvent* /*ev*/ )
{
   if ( m_ScanImage == 0 || m_ScanImage->width( ) != m_Slice->getWidth( ) || m_ScanImage->height( ) != m_Slice->getHeight( ) )
   {
      if ( m_ScanImage != 0 )
         delete m_ScanImage;

      m_ScanImage = new QImage( m_Slice->getWidth( ), m_Slice->getHeight( ), QImage::Format_ARGB32_Premultiplied );

      if ( m_Segmentation != 0 )
         delete m_Segmentation;

      m_Segmentation = new QImage( m_Slice->getWidth( ), m_Slice->getHeight( ), QImage::Format_ARGB32_Premultiplied );
   }
   
   if ( m_SliceChanged )
   {
      m_SliceChanged = false;
      refreshScanImage( );
   }

   refreshSegmentation( );

   QPainter painter( this );

   // This is the scale we're going to apply...
   qreal scale = static_cast<qreal>( width( ) ) / m_ScanImage->width( );

   // ...but not if this puts the bottom of the image out of sight
   if ( m_ScanImage->height( ) * scale > height( ) )
      scale = static_cast<qreal>( height( ) ) / m_ScanImage->height( );

   painter.scale( scale, scale );

   painter.fillRect( rect( ), painter.background( ) );

   painter.drawImage( 0, 0, *this->m_ScanImage );

   if ( m_ShowSegmentation )
      painter.drawImage( 0, 0, *this->m_Segmentation );
}

double SliceDisplayWidget::getScale( ) const
{
   return this->m_Scale;
}

void SliceDisplayWidget::setScale(double scale)
{
   if ( m_ScanImage == 0 )
      return;

   if ( scale == 1 )
   {
      this->adjustSize( );
   }

   this->resize(scale * m_ScanImage->size());
   this->m_Scale = scale;
}

SegmentationSlice* SliceDisplayWidget::getSlice( )
{
   return this->m_Slice;
}

void SliceDisplayWidget::setSlice( SegmentationSlice* slice )
{
   if ( m_Slice != 0 )
      delete m_Slice;

   m_Slice = slice;

   m_SliceChanged = true;

   QSize newSize = QSize( slice->getWidth( ), slice->getHeight( ) ) * m_Scale;
   if ( newSize != size( ) )
      resize( newSize );

   repaint( );
}

bool SliceDisplayWidget::isShowSegmentation( ) const
{
   return m_ShowSegmentation;
}

void SliceDisplayWidget::setShowSegmentation( const bool state )
{
   if ( m_ShowSegmentation ^ state )
   {
      m_ShowSegmentation = state;
      repaint( );
   }
}
