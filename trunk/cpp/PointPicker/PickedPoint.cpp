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
#include "include/PickedPoint.h"

#include <limits>

PickedPoint::PickedPoint( )
{
   setName( QObject::tr( "Unnamed Point" ) );
   const double pnt[] = { std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity() };
   setPoint( pnt );
}

PickedPoint::PickedPoint( const PickedPoint& other )
{
   setName( other.getName() );

   double pnt[3];
   other.getPoint( pnt );
   setPoint( pnt );
}

PickedPoint::PickedPoint( const QString& name )
{
   setName( name );

   const double pnt[] = { std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity() };
   setPoint( pnt );
}

PickedPoint::PickedPoint( const QString& name, const double pnt[3] )
{
   setName( name );
   setPoint( pnt );
}

PickedPoint::~PickedPoint( )
{
}

void PickedPoint::getPoint( double pnt[3] ) const
{
   for ( unsigned int i = 0; i < 3; ++i )
      pnt[i] = m_Pnt[i];
}

void PickedPoint::setPoint( const double pnt[3] )
{
   for ( unsigned int i = 0; i < 3; ++i )
      m_Pnt[i] = pnt[i];
}

QString PickedPoint::getName( ) const
{
   return m_Name;
}

void PickedPoint::setName( const QString& name )
{
   m_Name = name;
}