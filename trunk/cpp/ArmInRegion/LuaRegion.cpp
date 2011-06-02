#include "LuaRegion.h"

#include <exception>

#include "Useful/PrintMacros.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

template <class T>
T getfield (lua_State* L, const char *key) {
   T result;
   lua_pushstring(L, key);
   lua_gettable(L, -2);  /* get background[key] */
   if (!lua_isnumber(L, -1))
   {
      PRINTVBL( key );
      throw std::exception( "Not number" );
   }
   result = static_cast<T>( lua_tonumber(L, -1) );
   lua_pop(L, 1);  /* remove number */
   return result;
}


LuaRegion::LuaRegion( const char* filename )
: L( 0 )
{
   L = lua_open( );

   luaopen_io(L); // provides io.*
   luaopen_base(L);
   luaopen_table(L);
   luaopen_string(L);
   luaopen_math(L);

   if ( 0 != lua_dofile( L, filename ) )
      throw std::exception( "Could not load file" );

   lua_getglobal( L, "region" );
   if ( !lua_isfunction( L, -1 ) )
      throw std::exception( "region is not a function" );

   lua_pop( L, 1 );

   /*
   lua_getglobal( L, "bounds" );
   if ( !lua_istable( L, -1 ) )
      throw std::exception( "bounds variable not found!" );

   bounds[0] = getfield<double>( L, "xmin" );
   bounds[1] = getfield<double>( L, "xmax" );
   bounds[2] = getfield<double>( L, "ymin" );
   bounds[3] = getfield<double>( L, "ymax" );
   bounds[4] = getfield<double>( L, "zmin" );
   bounds[5] = getfield<double>( L, "zmax" );
   PRINTVEC( bounds, 6 );
   
   lua_pop( L, 1 );

   lua_getglobal( L, "sampling" );
   if ( !lua_istable( L, -1 ) )
      throw std::exception( "sampling variable not found!" );

   sampling[0] = getfield<int>( L, "nx" );
   sampling[1] = getfield<int>( L, "ny" );
   sampling[2] = getfield<int>( L, "nz" );
   PRINTVEC( sampling, 3 );

   lua_pop( L, 1 );
   */
}

LuaRegion::~LuaRegion( )
{
   lua_close( L );
}

void LuaRegion::getBounds( double* b )
{
   for ( int i = 0; i < 6; ++i )
      b[i] = bounds[i];
}

void LuaRegion::getSampling( int* s )
{
   for ( int i = 0; i < 3; ++i )
      s[i] = sampling[i];
}

double LuaRegion::testPoint( const double* x )
{
   lua_getglobal( L, "region" );
   lua_pushnumber( L, x[0] );
   lua_pushnumber( L, x[1] );
   lua_pushnumber( L, x[2] );

   if ( lua_pcall( L, 3, 1, 0 ) != 0 )
   {
      printf( "%s\n", lua_tostring( L, -1 ) );
      throw std::exception( "Error running region function" );
   }

   if ( !lua_isnumber( L, -1 ) )
      throw std::exception( "region function must return a number" );

   double ret = lua_tonumber( L, -1 );
   lua_pop( L, 1 );

   return ret;
}