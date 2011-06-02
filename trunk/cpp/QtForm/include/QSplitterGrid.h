#ifndef __QSPLITTERGRID_H__
#define __QSPLITTERGRID_H__

#include <QObject>

class QSplitterEx;

class QSplitterGrid : public QObject
{
   Q_OBJECT;

public:
   QSplitterGrid( );
   virtual ~QSplitterGrid( );

   void addSplitterEx( QSplitterEx* splitter );
   void removeSplitterEx( QSplitterEx* splitter );

public slots:
   void splitterResize( QSplitterEx* splitter );

protected:
   struct D;
   D* m_D;
};

#endif // __QSPLITTERGRID_H__