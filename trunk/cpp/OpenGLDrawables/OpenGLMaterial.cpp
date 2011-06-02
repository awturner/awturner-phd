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
#include "OpenGLMaterial.h"

struct AWT::OpenGLMaterial::D
{
   float m_Ambient[4];
   float m_Diffuse[4];
   float m_Specular[4];
   float m_Shininess;
   float m_Opacity;
};

AWT::OpenGLMaterial::OpenGLMaterial(const float* ambient, const float* diffuse, const float* specular, const float shininess, const float opacity)
{
   m_D = new D();

   m_D->m_Shininess = 128.f * shininess;
   m_D->m_Opacity   = opacity;

   for (unsigned int i = 0; i < 3; ++i)
   {
      m_D->m_Ambient[i]  = ambient[i];
      m_D->m_Diffuse[i]  = diffuse[i];
      m_D->m_Specular[i] = specular[i];
   }

   // Keep the opacity too
   m_D->m_Ambient[3]  = opacity;
   m_D->m_Diffuse[3]  = opacity;
   m_D->m_Specular[3] = opacity;
}

AWT::OpenGLMaterial::~OpenGLMaterial()
{
   delete m_D;
}

AWT::OpenGLMaterial::P AWT::OpenGLMaterial::getInstance(const float* ambient, const float* diffuse, const float* specular, const float shininess, const float opacity)
{
   AUTOGETINSTANCE(AWT::OpenGLMaterial, (ambient, diffuse, specular, shininess, opacity));
}

GETNAMEMACRO(AWT::OpenGLMaterial);

bool AWT::OpenGLMaterial::isOpaque() const
{
   return m_D->m_Opacity == 1.f;
}

void AWT::OpenGLMaterial::prepare()
{
   glEnable(GL_LIGHTING);

   glDisable(GL_COLOR_MATERIAL);

   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_D->m_Ambient);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_D->m_Diffuse);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_D->m_Specular);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_D->m_Shininess);

   glShadeModel(GL_SMOOTH);
}

void AWT::OpenGLMaterial::unprepare()
{
}

void AWT::OpenGLMaterial::tweak(const TweakType tw, const MeshIndex v)
{
}
