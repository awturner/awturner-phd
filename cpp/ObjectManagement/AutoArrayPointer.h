#ifndef __AUTOARRAYPOINTER_H__
#define __AUTOARRAYPOINTER_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   template <class DataType>
   class AutoArrayPointer
   {
   public:
      AutoArrayPointer( )
      {
         m_Data = 0;
      }

      AutoArrayPointer( const unsigned long length )
      {
         m_Data = 0;

         allocate( length );
      }

      virtual ~AutoArrayPointer( )
      {
         delete[] m_Data;
      }

      void reallocate( const unsigned long length )
      {
         if ( m_Data != 0 )
            delete[] m_Data;

         m_Data = new DataType[length];
      }

      DataType* operator*( )
      {
         return m_Data;
      }

   protected:
      DataType* m_Data;

   };
}

#endif // __AUTOARRAYPOINTER_H__