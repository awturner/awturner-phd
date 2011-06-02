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