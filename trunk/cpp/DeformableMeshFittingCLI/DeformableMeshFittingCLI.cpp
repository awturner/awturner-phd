#include "DeformableMeshFitting/DeformableMeshFittingStateMachine.h"

#include <iostream>

#include <signal.h>
#include <string>
#include <new>

#include "vld.h"

#include "StateMachine/StateMachineStack.h"

using namespace AWT;

DeformableMeshFittingStateMachine<double>::P sm;
std::string saveFilename;

void handleInterrupt( int i )
{
   switch ( i )
   {
   case SIGINT:
      if ( *sm != 0 )
      {
         sm->save( saveFilename );
         std::cerr << "Saved to " << saveFilename << std::endl;
      }

      AWT::ReferenceCountedObject::report( );

      DEBUGLINEANDEXIT( 1 );

      break;
   }
}

int main( int argc, char** argv )
{
   try
   {
      if ( argc < 3 )
      {
         std::cerr << "Usage: DeformableMeshFittingCLI <source filename> <output filename>" << std::endl;
         DEBUGLINEANDEXIT( 1 );
      }

      StateMachineStack::P sms = StateMachineStack::getInstance( );

      for ( int i = 1; i < argc; i += 2 )
      {
         saveFilename = std::string( argv[i+1] );

         while ( *sms->getTop() != 0 )
            sms->pop();

         sm = DeformableMeshFittingStateMachine<double>::getInstance( sms );

         sm->load( argv[i+0] );
         
         //signal( SIGINT, handleInterrupt );

         if ( i+2 < argc && !strcmp( argv[i+2], "-evalonly" ) )
         {
            DEBUGLINE;
            ++i;
            sm->evalOnly( );
         }

         try
         {
            while ( sm->isRunning( ) && !sm->isError( ) )
            {
               std::cerr << "State: " << sm->getStateName( ) << std::endl;
               sm->step( );
            }
         }
         catch ( const std::bad_alloc& x )
         {
            std::cerr << "Out of memory: " << x.what( ) << std::endl;
         }
         
         if ( sm->isError( ) )
            std::cerr << sm->getLastError( ) << std::endl;

         sm->save( saveFilename );
      }

      //DEBUGLINEANDEXIT( 99 );
      DEBUGLINE;
   }
   catch ( ... )
   {
      std::cerr << "An exception was caught" << std::endl;
   }

   //DEBUGLINE;

   //AWT::ReferenceCountedObject::report( );

   //DEBUGLINE;
}