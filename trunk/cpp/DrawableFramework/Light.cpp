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
#include "Light.h"

AWT::Light::P AWT::Light::getInstance()
{
   AUTOGETINSTANCE(Light, ());
}

AWT::Light::Light()
{
}

AWT::Light::~Light()
{
}

GETNAMEMACRO(AWT::Light);

void AWT::Light::setPosition(const float* position)
{
   for (int i = 0; i < 4; ++i)
      m_Position[i] = position[i];
}

void AWT::Light::getPosition(float* out_Position) const
{
   for (int i = 0; i < 4; ++i)
      out_Position[i] = m_Position[i];
}

void AWT::Light::setAmbient(const float* ambient)
{
   for (int i = 0; i < 4; ++i)
      m_Ambient[i] = ambient[i];
}

void AWT::Light::getAmbient(float* out_Ambient) const
{
   for (int i = 0; i < 4; ++i)
      out_Ambient[i] = m_Ambient[i];
}

void AWT::Light::setDiffuse(const float* diffuse)
{
   for (int i = 0; i < 4; ++i)
      m_Diffuse[i] = diffuse[i];
}

void AWT::Light::getDiffuse(float* out_Diffuse) const
{
   for (int i = 0; i < 4; ++i)
      out_Diffuse[i] = m_Diffuse[i];
}

void AWT::Light::setSpecular(const float* specular)
{
   for (int i = 0; i < 4; ++i)
      m_Specular[i] = specular[i];
}

void AWT::Light::getSpecular(float* out_Specular) const
{
   for (int i = 0; i < 4; ++i)
      out_Specular[i] = m_Specular[i];
}