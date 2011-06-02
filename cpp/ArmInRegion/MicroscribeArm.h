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
#ifndef __MicroscribeArm_H__
#define __MicroscribeArm_H__

#include <windows.h>
#include <armdll32.h>

#include <time.h>

#include <map>

class MicroscribeArm
{
public:
   static MicroscribeArm* getInstance( );

   virtual bool connect( );
   virtual bool disconnect( );

   virtual bool isConnected( );

   virtual double getPosition( double* pos );
   virtual double getOrientation( double* ori );
   virtual double getPositionAndOrientation( double* pos, double* ori, double* joints = 0 );

   virtual void getMatrixRowMajor( double* mat );
   virtual void getMatrixColMajor( double* mat );

   virtual bool needsToUpdate( );
   virtual int getUpdateFrequency( );

   MicroscribeArm( int updatesPerSecond = 50 );
   ~MicroscribeArm( );
   
protected:

   virtual void zeroPositionAndOrientation( );
   virtual void doUpdateIfNeeded( );

   template <class T>
   void swap( T& a, T& b );
   void transposeMatrix( double* mat );

   inline bool isAtLeastOneInstanceConnected( );
   static void disconnectAll( );
   
   static clock_t   lastUpdate;
   static clock_t   clocksPerUpdate;

   length_3D position;
   angle_3D  orientation;
   angle     joints[6];

   const int    updatesPerSecond;
   const double secondsPerClock;

   static int  connectedCount;
   bool        connected;
   bool        canConnect;

   static double PIBY180;
};


#endif // __MicroscribeArm_H__
