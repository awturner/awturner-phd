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
#ifndef __SAMPLEDMESHDATA_H__
#define __SAMPLEDMESHDATA_H__

#include "MeshData.h"
#include "SamplePoints.h"
#include "Tuples.h"

namespace AWT
{
   template <class T>
   class SampledMeshData : public MeshData<T>
   {
   public:
      typedef ManagedAutoPointer<SampledMeshData<T>> P;

   protected:
      SampledMeshData( typename MeshData<T>::P data, SamplePoints::P points );
      virtual ~SampledMeshData( );

   public:
      static typename SampledMeshData<T>::P getInstance( typename MeshData<T>::P data, SamplePoints::P points );
      virtual std::string getClassName( ) const;

      virtual void addVertexData( const std::string& label, typename Tuples<T>::P data );
      virtual void addFaceData( const std::string& label, typename Tuples<T>::P data );

      virtual bool hasVertexData( const std::string& label ) const;
      virtual bool hasFaceData( const std::string& label ) const;

      virtual typename Tuples<T>::P getVertexData( const std::string& label );
      virtual typename Tuples<T>::P getFaceData( const std::string& label );

      virtual void removeVertexData( const std::string& label );
      virtual void removeFaceData( const std::string& label );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __SAMPLEDMESHDATA_H__