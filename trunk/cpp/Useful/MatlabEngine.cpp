#include "MatlabEngine.h"

#include "engine.h"

struct AWT::MatlabEngine::D
{
   static Engine* ms_Engine;
   static unsigned int ms_count;
};

Engine* AWT::MatlabEngine::D::ms_Engine = 0;
unsigned int AWT::MatlabEngine::D::ms_count  = 0;

AWT::MatlabEngine::MatlabEngine( )
{
   m_D = new D;

   if ( D::ms_Engine == 0 )
   {
      D::ms_Engine = engOpen( "\0" );

      if ( !D::ms_Engine )
      {
         throw "Could not start MATLAB Engine!";
      }
   }

   ++D::ms_count;
}

AWT::MatlabEngine::~MatlabEngine( )
{
   if ( --D::ms_count == 0 )
   {
      engClose( D::ms_Engine );
      D::ms_Engine = 0;
   }

   delete m_D;
}

AWT::MatlabEngine::P AWT::MatlabEngine::getInstance( )
{
   AUTOGETINSTANCE( AWT::MatlabEngine, ( ) );
}

GETNAMEMACRO( AWT::MatlabEngine );

Engine* AWT::MatlabEngine::getEngine( )
{
   return D::ms_Engine;
}

int AWT::MatlabEngine::eval( const char* cmd )
{
   return engEvalString( D::ms_Engine, cmd );
}

AWT::MatlabArray::P AWT::MatlabEngine::getVariable( const char* name )
{
   return MatlabArray::getInstance( this, name );
}

int AWT::MatlabEngine::putVariable( const char* name, AWT::MatlabArray::P var )
{
   return engPutVariable( D::ms_Engine, name, var->getArray() );
}