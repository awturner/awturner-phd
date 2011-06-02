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
#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   class Light : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<Light> P;

   public:
      static P getInstance();

      void setPosition(const float* position);
      void getPosition(float* out_Position) const;

      void setAmbient(const float* ambient);
      void getAmbient(float* out_Ambient) const;

      void setDiffuse(const float* diffuse);
      void getDiffuse(float* out_Diffuse) const;

      void setSpecular(const float* specular);
      void getSpecular(float* out_Specular) const;

      virtual std::string getClassName() const;

   protected:
      Light();
      virtual ~Light();

      float m_Position[4];
      float m_Ambient[4];
      float m_Diffuse[4];
      float m_Specular[4];
   };
}

#endif __LIGHT_H__