#include "include/QtSegEditForm.h"

int main( int argc, char** argv )
{
   QApplication app( argc, argv );

   QtSegEditForm* form = new QtSegEditForm( );

   form->showMaximized( );

   return app.exec( );
}