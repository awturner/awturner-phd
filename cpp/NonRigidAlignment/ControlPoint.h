#ifndef __CONTROLPOINT_H__
#define __CONTROLPOINT_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   template <class T>
   class ControlPoint : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<ControlPoint<T>> P;

   protected:
      ControlPoint( const T* position, const T scale );

      ControlPoint( const typename ControlPoint<T>::P _other );

      virtual ~ControlPoint( );

   public:
      static typename ControlPoint<T>::P getInstance( const T* pos, const T scale );

      static typename ControlPoint<T>::P getInstance( const typename ControlPoint<T>::P _other );

      virtual void getPosition( T* out_Position ) const;

      virtual T getPosition( unsigned char axis ) const;

      virtual void getValue( T* out_Position ) const;

      virtual T getValue( unsigned char axis ) const;

      virtual void setValue( const T* in_Value );

      virtual void setValue( const unsigned char axis, const T v );

      virtual T getScale( ) const;

      virtual T getInfluence( const T* point, bool debug = false ) const;

      virtual std::string getClassName( ) const;

      virtual void setUsed( const bool used );

      virtual bool isUsed( ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __CONTROLPOINT_H__