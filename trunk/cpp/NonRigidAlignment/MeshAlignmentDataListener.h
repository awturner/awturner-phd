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
#ifndef __MESHALIGNMENTDATALISTENER_H__
#define __MESHALIGNMENTDATALISTENER_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   template <class T>
   class MeshAlignmentData;

   template <class T>
   class MeshAlignmentDataListener : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshAlignmentDataListener<T> > P;

   public:
      virtual void fixedPointsSet(typename MeshAlignmentData<T>::P mad) = 0;
      virtual void movingPointsSet(typename MeshAlignmentData<T>::P mad) = 0;
   };
}

//#include "ObjectManagement/ManagedObjectSet.h"
//namespace AWT
//{
//   template <class T>
//   class CompositeMeshAlignmentDataListener : public CompositeListener<typename MeshAlignmentDataListener<T>,MeshAlignmentData<T> >
//   {
//      virtual std::string getClassName() const { return "AWT::CompositeMeshAlignmentDataListener<T>"; }
//   };
//}

#endif // __MESHALIGNMENTDATALISTENER_H__