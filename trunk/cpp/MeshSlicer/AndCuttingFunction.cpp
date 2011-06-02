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
#include "AndCuttingFunction.h"



template <class T>
struct AWT::AndCuttingFunction<T>::D
{
   typename CuttingFunction<T>::P m_FuncA;
   typename CuttingFunction<T>::P m_FuncB;
};

template <class T>
AWT::AndCuttingFunction<T>::AndCuttingFunction(typename AWT::CuttingFunction<T>::P funcA, typename AWT::CuttingFunction<T>::P funcB)
{
   m_D = new D;

   m_D->m_FuncA = funcA;
   m_D->m_FuncB = funcB;
}

template <class T>
AWT::AndCuttingFunction<T>::~AndCuttingFunction()
{
   delete m_D;
}

template <class T>
typename AWT::AndCuttingFunction<T>::P AWT::AndCuttingFunction<T>::getInstance(typename AWT::CuttingFunction<T>::P funcA, typename AWT::CuttingFunction<T>::P funcB)
{
   AUTOGETINSTANCE(AWT::AndCuttingFunction<T>, (funcA, funcB));
}

template <class T>
GETNAMEMACRO(AWT::AndCuttingFunction<T>);

template <class T>
bool AWT::AndCuttingFunction<T>::accept(T* vtx)
{
   return m_D->m_FuncA->accept(vtx) && m_D->m_FuncB->accept(vtx);
}

template class AWT::AndCuttingFunction<double>;
template class AWT::AndCuttingFunction<float>;