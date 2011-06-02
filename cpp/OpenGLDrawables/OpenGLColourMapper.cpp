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
#include "OpenGLColourMapper.h"

#include "OpenGLHeaders.h"

#include <cmath>

template <class T>
struct AWT::OpenGLColourMapper<T>::D
{
   static T clamp( T val, T minv, T maxv )
   {
      return std::max( std::min( maxv, val ), minv );
   }

   static T typedDiv( T num, T den )
   {
      return num / den;
   }

   static T mapVal( T val, T p )
   {
      // This should be equivalent to
      // clamp to [0..1] { 3/2 - 4*abs( val - p/4 ) }

      return clamp( typedDiv(3,2) - 4*abs( val - typedDiv( p, 4 ) ), 0, 1 );
   }
};

template <class T>
AWT::OpenGLColourMapper<T>::OpenGLColourMapper( )
{
   m_D = new D;
}

template <class T>
AWT::OpenGLColourMapper<T>::~OpenGLColourMapper( )
{
   delete m_D;
}

template <class T>
typename AWT::OpenGLColourMapper<T>::P AWT::OpenGLColourMapper<T>::getInstance( )
{
   AUTOGETINSTANCE( AWT::OpenGLColourMapper<T>, ( ) );
}

template <class T>
GETNAMEMACRO( AWT::OpenGLColourMapper<T> );

template <class T>
bool AWT::OpenGLColourMapper<T>::isOpaque( ) const
{
   return false;
}

template <class T>
void AWT::OpenGLColourMapper<T>::prepare( )
{
   glEnable( GL_LIGHTING );
}

template <class T>
void AWT::OpenGLColourMapper<T>::tweak( const typename AWT::OpenGLColourMapper<T>::TweakType tw, const MeshIndex v )
{
   if ( tw != DrawMaterial::TWEAK_VERTEX )
      return;

   Tuples<T>::P data = getData( );

   if ( v >= data->getNumberOfPoints( ) )
      return;

   T vtx[3];
   GLfloat colour[4];

   data->getPoint( v, vtx );

   for ( int i = 0; i < 3; ++i )
   {
      vtx[i] *= 5;
      vtx[i] = vtx[i] - floor( vtx[i] );

      colour[i] = ( vtx[i] >= 0.5 ) ? 255 : 0;
      //colour[i] = static_cast<float>( vtx[i] - floor( vtx[i] ) );
   }

   /*
   T val = data->getPointElement( v, 0 );

   val = ( val - getMin() ) / ( getMax() - getMin() );
   val = D::clamp( val, 0, 1 );

   colour[0] = static_cast<float>( D::mapVal( val, 3 ) );
   colour[1] = static_cast<float>( D::mapVal( val, 2 ) );
   colour[2] = static_cast<float>( D::mapVal( val, 1 ) );
   */

   colour[3] = 1.f;

   glColor4fv( colour );
}

template <class T>
void AWT::OpenGLColourMapper<T>::unprepare( )
{
}

template class AWT::OpenGLColourMapper<double>;
template class AWT::OpenGLColourMapper<float>;