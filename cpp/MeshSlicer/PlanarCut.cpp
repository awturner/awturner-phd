#include "PlanarCut.h"

#include "Useful/ArrayVectorFunctions.h"

template <class T>
struct AWT::PlanarCut<T>::D
{
   T m_Plane[4];
};

template <class T>
AWT::PlanarCut<T>::PlanarCut( )
{
   m_D = new D;

   m_D->m_Plane[0] = 0;
   m_D->m_Plane[1] = 0;
   m_D->m_Plane[2] = 1;
   m_D->m_Plane[3] = 0;
}

template <class T>
AWT::PlanarCut<T>::~PlanarCut( )
{
   delete m_D;
}

template <class T>
typename AWT::PlanarCut<T>::P AWT::PlanarCut<T>::getInstance( )
{
   AUTOGETINSTANCE( AWT::PlanarCut<T>, ( ) );
}

template <class T>
GETNAMEMACRO( AWT::PlanarCut<T> );

template <class T>
void AWT::PlanarCut<T>::setPlane( const T* in_Plane )
{
   for ( int i = 0; i < 4; ++i )
      m_D->m_Plane[i] = in_Plane[i];
}

template <class T>
void AWT::PlanarCut<T>::getPlane( T* out_Plane ) const
{
   for ( int i = 0; i < 4; ++i )
      out_Plane[i] = m_D->m_Plane[i];
}

template <class T>
bool AWT::PlanarCut<T>::accept( T* vtx )
{
   return ( dot<T>( m_D->m_Plane, vtx, 3 ) + m_D->m_Plane[3] ) >= 0;
}

template class AWT::PlanarCut<double>;
template class AWT::PlanarCut<float>;