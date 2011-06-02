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
#ifndef __CatesRegularizer_H__
#define __CatesRegularizer_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"
#include "Useful/ValueRange.h"
#include "TypeDefs.h"

namespace AWT
{
   namespace AlignParametric
   {
      class CatesParticleSurface;

      class CatesRegularizer : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<CatesRegularizer> P;

         enum DistanceType { DT_EUCLIDEAN, DT_APPROXGEODESIC };

      protected:
         CatesRegularizer( CatesParticleSurface* surf, const ValueRange<T> sigmaRange );
         virtual ~CatesRegularizer( );

      public:
         static P getInstance( CatesParticleSurface* surf, const ValueRange<T> sigmaRange );
         virtual std::string getClassName( ) const;

         TuplesType::P getSigma( );

         DistanceType getDistanceType( ) const;
         void setDistanceType( const DistanceType dt );

         T getSigma( MeshIndex i ) const;
         void setSigma( MeshIndex i, const T v );

         // Calculate the force which tries to push these particles apart
         void calculateUpdate( MatrixType& reg );

         T calculateCost( const bool debug = true );
         T calculateNegativeEntropy( );

         T getMinSigma( ) const;

         const ValueRange<T>& getSigmaRange( ) const;
         void setSigmaRange( const ValueRange<T>& sigmaRange );

         void associateSampleToFace( const MeshIndex i, const MeshIndex f );

         void fixSigma(const bool state);

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __CatesRegularizer_H__