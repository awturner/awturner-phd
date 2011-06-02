#include "ControlPoint.h"

#include "Maths/BSpline.h"

#include "Useful/PrintMacros.h"

template <class T>
struct AWT::ControlPoint<T>::D
{
   T    m_Position[3];
   T    m_Value[3];
   T    m_Scale;
   bool m_Used;

   static const BSpline<T>* ms_Bspline;
};

template <class T>
const AWT::BSpline<T>* AWT::ControlPoint<T>::D::ms_Bspline = new AWT::BSpline<T>( 256 );

template <class T>
AWT::ControlPoint<T>::ControlPoint( const T* position, const T scale )
{
   m_D = new D;

   for ( int i = 0; i < 3; ++i )
   {
      m_D->m_Position[i] = position[i];
      m_D->m_Value[i] = 0;
      m_D->m_Used = false;
   }

   m_D->m_Scale = scale;
}


template <class T>
AWT::ControlPoint<T>::ControlPoint( const typename ControlPoint<T>::P _other )
{
   m_D = new D;

   _other->getPosition( m_D->m_Position );
   m_D->m_Scale = _other->getScale( );
}



template <class T>
AWT::ControlPoint<T>::~ControlPoint( )
{
   delete m_D;
}

template <class T>
typename AWT::ControlPoint<T>::P AWT::ControlPoint<T>::getInstance( const T* pos, const T scale )
{
   AWT::ControlPoint<T>* inst = new AWT::ControlPoint<T>( pos, scale );
   inst->countMe( );

   AWT::ControlPoint<T>::P ret( inst );
   inst->release( );

   return ret;
}


template <class T>
typename AWT::ControlPoint<T>::P AWT::ControlPoint<T>::getInstance( const typename ControlPoint<T>::P _other )
{
   AWT::ControlPoint<T>* inst = new AWT::ControlPoint<T>( _other );
   inst->countMe( );

   AWT::ControlPoint<T>::P ret( inst );
   inst->release( );

   return ret;
}

template <class T>
std::string AWT::ControlPoint<T>::getClassName() const
{
   return "AWT::ControlPoint<T>";
}

template <class T>
void AWT::ControlPoint<T>::getPosition( T* out_Position ) const
{
   for ( int i = 0; i < 3; ++i )
      out_Position[i] = m_D->m_Position[i];
}

template <class T>
T AWT::ControlPoint<T>::getPosition( unsigned char axis ) const
{
   return m_D->m_Position[axis];
}

template <class T>
void AWT::ControlPoint<T>::getValue( T* out_Position ) const
{
   for ( int i = 0; i < 3; ++i )
      out_Position[i] = m_D->m_Value[i];
}

template <class T>
T AWT::ControlPoint<T>::getValue( unsigned char axis ) const
{
   return m_D->m_Value[axis];
}

template <class T>
void AWT::ControlPoint<T>::setValue( const T* in_Value )
{
   for ( int i = 0; i < 3; ++i )
      m_D->m_Value[i] = in_Value[i];

   modified( );
}

template <class T>
void AWT::ControlPoint<T>::setValue( const unsigned char axis, const T v )
{
   if ( m_D->m_Value[axis] != v )
   {
      m_D->m_Value[axis] = v;
      modified( );
   }
}

template <class T>
T AWT::ControlPoint<T>::getScale( ) const
{
   return m_D->m_Scale;
}

template <class T>
T AWT::ControlPoint<T>::getInfluence( const T* point, bool debug = false ) const
{
   T ret = 1;

   for ( int i = 0; i < 3; ++i )
   {
      T tmp = ( point[i] - m_D->m_Position[i] ) / m_D->m_Scale;

      ret *= m_D->ms_Bspline->getValue( tmp );

      if ( debug )
      {
         PRINTVBL( tmp );
         PRINTVBL( m_D->ms_Bspline->getValue( tmp ) );
         PRINTVBL( ret );
      }
   }

   return ret;
}

template <class T>
bool AWT::ControlPoint<T>::isUsed( ) const
{
   return m_D->m_Used;
}

template <class T>
void AWT::ControlPoint<T>::setUsed( const bool u )
{
   m_D->m_Used = u;
}

template class AWT::ControlPoint<double>;
template class AWT::ControlPoint<float>;