#include "OpenGLTemplatedFunctions.h"

#include "OpenGLHeaders.h"

template <>
void glNormal3T<double>( const double a, const double b, const double c )
{
   glNormal3d( a, b, c );
}

template <>
void glNormal3T<float>( const float a, const float b, const float c )
{
   glNormal3f( a, b, c );
}

template <>
void glVertex3T<double>( const double a, const double b, const double c )
{
   glVertex3d( a, b, c );
}

template <>
void glVertex3T<float>( const float a, const float b, const float c )
{
   glVertex3f( a, b, c );
}

template <>
void glTexCoord2T<double>( const double a, const double b )
{
   glTexCoord2d( a, b );
}

template <>
void glTexCoord2T<float>( const float a, const float b )
{
   glTexCoord2f( a, b );
}


template <>
void glGetTv<double>( GLenum pname, double* v )
{
   glGetDoublev( pname, v );
}

template <>
void glGetTv<float>( GLenum pname, float* v )
{
   glGetFloatv( pname, v );
}

template <>
void glVertex3Tv<double>( const double* v )
{
   glVertex3dv( v );
}

template <>
void glVertex3Tv<float>( const float* v )
{
   glVertex3fv( v );
}

template <>
void glTexCoord2Tv<double>( const double* v )
{
   glTexCoord2dv( v );
}

template <>
void glTexCoord2Tv<float>( const float* v )
{
   glTexCoord2fv( v );
}

template <>
void glNormal3Tv<double>( const double* v )
{
   glNormal3dv( v );
}

template <>
void glNormal3Tv<float>( const float* v )
{
   glNormal3fv( v );
}

template <>
void glMultMatrixT<double>( const double* v )
{
   glMultMatrixd( v );
}

template <>
void glMultMatrixT<float>( const float* v )
{
   glMultMatrixf( v );
}

template <>
void glRasterPos3Tv<double>( const double* a )
{
   glRasterPos3dv( a );
}

template <>
void glRasterPos3Tv<float>( const float* a )
{
   glRasterPos3fv( a );
}
