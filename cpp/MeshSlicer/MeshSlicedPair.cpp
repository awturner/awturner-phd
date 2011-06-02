#include "MeshSlicedPair.h"

#include "Mesh/Mesh.h"


template <class T>
struct AWT::MeshSlicedPair<T>::D
{
   typename Mesh<T>::P m_Included;
   typename Mesh<T>::P m_Excluded;
};

template <class T>
AWT::MeshSlicedPair<T>::MeshSlicedPair( typename AWT::Mesh<T>::P included, typename AWT::Mesh<T>::P excluded )
{
   m_D = new D;

   m_D->m_Included = included;
   m_D->m_Excluded = excluded;
}

template <class T>
AWT::MeshSlicedPair<T>::~MeshSlicedPair( )
{
   delete m_D;
}

template <class T>
typename AWT::MeshSlicedPair<T>::P AWT::MeshSlicedPair<T>::getInstance( typename AWT::Mesh<T>::P included, typename AWT::Mesh<T>::P excluded )
{
   AUTOGETINSTANCE( AWT::MeshSlicedPair<T>, ( included, excluded ) );
}

template <class T>
GETNAMEMACRO( AWT::MeshSlicedPair<T> );

template <class T>
typename AWT::Mesh<T>::P AWT::MeshSlicedPair<T>::getIncluded( )
{
   return m_D->m_Included;
}

template <class T>
typename AWT::Mesh<T>::P AWT::MeshSlicedPair<T>::getExcluded( )
{
   return m_D->m_Excluded;
}

template class AWT::MeshSlicedPair<double>;
template class AWT::MeshSlicedPair<float>;