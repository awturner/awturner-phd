#include "NonRigidAlignment/NonRigidAlignmentStateMachine.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <signal.h>

#include "Mesh/Mesh.h"
#include "NonRigidAlignment/NonRigidAlignment.h"

using namespace AWT;

NonRigidAlignmentStateMachine<double>::P nrasm;

void handleSignal( int sig )
{
   if ( *nrasm != 0 )
   {
      DEBUGMACRO( "Calling external stop method!" );
      DEBUGMACRO( "(Pressing Ctrl-C again will quit the program normally)" );
      nrasm->externalStop( );
   }
}

int main( int argc, char** argv )
{
   DEBUGLINE;
   if ( argc < 3 )
   {
      //std::cerr << "usage: NonRigidAlignmentCLI <source mesh> <target mesh> <output filename> <output deformed mesh filename>" << std::endl;
      std::cerr << "usage: NonRigidAlignmentCLI <input lua file> <output root>" << std::endl;
      exit(1);
   }

   signal( SIGINT, handleSignal );

   StateMachineStack::P sms = StateMachineStack::getInstance( );
   nrasm = NonRigidAlignmentStateMachine<double>::getInstance( sms, false );

   DEBUGLINE;
   if ( !nrasm->load( argv[1] ) )
   {
      DEBUGMACRO( "Could not load " << argv[1] );
      std::cerr << nrasm->getLastError( ) << std::endl;
      DEBUGLINEANDEXIT( 1 );
   }
   DEBUGLINE;

   if ( argc == 4 )
   {
      DEBUGMACRO( "Only loading and saving; no processing!" );
      nrasm->loadOnly( );
   }

   unsigned int el = 0;

   DEBUGMACRO( "Starting main loop..." );
   while ( nrasm->isRunning( ) && !nrasm->isError( ) )
   {
      nrasm->step( );

      if (false && nrasm->aboutToRefine())
      {
         if ( el < 4 )
         {
            std::stringstream defOutput; defOutput << argv[2] << ".level" << el << ".deformed.ply";
            std::stringstream tgtOutput; tgtOutput << argv[2] << ".level" << el << ".target.ply";

            nrasm->saveDeformedModel( defOutput.str( ).c_str( ) );
            nrasm->saveTargetModel( tgtOutput.str( ).c_str( ) );

            ++el;
         }
      }
   }

   if ( !nrasm->isError( ) )
   {
      //std::stringstream luaOutput; luaOutput << argv[2] << ".lua";
      //nrasm->save( luaOutput.str( ) );

      //std::stringstream defOutput; defOutput << argv[2] << ".deformed.ply";
      //nrasm->saveDeformedModel( defOutput.str( ).c_str( ) );

      std::stringstream prjOutput; prjOutput << argv[2] << ".projected.ply";
      nrasm->saveProjected( prjOutput.str( ).c_str( ) );

      //std::stringstream tgtOutput; tgtOutput << argv[2] << ".target.ply";
      //nrasm->saveTargetModel( tgtOutput.str( ).c_str( ) );

      std::cerr << "Success." << std::endl;
   }
   else
   {
      std::cerr << "An error was encountered:" << std::endl;  
      std::cerr << nrasm->getLastError( ) << std::endl;
   }
}