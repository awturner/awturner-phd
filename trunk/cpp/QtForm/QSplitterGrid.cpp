#include "include/QSplitterGrid.h"

#include "include/QSplitterEx.h"

#include <QList>

#include "Useful/PrintMacros.h"

struct QSplitterGrid::D
{
   bool                m_Busy;
   QList<QSplitterEx*> m_List;
};

QSplitterGrid::QSplitterGrid( )
{
   m_D = new D;

   m_D->m_Busy = false;
}

QSplitterGrid::~QSplitterGrid( )
{
   delete m_D;
}

void QSplitterGrid::addSplitterEx( QSplitterEx* splitter )
{
   m_D->m_List.append( splitter );

   connect( splitter, SIGNAL( widthsChanged( QSplitterEx* ) ), this, SLOT( splitterResize( QSplitterEx* ) ) );
}

void QSplitterGrid::removeSplitterEx( QSplitterEx* splitter )
{
   disconnect( splitter, SIGNAL( widthsChanged( QSplitterEx* ) ), this, SLOT( splitterResize( QSplitterEx* ) ) );
   m_D->m_List.removeAll( splitter );
}

void QSplitterGrid::splitterResize( QSplitterEx* splitter )
{
   if ( m_D->m_Busy )
      return;

   m_D->m_Busy = true;

   QList<QSplitterEx*>::iterator it = m_D->m_List.begin( );
   QList<QSplitterEx*>::iterator en = m_D->m_List.end( );

   QList<int> sizes = splitter->sizes( );
   for ( int i = 0; i < sizes.size(); ++i )
      PRINTVBL( sizes[i] );

   while ( it != en )
   {
      if ( *it != splitter )
      {
         (*it)->setSizes( sizes );
      }
      ++it;
   }

   m_D->m_Busy = false;
}