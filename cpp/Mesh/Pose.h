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
#ifndef __MESHPOSE_H__
#define __MESHPOSE_H__

#include "ObjectManagement/ManagedObject.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_matrix.h"

namespace AWT
{
   template <class T>
   class Pose : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<Pose<T>> P;

      typedef vnl_matrix_fixed<T,4,4> PoseMatrix;
      typedef vnl_vector_fixed<T,4>   PoseVector;

   protected:
      Pose( );
      virtual ~Pose( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual PoseMatrix getMatrix( ) const;

      virtual void setMatrix( const PoseMatrix& matrix );

      virtual void project( const T* in_Point, T* out_Point, const T implicit = 1 ) const;

      virtual P getRelativePose( const P pose ) const;

      virtual P getInversePose( ) const;

      virtual void identity( );

      virtual void translate( const T* trans );

      virtual void rotateDeg( const T theta, const T* axis );

      virtual void rotateDeg( const T theta, const T* axis, const T* centre );

      virtual void rotate( const T theta, const T* axis );

      virtual void rotate( const T theta, const T* axis, const T* centre );

      virtual void scale( const T sc );

   protected:
      struct D;
      D* m_D;

   };
}

template <class T>
vnl_matrix<T> operator*( const AWT::Pose<T>& pose, const vnl_matrix<T>& points );

#endif // __MESHPOSE_H__