#ifndef __SLICEWIDGET_H__
#define __SLICEWIDGET_H__

#include <QLabel>
#include <QMessageBox>
#include <QPainter>

#include "cimg.h"

#include <iostream>

using namespace cimg_library;

typedef CImg<unsigned char> ScanType;

class SliceWidget : public QLabel
{
   Q_OBJECT

public:
   SliceWidget( QWidget* parent = 0, Qt::WindowFlags flags = 0 )
      : QLabel( parent, flags )
   {
      scaleFactor = 1;
      
      setBackgroundRole( QPalette::Light );
      setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
      setScaledContents( true );
   }

public slots:
   void open( const QString& filename )
   {
      QImage image( filename );
      if ( image.isNull() )
      {
         QMessageBox::information( this, tr( "Data Loading" ), tr( "Cannot load %1!" ).arg( filename ) );
         return;
      }

      this->setPixmap( QPixmap::fromImage( image ) );
      
      scaleFactor = 1;

      this->adjustSize( );
   }

   void zoomBy( double factor )
   {
      if ( !hasData( ) )
         return;

      this->adjustSize( );
      scaleFactor *= factor;

      scaleImage( );
   }

   void zoom( double factor )
   {
      if ( !hasData( ) )
         return;

      this->adjustSize( );
      scaleFactor = factor;

      scaleImage( );
   }

public:
   double getScaleFactor( ) { return scaleFactor; }

   bool hasData( ) const
   {
      return pixmap( ) != 0;
   }

protected:
   void scaleImage( )
   {
      if ( !hasData( ) )
         return;

      resize( scaleFactor * pixmap( )->size( ) );

   }

   double scaleFactor;
};

#endif // __SLICEWIDGET_H__