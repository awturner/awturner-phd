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
#ifndef __DEFORMABLEMESHFITTINGLISTENER_H__
#define __DEFORMABLEMESHFITTINGLISTENER_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   template <class T>
   class DeformableMeshFitting;

   template <class T>
   class DeformableMeshFittingListener : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer< DeformableMeshFittingListener<T> > P;

   public:
      // Fired when the set of correspondences between the fixed and moving meshes is found
      virtual void correspondencesFound(typename DeformableMeshFitting<T>::P nra) = 0;
   };
}

//#include "ObjectManagement/ManagedObjectSet.h"
//namespace AWT
//{
//   template <class T>
//   class CompositeDeformableMeshFittingListener : public CompositeListener<DeformableMeshFittingListener<T>,DeformableMeshFitting<T> >
//   {
//   public:
//      typedef ManagedAutoPointer< CompositeDeformableMeshFittingListener<T> > P;
//
//   public:
//      virtual std::string getClassName() const { return "AWT::CompositeDeformableMeshFittingListener<T>"; }
//   };
//}

#endif // __DEFORMABLEMESHFITTINGLISTENER_H__
