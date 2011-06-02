#include "AndCuttingFunction.h"



template <class T>
struct AWT::AndCuttingFunction<T>::D
{
   typename CuttingFunction<T>::P m_FuncA;
   typename CuttingFunction<T>::P m_FuncB;
};

template <class T>
AWT::AndCuttingFunction<T>::AndCuttingFunction( typename AWT::CuttingFunction<T>::P funcA, typename AWT::CuttingFunction<T>::P funcB )
{
   m_D = new D;

   m_D->m_FuncA = funcA;
   m_D->m_FuncB = funcB;
}

template <class T>
AWT::AndCuttingFunction<T>::~AndCuttingFunction( )
{
   delete m_D;
}

template <class T>
typename AWT::AndCuttingFunction<T>::P AWT::AndCuttingFunction<T>::getInstance( typename AWT::CuttingFunction<T>::P funcA, typename AWT::CuttingFunction<T>::P funcB )
{
   AUTOGETINSTANCE( AWT::AndCuttingFunction<T>, ( funcA, funcB ) );
}

template <class T>
GETNAMEMACRO( AWT::AndCuttingFunction<T> );

template <class T>
bool AWT::AndCuttingFunction<T>::accept( T* vtx )
{
   return m_D->m_FuncA->accept( vtx ) && m_D->m_FuncB->accept( vtx );
}

template class AWT::AndCuttingFunction<double>;
template class AWT::AndCuttingFunction<float>;