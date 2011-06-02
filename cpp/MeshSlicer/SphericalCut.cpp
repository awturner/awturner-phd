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
#include "SphericalCut.h"

template <class T>
struct AWT::SphericalCut<T>::D
{
   T m_Sphere[4];
};

template <class T>
AWT::SphericalCut<T>::SphericalCut()
{
   m_D = new D;

   m_D->m_Sphere[0] = 0;
   m_D->m_Sphere[1] = 0;
   m_D->m_Sphere[2] = 0;
   m_D->m_Sphere[3] = 1;
}

template <class T>
AWT::SphericalCut<T>::~SphericalCut()
{
   delete m_D;
}

template <class T>
typename AWT::SphericalCut<T>::P AWT::SphericalCut<T>::getInstance()
{
   AUTOGETINSTANCE(AWT::SphericalCut<T>, ());
}

template <class T>
GETNAMEMACRO(AWT::SphericalCut<T>);

template <class T>
void AWT::SphericalCut<T>::getCentre(T* out_Centre) const
{
   for (int i = 0; i < 3; ++i)
      out_Centre[i] = m_D->m_Sphere[i];
}

template <class T>
void AWT::SphericalCut<T>::setCentre(const T* in_Centre)
{
   for (int i = 0; i < 3; ++i)
      m_D->m_Sphere[i] = in_Centre[i];
}

template <class T>
T AWT::SphericalCut<T>::getRadius() const
{
   return m_D->m_Sphere[3];
}

template <class T>
void AWT::SphericalCut<T>::setRadius(const T in_Radius)
{
   m_D->m_Sphere[3] = in_Radius;
}

template <class T>
bool AWT::SphericalCut<T>::accept(T* vtx)
{
   T tmp = m_D->m_Sphere[3];
   T disc = tmp*tmp;

   for (int i = 0; i < 3; ++i)
   {
      tmp = m_D->m_Sphere[i] - vtx[i];
      disc -= tmp*tmp;
   }

   return disc >= 0;
}

template class AWT::SphericalCut<double>;
template class AWT::SphericalCut<float>;