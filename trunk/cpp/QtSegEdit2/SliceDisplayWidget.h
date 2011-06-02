#ifndef __MYLABEL_H__
#define __MYLABEL_H__

#include <QLabel>

class SegmentationSlice;

class SliceDisplayWidget : public QLabel
{
   Q_OBJECT

public:
   SliceDisplayWidget( QWidget* parent = 0, Qt::WindowFlags f = 0 );
   virtual ~SliceDisplayWidget( );

   void setScale( double scale );
   double getScale( ) const;

   void setSlice( SegmentationSlice* model );

   SegmentationSlice* getSlice( );

   bool isShowSegmentation( ) const;
   void setShowSegmentation( const bool state );

signals:
   void mouseMoved( QMouseEvent* ev );
   void mousePressed( QMouseEvent* ev );
   void mouseReleased( QMouseEvent* ev );

protected:
   void mouseMoveEvent( QMouseEvent* ev );
   void mousePressEvent( QMouseEvent* ev );
   void mouseReleaseEvent( QMouseEvent* ev );

   void refreshScanImage( );
   void refreshSegmentation( );

   QMouseEvent transformedMouseEvent( const QMouseEvent& ev );

   void paintEvent( QPaintEvent* ev );

   double m_Scale;

   SegmentationSlice* m_Slice;

   QImage* m_ScanImage;
   QImage* m_Segmentation;

   bool m_SliceChanged;
   bool m_ShowSegmentation;
};

#endif // __MYLABEL_H__