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
#ifndef __MESHBREAKER_H__
#define __MESHBREAKER_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Mesh.h"
#include "Mesh/MeshConnectivity.h"

namespace AWT
{
   template <class T>
   class MeshBreaker : public ManagedObject
   {
   public:
      enum Symbol { SYM_UNDEF, SYM_C, SYM_L, SYM_R, SYM_E, SYM_S };

      typedef ManagedAutoPointer<MeshBreaker<T>> P;

   protected:
      MeshBreaker( typename Mesh<T>::P mesh );
      virtual ~MeshBreaker( );

   public:
      static P getInstance( typename Mesh<T>::P mesh );
      virtual std::string getClassName( ) const;

      virtual unsigned int getNumberOfBranches( ) const;
      virtual typename Tuples<T>::P getBranch( unsigned int i );

      virtual typename Mesh<T>::P getMesh( );
      virtual typename MeshConnectivity<T>::P getConnectivity( );

      virtual typename Mesh<T>::P getRegion( const unsigned int i );

      virtual unsigned int getNumberOfContours( ) const;
      virtual typename Tuples<T>::P getContour( const unsigned int i );

      virtual MeshIndex getHistoryLength( ) const;
      virtual void printHistoryItem( MeshIndex h ) const;

      virtual Symbol    getHistorySymbol( AWT::MeshIndex h ) const;
      virtual MeshIndex getHistoryFace( MeshIndex h ) const;
      virtual MeshIndex getHistoryLeftNeighbour( MeshIndex h ) const;
      virtual MeshIndex getHistoryRightNeighbour( MeshIndex h ) const;

      virtual MeshIndex getNumberOfDebugFaces( ) const;
      virtual MeshIndex getDebugFace( const MeshIndex i ) const;

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHBREAKER_H__