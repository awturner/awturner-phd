#include "MyScrollArea.h"

#include <QWheelEvent>
#include <QKeyEvent>

#include "Useful/PrintMacros.h"

MyScrollArea::MyScrollArea( QWidget* parent )
: QScrollArea( parent )
{
}

void MyScrollArea::wheelEvent( QWheelEvent* ev )
{
   const Qt::KeyboardModifiers mod = ev->modifiers( );

   if ( mod == Qt::ControlModifier )
   {
      emit requestSliceChange( ev->delta( ) > 0 ? 1 : -1 );
   }
   else if ( mod == (Qt::ControlModifier | Qt::ShiftModifier) )
   {
      emit requestSliceChange( ev->delta( ) > 0 ? 10 : -10 );
   }
   else if ( mod == 0 )
   {
      QScrollArea::wheelEvent( ev );
   }
}

void MyScrollArea::keyPressEvent( QKeyEvent* ev )
{
   if ( ev->modifiers( ) == 0 )
   {
      switch ( ev->key( ) )
      {
      case Qt::Key_PageUp:
         emit requestSliceChange( 10 );
         break;
      case Qt::Key_PageDown:
         emit requestSliceChange( -10 );
         break;
      }
   }
   else if ( ev->modifiers( ) & Qt::ControlModifier )
   {
      if ( ev->modifiers( ) == Qt::ControlModifier )
      {
         switch ( ev->key( ) )
         {
         case Qt::Key_Up:
            emit requestSliceChange( 1 );
            break;
         case Qt::Key_Down:
            emit requestSliceChange( -1 );
            break;
         }
      }
   }
}