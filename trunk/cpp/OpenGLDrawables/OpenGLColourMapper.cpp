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