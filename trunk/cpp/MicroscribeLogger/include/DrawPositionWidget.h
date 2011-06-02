#ifndef __DRAWPOSITIONWIDGET_H__
#define __DRAWPOSITIONWIDGET_H__

#include <QWidget>
#include <QVector>

class QPaintEvent;
class PositionHistory;

class DrawPositionWidget : public QWidget
{
   Q_OBJECT;

public:
   DrawPositionWidget( QWidget* in_Parent = 0, Qt::WFlags in_Fl = 0 );
   virtual ~DrawPositionWidget( );

   void setPositionHistory( PositionHistory* ph );
   PositionHistory* getPositionHistory( );

public slots:
   virtual void sometimesRepaint( );

protected:
   virtual void paintEvent( QPaintEvent* ev );

   PositionHistory* m_History;
};


#endif // __DRAWPOSITIONWIDGET_H__