#ifndef __OPENGLTEMPLATEDFUNCTIONS_H__
#define __OPENGLTEMPLATEDFUNCTIONS_H__

#include "OpenGLHeaders.h"

template <class T>
void glVertex3Tv( const T* v );

template <class T>
void glNormal3Tv( const T* v );

template <class T>
void glTexCoord2Tv( const T* v );

template <class T>
void glVertex3T( const T a, const T b, const T c );

template <class T>
void glNormal3T( const T a, const T b, const T c );

template <class T>
void glTexCoord2T( const T a, const T b );

template <class T>
void glRasterPos3Tv( const T* v );

template <class T>
void glGetTv( GLenum pname, T* v );

template <class T>
void glMultMatrixT( const T* v );

#endif // __OPENGLTEMPLATEDFUNCTIONS_H__