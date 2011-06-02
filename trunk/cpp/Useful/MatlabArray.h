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
#ifndef __MATLABARRAY_H__
#define __MATLABARRAY_H__

#include "ObjectManagement/ManagedObject.h"
#include "engine.h"
#include "vnl/vnl_matrix.h"

namespace AWT
{
   class MatlabEngine;

   class MatlabArray : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MatlabArray> P;

   protected:
      MatlabArray(MatlabEngine* eng, const char* name);
      MatlabArray(const vnl_matrix<double>& matrix);

      virtual ~MatlabArray();

   public:
      // Retrieves the variable with the given name from the engine's workspace
      static P getInstance(MatlabEngine* eng, const char* name);

      // Creates a new array and populate with the data in this matrix
      static P getInstance(const vnl_matrix<double>& matrix);

      virtual std::string getClassName() const;

      const unsigned int getNumberOfDimensions();
      void getDimensions(unsigned int* dims);

      const char* getVariableName();
      const char* getTypeName();
      
      template <typename T>
      T* getData();

      // Convenience method to copy variable back into matrix
      void getMatrix(vnl_matrix<double>& matrix);

      mxArray* getArray();

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MATLABARRAY_H__