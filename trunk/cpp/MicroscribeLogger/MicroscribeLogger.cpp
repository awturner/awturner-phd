#include "include/MicroscribeLoggerForm.h"

#include "ArmInRegion/MicroscribeArm.h"

#include <QMessageBox>

#include "Useful/PrintMacros.h"

int main( int argc, char** argv )
{
  QApplication app( argc, argv );

  MicroscribeArm* arm = new MicroscribeArm( 50 );

  if ( !arm->connect( ) )
  {
    QMessageBox::critical( 0, QObject::tr( "Critical Error" ), QObject::tr( "Could not connect to Microscribe Arm; exiting." ) );
    exit( 1 );
  }

  MicroscribeLoggerForm* mlf = new MicroscribeLoggerForm( );

  mlf->setMicroscribeArm( arm );
  mlf->show( );

  int ret = app.exec( );

  if ( arm->isConnected( ) )
    arm->disconnect( );

  delete arm;

  return ret;
}