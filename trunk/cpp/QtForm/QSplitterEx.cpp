#include "include/QSplitterEx.h"

#include "Useful/PrintMacros.h"

QSplitterEx::QSplitterEx( QWidget* parent )
: QSplitter( parent )
{
   connect( this, SIGNAL( splitterMoved( int, int ) ), this, SLOT( handleSplitterMoved( ) ) );
}

void QSplitterEx::handleSplitterMoved( )
{
   emit widthsChanged( this );
}