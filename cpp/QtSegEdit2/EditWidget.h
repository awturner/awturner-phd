#ifndef __EDITWIDGET_H__
#define __EDITWIDGET_H__

#include <QWidget>

class SliceDisplayWidget;
class MyScrollArea;

class SegmentationModel;

class QScrollBar;
class QSlider;
class QMouseEvent;

class EditWidget : public QWidget
{
   Q_OBJECT;

public:
   EditWidget( QWidget* parent = 0, Qt::WindowFlags f = 0 );
   virtual ~EditWidget( );

signals:
   void scaleChanged( const double newscale );

public slots:
   void setScale( const double newscale );
   void setFitToContainer( const bool state );
   void showSegmentation( const bool state );

public:
   const double getScale( ) const;
   const bool   isFitToContainer( ) const;
   const bool   isLoaded( ) const;

   const bool   isShowSegmentation( ) const;
   const bool   isShowScan( ) const;

   bool load( const QString& filename );

protected slots:
   void attemptSliceSet( int value );
   void attemptSliceChange( int delta );

   void mouseMoved( QMouseEvent* );

protected:
   void adjustScrollBar(QScrollBar *scrollBar, const double factor);

   bool   m_FitToContainer;
   double m_Scale;

   SliceDisplayWidget*      m_ImageLabel;
   MyScrollArea* m_ScrollArea;

   QSlider*     m_Slider;
   QLayout*     m_Layout;

   SegmentationModel* m_Model;
};

#endif // __EDITWIDGET_H__