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
#ifndef __GRADIENTACCUMULATOR_H__
#define __GRADIENTACCUMULATOR_H__

#include "ControlPointSearcherCallback.h"

namespace AWT
{
   template <class T>
   class NonRigidAlignment;

   template <class T>
   class Tuples;

   template <class T>
   class GradientAccumulator : public ControlPointSearcherCallback<T>
   {
   public:
      typedef ManagedAutoPointer<GradientAccumulator<T>> P;

   protected:
      GradientAccumulator( typename NonRigidAlignment<T>::P data );

      virtual ~GradientAccumulator( );

   public:
      static P getInstance( typename NonRigidAlignment<T>::P nra );

      virtual void controlPointFound( const MeshIndex controlIndex );

      Tuples<T>* getGradient( ) const;

      void getMovingPoint( T* out_Point ) const;

      void getFixedPoint( T* out_Point ) const;

      void setMovingPoint( const T* in_Point );

      void setFixedPoint( const T* in_Point );

      MeshIndex getFaceIndex( ) const;

      void setFaceIndex( const MeshIndex idx );

      virtual std::string getClassName( ) const;

   protected:
      struct D;
      D* m_D;
   };
}  

#endif // __GRADIENTACCUMULATOR_H__