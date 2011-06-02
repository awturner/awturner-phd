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
