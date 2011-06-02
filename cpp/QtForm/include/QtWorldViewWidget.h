#ifndef __QTWORLDVIEWWIDGET_H__
#define __QTWORLDVIEWWIDGET_H__

#include <QSplitter>

#include "DrawableFramework/DrawableWorld.h"
#include "WorldViewer.h"
#include "WorldTreeModel.h"

class QMutex;

class QtWorldViewWidget : public QSplitter
{
   Q_OBJECT

public:
   QtWorldViewWidget( QWidget* parent = 0 );
   virtual ~QtWorldViewWidget( );

   void initialize( const unsigned int r = 1, const unsigned int c = 1, QMutex* mutex = 0 );

   AWT::DrawableAssembly::P getAllWorlds( );

   unsigned int getNumberOfViewers( ) const;

   AWT::WorldViewer::P getViewer( const unsigned int i );

   

   AWT::WorldTreeModel* getWorldTreeModel( );

public slots:
   void repaintViews( );
   void repaintView( const unsigned int i );

   void centreViews( );
   void centreView( const unsigned int i );

protected:
   struct D;
   D* m_D;
};

#endif // __QTWORLDVIEWWIDGET_H__