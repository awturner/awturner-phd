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
#include "NotCuttingFunction.h"



template <class T>
struct AWT::NotCuttingFunction<T>::D
{
   typename CuttingFunction<T>::P m_Func;
};

template <class T>
AWT::NotCuttingFunction<T>::NotCuttingFunction( typename CuttingFunction<T>::P func )
{
   m_D = new D;

   m_D->m_Func = func;
}

template <class T>
AWT::NotCuttingFunction<T>::~NotCuttingFunction( )
{
   delete m_D;
}

template <class T>
typename AWT::NotCuttingFunction<T>::P AWT::NotCuttingFunction<T>::getInstance( typename CuttingFunction<T>::P func )
{
   AUTOGETINSTANCE( AWT::NotCuttingFunction<T>, ( func ) );
}

template <class T>
GETNAMEMACRO( AWT::NotCuttingFunction<T> );

template <class T>
bool AWT::NotCuttingFunction<T>::accept( T* vtx )
{
   return !m_D->m_Func->accept( vtx );
}

template class AWT::NotCuttingFunction<double>;
template class AWT::NotCuttingFunction<float>;