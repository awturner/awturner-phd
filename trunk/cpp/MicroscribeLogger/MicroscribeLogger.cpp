/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
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