#include "include/FormPointPicker.h"

#include <QApplication>

int main( int argc, char** argv )
{
   QApplication app( argc, argv );

   FormPointPicker* frm = new FormPointPicker( );

   frm->showMaximized( );

   return app.exec( );
}