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
#include "PlanarCut.h"

#include "Useful/ArrayVectorFunctions.h"

template <class T>
struct AWT::PlanarCut<T>::D
{
   T m_Plane[4];
};

template <class T>
AWT::PlanarCut<T>::PlanarCut()
{
   m_D = new D;

   m_D->m_Plane[0] = 0;
   m_D->m_Plane[1] = 0;
   m_D->m_Plane[2] = 1;
   m_D->m_Plane[3] = 0;
}

template <class T>
AWT::PlanarCut<T>::~PlanarCut()
{
   delete m_D;
}

template <class T>
typename AWT::PlanarCut<T>::P AWT::PlanarCut<T>::getInstance()
{
   AUTOGETINSTANCE(AWT::PlanarCut<T>, ());
}

template <class T>
GETNAMEMACRO(AWT::PlanarCut<T>);

template <class T>
void AWT::PlanarCut<T>::setPlane(const T* in_Plane)
{
   for (int i = 0; i < 4; ++i)
      m_D->m_Plane[i] = in_Plane[i];
}

template <class T>
void AWT::PlanarCut<T>::getPlane(T* out_Plane) const
{
   for (int i = 0; i < 4; ++i)
      out_Plane[i] = m_D->m_Plane[i];
}

template <class T>
bool AWT::PlanarCut<T>::accept(T* vtx)
{
   return (dot<T>(m_D->m_Plane, vtx, 3) + m_D->m_Plane[3]) >= 0;
}

template class AWT::PlanarCut<double>;
template class AWT::PlanarCut<float>;