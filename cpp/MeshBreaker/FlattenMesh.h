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
#ifndef __FLATTENMESH_H__
#define __FLATTENMESH_H__

#include "ObjectManagement/ManagedObject.h"

#include "FlattenMeshPair.h"
#include "GeneratorProvider.h"

namespace AWT
{
   template <class T>
   class FlattenMesh : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<FlattenMesh> P;

      enum WeightType
      {
         WEIGHT_TUTTE,
         WEIGHT_FLOATER,
         WEIGHT_HARMONIC,
         WEIGHT_WACHSPRESS,
      };

      enum SpreadType
      {
         SPREAD_NONE,
         SPREAD_AWT,
         SPREAD_YOSHIZAWA,
      };

      static WeightType getWeightType( const char* str );
      static SpreadType getSpreadType( const char* str );

   protected:
      FlattenMesh( );
      virtual ~FlattenMesh( );

   public:
      static typename FlattenMeshPair<T>::P flattenToPair( typename GeneratorProvider<T>::P fg, const WeightType wt, const SpreadType st );
      static typename Mesh<T>::P flatten( typename GeneratorProvider<T>::P fg, const WeightType wt, const SpreadType st );

   protected:
      struct D;
   };
}

#endif // __FLATTENMESH_H__