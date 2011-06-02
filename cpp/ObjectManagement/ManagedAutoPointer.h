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
#ifndef __MANAGEDAUTOPOINTER_H__
#define __MANAGEDAUTOPOINTER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Useful/PrintMacros.h"

#define AUTOGETINSTANCE( Class, Args ) \
   { \
      Class* inst = new Class##Args; \
      inst->countMe( ); \
      Class::P ret( inst ); \
      inst->release( ); \
      return ret; \
   }

namespace AWT
{
   template <class T>
   class ManagedAutoPointer
   {
   public:
      ManagedAutoPointer( )
      {
         m_Data = 0;
      }

      ManagedAutoPointer( T* val )
      {
         m_Data = 0;
         set( val );
      }

      virtual ~ManagedAutoPointer( )
      {
         if ( m_Data != 0 )
         {
            if ( m_Data->getReferenceCount( ) == 0 )
            {
               DEBUGMACRO( "Destructor" );
               
               PRINTVBL( m_Data->getReferenceCount( ) );
               PRINTVBL( m_Data );
            }
            else
            {
               m_Data->release( );
            }
         }
      }

      ManagedAutoPointer( const ManagedAutoPointer<T>& ptr )
      {
         m_Data = 0;
         set( ptr.getDataConst( ) );
      }

      template <class Other>
      ManagedAutoPointer( const ManagedAutoPointer<Other>& ptr )
      {
         m_Data = 0;
         set( ptr.getDataConst( ) );
      }
      
   public:
      T* getData( )
      {
         return m_Data;
      }

      T* getDataConst( ) const
      {
         return m_Data;
      }

      T*& operator*( )
      {
         return m_Data;
      }

      T* operator->( )
      {
         return m_Data;
      }

      const T* operator->( ) const
      {
         return m_Data;
      }

      void nullify( )
      {
         if ( m_Data != 0 )
         {
            m_Data->release( );
            m_Data = 0;
         }
      }

      void set( T* val )
      {
         AWT::ReferenceCountedObject::swapRefCountedObjects( this, m_Data, val );
      }

      void operator=( const ManagedAutoPointer& oth )
      {
         set( oth.m_Data );
      }

      template <class OtherType>
      const bool operator==( const ManagedAutoPointer<OtherType>& oth ) const
      {
         return (void*)m_Data == (void*)oth.getDataConst( );
      }

      template <class OtherType>
      const bool operator!=( const ManagedAutoPointer<OtherType>& oth ) const
      {
         return (void*)m_Data != (void*)oth.getDataConst( );
      }

      template <class OtherType>
      const bool operator<( const ManagedAutoPointer<OtherType>& oth ) const
      {
         return (void*)m_Data < (void*)oth.getDataConst( );
      }

   protected:
      T* m_Data;
   };
}

#endif // __MANAGEDAUTOPOINTER_H__