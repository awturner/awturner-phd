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
#include "Useful/MatlabArray.h"

#include "Useful/MatlabEngine.h"

struct AWT::MatlabArray::D
{
   mxArray* array;
   char name[256];
};

AWT::MatlabArray::MatlabArray(MatlabEngine* eng, const char* name)
{
   m_D = new D;

   strcpy_s(m_D->name, name);

   m_D->array = engGetVariable(eng->getEngine(), name);
}

AWT::MatlabArray::MatlabArray(const vnl_matrix<double>& mat)
{
   m_D = new D;

   m_D->array = mxCreateDoubleMatrix(mat.cols(), mat.rows(), mxREAL);

   // Copy this matrix into the data
   double *ptr = (double*)mxGetData(m_D->array);
   mat.copy_out(ptr);
}

AWT::MatlabArray::~MatlabArray()
{
   mxDestroyArray(m_D->array);

   delete m_D;
}

AWT::MatlabArray::P AWT::MatlabArray::getInstance(MatlabEngine* eng, const char* name)
{
   AUTOGETINSTANCE(AWT::MatlabArray, (eng, name));
}

AWT::MatlabArray::P AWT::MatlabArray::getInstance(const vnl_matrix<double>& mat)
{
   AUTOGETINSTANCE(AWT::MatlabArray, (mat));
}

GETNAMEMACRO(AWT::MatlabArray);

const char* AWT::MatlabArray::getVariableName()
{
   return m_D->name;
}

const char* AWT::MatlabArray::getTypeName()
{
   return mxGetClassName(m_D->array);
}

template <typename T>
T* AWT::MatlabArray::getData()
{
   return static_cast<T*>(mxGetData(m_D->array));
}

mxArray* AWT::MatlabArray::getArray()
{
   return m_D->array;
}

const unsigned int AWT::MatlabArray::getNumberOfDimensions()
{
   return mxGetNumberOfDimensions(m_D->array);
}

void AWT::MatlabArray::getDimensions(unsigned int* dims)
{
   const mwSize* mdims = mxGetDimensions(m_D->array);

   for (unsigned int i = 0, imax = getNumberOfDimensions(); i < imax; ++i)
   {
      dims[i] = mdims[i];
   }
}

void AWT::MatlabArray::getMatrix(vnl_matrix<double>& matrix)
{
   const unsigned int ndims = getNumberOfDimensions();

   unsigned int* dims = new unsigned int[ndims];
   getDimensions(dims);

   PRINTVEC(dims, ndims);

   // TRANSPOSED!!
   matrix.set_size(dims[1], dims[0]);
   matrix.set(static_cast<double*>(mxGetData(m_D->array)));

   delete [] dims;
}