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
#if 0
#ifndef __SIMPLESAMPLEDSURFACE_H__
#define __SIMPLESAMPLEDSURFACE_H__

#include "SampledSurface.h"
#include "Mesh/Tuples.h"

namespace AWT
{
   namespace AlignParametric
   {
      template <unsigned char Dims>
      class SimpleSampledSurface : public SampledSurface<Dims>
      {
      public:
         typedef ManagedAutoPointer<SimpleSampledSurface> P;

      protected:
         SimpleSampledSurface( TuplesType::P points );
         virtual ~SimpleSampledSurface( );

      public:
         static P getInstance( TuplesType::P points );
         virtual std::string getClassName( ) const;

         // Get the number of parameter components describing each sample
         virtual Idx getParameterDimensionality( ) const;

         // Get the number of samples on the surface
         virtual Idx getNumberOfSamples( ) const;

         // Get the current sample locations
         virtual void getSamples( vnl_matrix<T>& samples ) const;

         // Get the number of parameters which control this sampling
         virtual Idx getNumberOfParameters( ) const;

         // Get the current set of control values
         virtual void getParameters( vnl_matrix<T>& controls ) const;

         // Update the current set of control values
         virtual void setParameters( vnl_matrix<T>& controls );

         // Calculates the jacobian matrix of the l-th sample location w.r.t. the p-th parameters
         virtual void jacobian( const Idx l, const Idx p, vnl_matrix<T>& matrix ) const;

         virtual void refine( ) {}

         // Iterator functions - allows you to skip all the zero jacobians
         // Takes the internal iterator back to the start
         virtual void resetIterator( );

         // Advances to the next non-zero jacobian pair
         virtual bool next( Idx& l, Idx& p );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __SIMPLESAMPLEDSURFACE_H__
#endif