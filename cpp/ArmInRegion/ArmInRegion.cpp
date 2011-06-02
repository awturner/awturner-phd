
#include "LuaRegion.h"

#include "LuaRegionImplicitFunction.h"
#include "vtkSampleFunction.h"
#include "vtkContourFilter.h"
#include "vtkPolyDataWriter.h"

#include "Useful/Noise.h"
#include "Useful/PrintMacros.h"

#include "Useful/ProfTimer.h"

#include "MicroscribeArm.h"

#include <fstream>

using namespace AWT;

double timeFunction( LuaRegion& region, const unsigned int ntests )
{
   ProfilingTimer timer;

   double* vtxs = new double[ 3*ntests ];
   for ( unsigned int i = 0; i < ntests; ++i )
   {
      vtxs[3*i + 0] = Noise<double>::randu( -20, 20 );
      vtxs[3*i + 1] = Noise<double>::randu( -20, 20 );
      vtxs[3*i + 2] = Noise<double>::randu( -20, 20 );
   }

   timer.start();
   for ( unsigned int i = 0; i < ntests; ++i )
      region.testPoint( &vtxs[3*i] );
   timer.stop();

   delete [] vtxs;

   return timer.getDurationInSecs( ) / ntests;
}

void traceShape( LuaRegion& region )
{
   LuaRegionImplicitFunction* regFunc = LuaRegionImplicitFunction::New( );
   regFunc->SetRegion( &region );

   vtkSampleFunction* sampFunc = vtkSampleFunction::New( );
   sampFunc->SetImplicitFunction( regFunc );

   int sampling[3];
   double bounds[6];

   region.getBounds( bounds );
   region.getSampling( sampling );

   sampFunc->SetModelBounds( bounds );
   sampFunc->SetSampleDimensions( sampling );

   sampFunc->Update( );

   vtkContourFilter* contFilt = vtkContourFilter::New( );
   contFilt->SetInputConnection( sampFunc->GetOutputPort( ) );

   vtkPolyDataWriter* writer = vtkPolyDataWriter::New( );
   writer->SetInputConnection( contFilt->GetOutputPort( ) );
   writer->SetFileName( "file.vtk" );

   writer->Update( );

   writer->Delete( );
   contFilt->Delete( );
   sampFunc->Delete( );
   regFunc->Delete( );

   DEBUGLINE;
}

#include <signal.h>

bool keepRunning = true;

void interrupted( int i )
{
   keepRunning = false;
}

int main( int argc, char** argv )
{
   MicroscribeArm* arm = MicroscribeArm::getInstance( );
   if ( !arm->connect( ) )
   {
      DEBUGMACRO( "Cannot connect to arm" );
      DEBUGLINEANDEXIT( 1 );
   }
   
   double pos[3], ori[3];

   signal( SIGINT, interrupted );
   
   keepRunning = true;
   while ( keepRunning )
   {
      arm->getPositionAndOrientation( pos, ori );
      PRINTVEC( pos, 3 );
   }

   arm->disconnect( );

   DEBUGLINEANDEXIT( 1 );

   try
   {
      LuaRegion region( "my.lua" );

      traceShape( region );

      system( "DisplayPolyData file.vtk -nocull" );

      // This is checking how fast the function runs
      PRINTVBL( timeFunction( region, 100000 ) );

      /*
      double vtx[3];
      std::ofstream os( "region.m" );

      os << "pts = [" << std::endl;
      for ( int i = 0; i < 5000; )
      {
         vtx[0] = Noise<double>::randu( -2, 2 );
         vtx[1] = Noise<double>::randu( -2, 2 );
         vtx[2] = Noise<double>::randu( -2, 2 );

         const double d = region.testPoint( vtx );

         if ( d < 0 && d > -0.05 )
         {
            ++i;
            os << vtx[0] << " " << vtx[1] << " " << vtx[2] << std::endl;
         }
      }

      os << "]';" << std::endl;
      os.close( );
      */
   }
   catch ( std::exception ex )
   {
      PRINTVBL( ex.what() );
   }
}