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
#ifndef __MESHAPPROXGEODESIC_H__
#define __MESHAPPROXGEODESIC_H__

#include "ObjectManagement/ManagedObject.h"
#include "Mesh/Mesh.h"

namespace AWT
{
   template <class T>
   class MeshApproxGeodesic : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshApproxGeodesic<T>> P;

   protected:
      MeshApproxGeodesic( typename Mesh<T>::P mesh );
      virtual ~MeshApproxGeodesic( );

   public:
      static P getInstance( typename Mesh<T>::P mesh );
      virtual std::string getClassName( ) const;

      void setOrigin( const T vtxStart[3], const T distMax );
      void setOrigin( const T vtxStart[3], const MeshIndex cell, const T distMax );

      MeshIndex getNumberOfFoundCells( ) const;
      MeshIndex getFoundCell( const MeshIndex i ) const;

      T distanceTo( const T vtxEnd[3] ) const;
      T distanceTo( const T vtxEnd[3], const MeshIndex cell ) const;

      void directionTo( const T vtxEnd[3], T dir[3], const bool debug = false ) const;
      void directionTo( const T vtxEnd[3], const MeshIndex cell, T dir[3], const bool debug = false ) const;

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHAPPROXGEODESIC_H__