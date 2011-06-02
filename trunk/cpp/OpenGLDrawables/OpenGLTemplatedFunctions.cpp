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
#include "OpenGLTemplatedFunctions.h"

#include "OpenGLHeaders.h"

template <>
void glNormal3T<double>(const double a, const double b, const double c)
{
   glNormal3d(a, b, c);
}

template <>
void glNormal3T<float>(const float a, const float b, const float c)
{
   glNormal3f(a, b, c);
}

template <>
void glVertex3T<double>(const double a, const double b, const double c)
{
   glVertex3d(a, b, c);
}

template <>
void glVertex3T<float>(const float a, const float b, const float c)
{
   glVertex3f(a, b, c);
}

template <>
void glTexCoord2T<double>(const double a, const double b)
{
   glTexCoord2d(a, b);
}

template <>
void glTexCoord2T<float>(const float a, const float b)
{
   glTexCoord2f(a, b);
}


template <>
void glGetTv<double>(GLenum pname, double* v)
{
   glGetDoublev(pname, v);
}

template <>
void glGetTv<float>(GLenum pname, float* v)
{
   glGetFloatv(pname, v);
}

template <>
void glVertex3Tv<double>(const double* v)
{
   glVertex3dv(v);
}

template <>
void glVertex3Tv<float>(const float* v)
{
   glVertex3fv(v);
}

template <>
void glTexCoord2Tv<double>(const double* v)
{
   glTexCoord2dv(v);
}

template <>
void glTexCoord2Tv<float>(const float* v)
{
   glTexCoord2fv(v);
}

template <>
void glNormal3Tv<double>(const double* v)
{
   glNormal3dv(v);
}

template <>
void glNormal3Tv<float>(const float* v)
{
   glNormal3fv(v);
}

template <>
void glMultMatrixT<double>(const double* v)
{
   glMultMatrixd(v);
}

template <>
void glMultMatrixT<float>(const float* v)
{
   glMultMatrixf(v);
}

template <>
void glRasterPos3Tv<double>(const double* a)
{
   glRasterPos3dv(a);
}

template <>
void glRasterPos3Tv<float>(const float* a)
{
   glRasterPos3fv(a);
}
