#ifndef __MANAGEDARRAY_H__
#define __MANAGEDARRAY_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   typedef unsigned long ManagedArrayIndex;

   template <class T>
   class ManagedArray : public ManagedObject
   {
   public:
      typename ManagedAutoPointer<ManagedArray<T>> P;

   protected:
      ManagedArray( const ManagedArrayIndex len )
      {
         m_Data = new T[ m_Length = len ];
      }

      ~ManagedArray( )
      {
         delete [] m_Data;
      }

   public:
      static P getInstance( const ManagedArrayIndex len )
      {
         AUTOGETINSTANCE( AWT::ManagedArray<T>, ( len ) );
      }

      std::string getClassName( ) const
      {
         return "AWT::ManagedArray<T>";
      }

      void reallocate( const ManagedArrayIndex len )
      {
         if ( len != m_Length )
         {
            delete[] m_Data;

            m_Data = new T[ m_Length = len ];
         }
      }

      T* operator*( )
      {
         return m_Data;
      }

      T& operator[]( const ManagedArrayIndex i )
      {
         return m_Data[i];
      }

      ManagedArrayIndex getLength( ) const
      {
         return m_Length;
      }

   protected:
      T* m_Data;
      ManagedArrayIndex m_Length;
   };
}

#endif // __MANAGEDARRAY_H__