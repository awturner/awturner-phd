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
#include "LuaPlus.h"
#include "Useful/PrintMacros.h"

#include <string>

using namespace LuaPlus;

void readvec3( LuaObject& object, double v[3] )
{
   for ( unsigned int i = 0; i < 3; ++i )
      v[i] = object[ i+1 ].GetDouble( );
}

void main( int argc, char** argv ) 
{
   if ( argc < 2 )
   {
      DEBUGMACRO( "Must provide a lua script filename!" );
      DEBUGLINEANDEXIT( 1 );
   }

   //Create state
   LuaStateOwner state;

   int iret = state->DoFile( argv[1] );
   if ( iret != 0 )
   {
      DEBUGLINEANDEXIT( iret );
   }


   //LuaObject objLoadFlag = state->GetGlobal( "loadstatemachine" );
   //if ( objLoadFlag.IsBoolean( ) && objLoadFlag.GetBoolean( ) )
   //{
   //   LuaObject objSm = state->GetGlobal( "StateMachine" );

   //   LuaFunction<LuaObject> fnStep = objSm["step"];
   //   LuaFunction<LuaObject> fnStopped = objSm["isStopped"];
   //   LuaFunction<LuaObject> fnDesc = objSm["description"];

   //   for ( int i = 0; i < 10; ++i )
   //   {
   //      PRINTVBL2( "Current State", fnDesc( objSm ).ToString( ) );

   //      LuaObject stopped = fnStopped( objSm );
   //      if ( !stopped.IsBoolean( ) || stopped.GetBoolean( ) )
   //      {
   //         break;
   //      }

   //      fnStep( objSm );
   //   }

   //   /*
   //   LuaObject objObj = state->GetGlobal( "statemachine" );

   //   PRINTVBL( objObj["setup"].TypeName( ) );

   //   PRINTVBL( objObj.TypeName( ) );
   //   //PRINTVBL( objObj.GetTableCount( ) );

   //   //for ( int i = 1; i <= objObj.GetTableCount( ); ++i )
   //   //{
   //   //   PRINTVBL( objObj[i].TypeName( ) );
   //   //}
   //   */

   //   DEBUGLINE;
   //   return;
   //}

   LuaObject objJob = state->GetGlobal( "alignment" );

   std::string sourceFilename( objJob["source"].GetString( ) );
   std::string targetFilename( objJob["target"].GetString( ) );

   PRINTVBL( sourceFilename );
   PRINTVBL( targetFilename );

   double rigid[4][4];

   LuaObject objRigid = objJob["rigid"];
   if ( objRigid.IsTable( ) )
   {
      for ( int r = 0; r < 4; ++r )
      {
         LuaObject objRow = objRigid[r+1];

         for ( int c = 0; c < 4; ++c )
            rigid[r][c] = objRow[c+1].GetDouble( );
      }
   }

   for ( int r = 0; r < 4; ++r )
   {
      PRINTVEC( rigid[r], 4 );
   }

   LuaObject objNonRigid = objJob["nonrigid"];
   if ( objNonRigid.IsTable( ) )
   {
      PRINTVBL( objNonRigid.GetTableCount( ) );

      for ( int i = 1; i <= objNonRigid.GetTableCount( ); ++i )
      {
         LuaObject objControlPoint = objNonRigid[i];

         bool used    = objControlPoint["used"].GetBoolean( );

         double position[3], value[3];
         readvec3( objControlPoint["position"], position );
         readvec3( objControlPoint["value"], value );

         //DEBUGMACRO( i << "\t" << (used?"true":"false") << "\t" );
         //PRINTVEC( position, 3 );
         //PRINTVEC( value, 3 );
      }
   }
}