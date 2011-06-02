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
#ifndef __DEFORMABLEMESH_H__
#define __DEFORMABLEMESH_H__

#include "Mesh/Mesh.h"
#include "vnl/vnl_matrix.h"

namespace AWT
{
   /*! \class DeformableMesh
   \brief Deformable Mesh, i.e. a shape model

   Abstract representation of a shape model.
   */

   template <class T>
   class DeformableMesh : public Mesh<T>
   {
   public:
      typedef ManagedAutoPointer< DeformableMesh<T> > P;

   public:
      virtual MeshIndex getNumberOfActiveModes( ) const = 0;

      virtual void setActiveModes( const MeshIndex m ) = 0;
      virtual void activateAllModes( ) = 0;

      virtual void setModeWeight( const MeshIndex m, const T weight ) = 0;

      virtual T getModeWeight( const MeshIndex m ) const = 0;

      virtual void setModeWeights( const T* weights ) = 0;

      virtual void getModeWeights( T* weights ) const = 0;

      virtual typename Tuples<T>::P getMode( const MeshIndex m ) = 0;

      virtual void getJacobian( const MeshIndex v, T* pnt, vnl_matrix<T>& jacobian ) const = 0;
   };
}

#endif // __DEFORMABLEMESH_H__