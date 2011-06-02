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
#ifndef __MESHBREAKERHISTORY_H__
#define __MESHBREAKERHISTORY_H__

#include "ObjectManagement/ManagedObject.h"

#include "MeshBreaker.h"

namespace AWT
{
   template <class T>
   class MeshBreakerHistory : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshBreakerHistory> P;

   protected:
      MeshBreakerHistory(const typename MeshBreaker<T>::Symbol sym, const MeshIndex f, const MeshIndex g, const MeshIndex distVert, const MeshIndex lneigh, const MeshIndex rneigh );
      virtual ~MeshBreakerHistory();

   public:
      static P getInstance(const typename MeshBreaker<T>::Symbol sym, const MeshIndex f, const MeshIndex g, const MeshIndex distVert, const MeshIndex lneigh, const MeshIndex rneigh);
      virtual std::string getClassName() const;

      virtual typename MeshBreaker<T>::Symbol getSymbol() const;

      virtual MeshIndex getF() const;
      virtual MeshIndex getG() const;

      virtual MeshIndex getDistalVertex() const;

      virtual MeshIndex getLeftNeighbour() const;
      virtual MeshIndex getRightNeighbour() const;
         
   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MESHBREAKERHISTORY_H__