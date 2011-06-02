#include "OrCuttingFunction.h"



template <class T>
struct AWT::OrCuttingFunction<T>::D
{
   CuttingFunction<T>::P m_FuncA;
   CuttingFunction<T>::P m_FuncB;
};

template <class T>
AWT::OrCuttingFunction<T>::OrCuttingFunction( typename AWT::CuttingFunction<T>::P funcA, typename AWT::CuttingFunction<T>::P funcB )
{
   m_D = new D;

   m_D->m_FuncA = funcA;
   m_D->m_FuncB = funcB;
}

template <class T>
AWT::OrCuttingFunction<T>::~OrCuttingFunction( )
{
   delete m_D;
}

template <class T>
typename AWT::OrCuttingFunction<T>::P AWT::OrCuttingFunction<T>::getInstance( typename AWT::CuttingFunction<T>::P funcA, typename AWT::CuttingFunction<T>::P funcB )
{
   AUTOGETINSTANCE( AWT::OrCuttingFunction<T>, ( funcA, funcB ) );
}

template <class T>
GETNAMEMACRO( AWT::OrCuttingFunction<T> );

template <class T>
bool AWT::OrCuttingFunction<T>::accept( T* vtx )
{
   return m_D->m_FuncA->accept( vtx ) || m_D->m_FuncB->accept( vtx );
}

template class AWT::OrCuttingFunction<double>;
template class AWT::OrCuttingFunction<float>;