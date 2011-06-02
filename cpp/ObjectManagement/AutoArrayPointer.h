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
#ifndef __AUTOARRAYPOINTER_H__
#define __AUTOARRAYPOINTER_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   template <class DataType>
   class AutoArrayPointer
   {
   public:
      AutoArrayPointer()
      {
         m_Data = 0;
      }

      AutoArrayPointer(const unsigned long length)
      {
         m_Data = 0;

         allocate(length);
      }

      virtual ~AutoArrayPointer()
      {
         delete[] m_Data;
      }

      void reallocate(const unsigned long length)
      {
         if (m_Data != 0)
            delete[] m_Data;

         m_Data = new DataType[length];
      }

      DataType* operator*()
      {
         return m_Data;
      }

   protected:
      DataType* m_Data;

   };
}

#endif // __AUTOARRAYPOINTER_H__