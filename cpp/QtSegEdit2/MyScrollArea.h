#ifndef __MYSCROLLAREA_H__
#define __MYSCROLLAREA_H__

#include <QScrollArea>

class QWheelEvent;
class QKeyEvent;

class MyScrollArea : public QScrollArea
{
   Q_OBJECT;
public:
   MyScrollArea( QWidget* parent = 0 );

signals:
   void requestSliceChange( int dir );

protected:
   virtual void wheelEvent( QWheelEvent* );
   virtual void keyPressEvent( QKeyEvent* );
};

#endif // __MYSCROLLAREA_H__