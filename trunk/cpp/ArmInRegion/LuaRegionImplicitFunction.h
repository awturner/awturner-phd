#ifndef __LUAREGIONIMPLICITFUNCTION_H__
#define __LUAREGIONIMPLICITFUNCTION_H__

#include "vtkImplicitFunction.h"

class LuaRegion;

class LuaRegionImplicitFunction : public vtkImplicitFunction
{
public:

   static LuaRegionImplicitFunction* New( );

   void SetRegion( LuaRegion* reg );
   LuaRegion* GetRegion( );

   void SetStep( double step );
   double GetStep( );

   virtual double EvaluateFunction( double x[] );
   virtual void EvaluateGradient( double x[], double g[] );

protected:
   LuaRegionImplicitFunction( );
   virtual ~LuaRegionImplicitFunction( );

   LuaRegion* Region;
   double Step;
};

#endif // __LUAREGIONIMPLICITFUNCTION_H__