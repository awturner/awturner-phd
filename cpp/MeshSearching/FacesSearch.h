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
#ifndef __SURFACEMESH_FACESSEARCHER_H__
#define __SURFACEMESH_FACESSEARCHER_H__

#include "MeshSearch.h"
#include "FacesTreeData.h"

namespace AWT
{
   template <class T>
   class FacesSearch : public MeshSearch<T>
   {
   public:
      void setData(typename OEKDTreeData<T,3>::P in_Data);
      typename typename FacesTreeData<T>::P getData();

   protected:
      typename FacesTreeData<T>::P m_Data;

   };
   
   template <class T>
   void FacesSearch<T>::setData(typename OEKDTree::OEKDTreeData<T,3>::P data)
   {
      m_Data = checkType<OEKDTree::OEKDTreeData<T,3>,FacesTreeData<T> >(data);
   }

   template <class T>
   typename FacesTreeData<T>::P FacesSearch<T>::getData()
   {
      return m_Data;
   }

}

#endif // __SURFACEMESH_FACESSEARCHER_H__