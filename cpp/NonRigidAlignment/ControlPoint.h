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