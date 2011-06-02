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
#include "OpenGLTexture.h"

#include "OpenGLHeaders.h"

struct AWT::OpenGLTexture::D
{
   bool lighting;
   GLuint texid;
};

#define CHECKGL(GLcall) { GLcall; if (1) CheckGLError(#GLcall, __FILE__, __LINE__); }
#define CHECKGL_MSG(msg) { if (1) CheckGLError(#msg, __FILE__, __LINE__); }

int CheckGLError(const char *GLcall, const char *file, int line)
{
   GLenum glErr;
   int retCode = 0;
   while ((glErr=glGetError()) != GL_NO_ERROR)
   {
      switch(glErr)
      {
      case GL_INVALID_ENUM:
         printf("GL_INVALID_ENUM error in File %s at line: %d\n", file, line);
         break;
      case GL_INVALID_VALUE:
         printf("GL_INVALID_VALUE error in File %s at line: %d\n", file, line);
         break;
      case GL_INVALID_OPERATION:
         printf("GL_INVALID_OPERATION error in File %s at line: %d\n", file, line);
         break;
      case GL_STACK_OVERFLOW:
         printf("GL_STACK_OVERFLOW error in File %s at line: %d\n", file, line);
         break;
      case GL_STACK_UNDERFLOW:
         printf("GL_STACK_UNDERFLOW error in File %s at line: %d\n", file, line);
         break;
      case GL_OUT_OF_MEMORY:
         printf("GL_OUT_OF_MEMORY error in File %s at line: %d\n", file, line);
         break;
      default:
         printf("UNKNOWN ERROR in File %s at line: %d\n", file, line);
      }
      PAUSE;
   }
   return retCode;
}

AWT::OpenGLTexture::OpenGLTexture(const unsigned char* data, unsigned int w, unsigned int h, bool lighting)
{
   m_D = new D;
   m_D->lighting = lighting;

   CHECKGL(glGenTextures(1, &m_D->texid));

   glBindTexture(GL_TEXTURE_2D, m_D->texid);
   CheckGLError("glBindTexture", __FILE__, __LINE__);

   // select modulate to mix texture with color for shading
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   CheckGLError("glTexEnvf", __FILE__, __LINE__);

   // when texture area is small, bilinear filter the closest mipmap
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
      GL_LINEAR_MIPMAP_LINEAR);
   // when texture area is large, bilinear filter the first mipmap
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   // if wrap is true, the texture wraps over at the edges (repeat)
   //       ... false, the texture ends at the edges (clamp)
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   // build our texture mipmaps
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h,
      GL_RGB, GL_UNSIGNED_BYTE, data);

}

AWT::OpenGLTexture::~OpenGLTexture()
{
   delete m_D;
}

AWT::OpenGLTexture::P AWT::OpenGLTexture::getInstance(const unsigned char* data, unsigned int w, unsigned int h, bool lighting)
{
   AUTOGETINSTANCE(AWT::OpenGLTexture, (data, w, h, lighting));
}

GETNAMEMACRO(AWT::OpenGLTexture);

bool AWT::OpenGLTexture::isOpaque() const
{
   return true;
}

void AWT::OpenGLTexture::prepare()
{
   if (m_D->lighting)
      glEnable(GL_LIGHTING);
   else
      glDisable(GL_LIGHTING);

   glColor3f(1.f, 1.f, 1.f);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, m_D->texid);

   /*
   glMatrixMode(GL_TEXTURE);
   glPushMatrix();

   glLoadIdentity();
   glScalef(1.f, 1.f, 1.f);
   glMatrixMode(GL_MODELVIEW);
   */
}

void AWT::OpenGLTexture::unprepare()
{
   glDisable(GL_TEXTURE_2D);

   glMatrixMode(GL_TEXTURE);
   glPopMatrix();

   glMatrixMode(GL_MODELVIEW);
}

void AWT::OpenGLTexture::tweak(const TweakType /*tw*/, const MeshIndex /*v*/)
{
   // Don't do anything
}

bool AWT::OpenGLTexture::isLighting() const
{
   return m_D->lighting;
}

void AWT::OpenGLTexture::setLighting(const bool v)
{
   if (v ^ m_D->lighting)
   {
      m_D->lighting = v;
      modified();
   }
}