#ifndef __QSPLITTEREX_H__
#define __QSPLITTEREX_H__

#include <QSplitter>

class QSplitterEx : public QSplitter
{
   Q_OBJECT;

public:
   QSplitterEx( QWidget* parent = 0 );

signals:
   void widthsChanged( QSplitterEx* self );

protected slots:
   void handleSplitterMoved( );
};

#endif // __QSPLITTEREX_H__