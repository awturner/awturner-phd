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