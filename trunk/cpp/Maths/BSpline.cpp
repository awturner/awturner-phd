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
#include "BSpline.h"

#include <cmath>
#include <limits>
#include "Useful/PrintMacros.h"


template <class T>
T AWT::BSpline<T>::evaluate(T u)
{
   int segment = static_cast<int>(floor(u + 2));

   u -= segment - 2;

   switch (segment)
	{
   case 3:
		return (1 + (-3 + 3*u - u*u)*u) / 6;
	case 2:
		return ((3*u - 6)*u*u + 4) / 6;
	case 1:
		return (((-3*u + 3)*u + 3)*u + 1) / 6;
	case 0:
		return u*u*u / 6;
	}
	
	return 0;
}

template <class T>
AWT::BSpline<T>::BSpline(const int nm_Points)
{
   m_Points = new T[nm_Points];
   m_NPoints = nm_Points;

   T u;
   for (int i = 0; i < nm_Points; ++i)
   {
      // Map i \in [0..nm_Points) to u \in [-2..2)
      u = 4 * static_cast<T>(i) / static_cast<T>(nm_Points) - 2;

      m_Points[i] = evaluate(u);
   }
}

template <class T>
AWT::BSpline<T>::~BSpline()
{
   delete[] m_Points;
}

template <class T>
T AWT::BSpline<T>::getValue(T u) const
{
   // Map u \in [-2..2) to i \in [0..m_NPoints)
   T   uu = (u + 2) * m_NPoints / 4;

   // DEBUG CHECK ME
   if (uu < -1 || uu >= m_NPoints)
      return 0;

   // Performs the linear interpolation of the spline function
   int i   = static_cast<int>(floor(uu));
   T alpha = uu - i;

   if (i == -1)
   {
      return alpha * m_Points[i+1];
   }
   else if (i == m_NPoints-1)
   {
      return (1 - alpha)*m_Points[i];
   }
   else
   {
      return (1 - alpha)*m_Points[i] + alpha*m_Points[i+1];
   }
}

template class AWT::BSpline<double>;
template class AWT::BSpline<float>;