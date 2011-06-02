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
#ifndef __SHAPEIMAGE_H__
#define __SHAPEIMAGE_H__

#include "ObjectManagement/ManagedObject.h"

#include "FlattenMeshPair.h"

namespace AWT
{
   template <class T>
   class ShapeImage : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<ShapeImage> P;

   protected:
      ShapeImage(typename FlattenMeshPair<T>::P flat, unsigned int ngrid);
      virtual ~ShapeImage();

   public:
      static P getInstance(typename FlattenMeshPair<T>::P flat, unsigned int ngrid);
      virtual std::string getClassName() const;

      virtual MeshIndex mapMeshToFlattened(const T* meshCoords, T* flatCoords);
      virtual MeshIndex mapFlattenedToMesh(const T* flatCoords, T* meshCoords);

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __SHAPEIMAGE_H__