#ifndef __LUAREGION_H__
#define __LUAREGION_H__

struct lua_State;

class LuaRegion
{
public:
   LuaRegion( const char* filename );
   virtual ~LuaRegion( );

   double testPoint( const double* x );

   void getBounds( double* b );
   void getSampling( int* s );

protected:
   lua_State* L;

   double bounds[6];
   int sampling[3];
};

#endif // __LUAREGION_H__