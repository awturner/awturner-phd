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
#include "ColourMapper.h"

template <class T>
struct AWT::ColourMapper<T>::D
{
   typename Tuples<T>::P m_Data;

   T m_Min;
   T m_Max;
};

template <class T>
AWT::ColourMapper<T>::ColourMapper( )
{
   m_D = new D;

   m_D->m_Min = 0;
   m_D->m_Max = 1;
}

template <class T>
AWT::ColourMapper<T>::~ColourMapper( )
{
   delete m_D;
}

template <class T>
void AWT::ColourMapper<T>::setMin( T v )
{
   m_D->m_Min = v;
   modified();
}

template <class T>
T AWT::ColourMapper<T>::getMin( ) const
{
   return m_D->m_Min;
}

template <class T>
void AWT::ColourMapper<T>::setMax( T v )
{
   m_D->m_Max = v;
   modified();
}

template <class T>
T AWT::ColourMapper<T>::getMax( ) const
{
   return m_D->m_Max;
}

template <class T>
void AWT::ColourMapper<T>::setData( typename Tuples<T>::P data )
{
   if ( *m_D->m_Data != *data )
   {
      m_D->m_Data = data;
      modified( );
   }
}

template <class T>
typename AWT::Tuples<T>::P AWT::ColourMapper<T>::getData( )
{
   return m_D->m_Data;
}

template class AWT::ColourMapper<double>;
template class AWT::ColourMapper<float>;