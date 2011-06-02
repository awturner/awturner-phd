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
      typename ManagedAutoPointer<ManagedArray<T> > P;

   protected:
      ManagedArray(const ManagedArrayIndex len)
      {
         m_Data = new T[ m_Length = len ];
      }

      ~ManagedArray()
      {
         delete [] m_Data;
      }

   public:
      static P getInstance(const ManagedArrayIndex len)
      {
         AUTOGETINSTANCE(AWT::ManagedArray<T>, (len));
      }

      std::string getClassName() const
      {
         return "AWT::ManagedArray<T>";
      }

      void reallocate(const ManagedArrayIndex len)
      {
         if (len != m_Length)
         {
            delete[] m_Data;

            m_Data = new T[ m_Length = len ];
         }
      }

      T* operator*()
      {
         return m_Data;
      }

      T& operator[](const ManagedArrayIndex i)
      {
         return m_Data[i];
      }

      ManagedArrayIndex getLength() const
      {
         return m_Length;
      }

   protected:
      T* m_Data;
      ManagedArrayIndex m_Length;
   };
}

#endif // __MANAGEDARRAY_H__