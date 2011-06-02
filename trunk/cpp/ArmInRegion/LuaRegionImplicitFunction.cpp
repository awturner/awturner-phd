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