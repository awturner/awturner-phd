#include "LuaRegionImplicitFunction.h"

#include "vtkObjectFactory.h"

#include "LuaRegion.h"

#include <limits>

#include "Useful/PrintMacros.h"

vtkStandardNewMacro( LuaRegionImplicitFunction );

void LuaRegionImplicitFunction::SetRegion( LuaRegion* reg )
{
   this->Region = reg;
   this->Step = 1e-5;
}

LuaRegion* LuaRegionImplicitFunction::GetRegion( )
{
   return Region;
}

void LuaRegionImplicitFunction::SetStep(double step)
{
   this->Step = step;
}

double LuaRegionImplicitFunction::GetStep( )
{
   return this->Step;
}

double LuaRegionImplicitFunction::EvaluateFunction( double x[] )
{
   double ret = ( Region ) ? -Region->testPoint( x ) : std::numeric_limits<double>::quiet_NaN();

   return ret;
}

void LuaRegionImplicitFunction::EvaluateGradient( double x[], double g[] )
{
   if ( !Region )
   {
      g[0] = g[1] = g[2] = std::numeric_limits<double>::quiet_NaN();
      return;
   }

   double xp[3];
   double xm[3];

   for ( int ax = 0; ax < 3; ++ax )
   {
      for ( int axx = 0; axx < 3; ++axx )
      {
         xp[axx] = x[axx] + ( ax == axx ) ? Step : 0;
         xm[axx] = x[axx] - ( ax == axx ) ? Step : 0;
      }

      g[ax] = ( EvaluateFunction( xp ) - EvaluateFunction( xm ) ) / ( 2*Step );
   }
}

LuaRegionImplicitFunction::LuaRegionImplicitFunction( )
{
   Region = 0;
   Step = 1e-3;
}

LuaRegionImplicitFunction::~LuaRegionImplicitFunction( )
{

}