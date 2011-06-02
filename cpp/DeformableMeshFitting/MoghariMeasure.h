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
#ifndef __MOGHARIMEASURE_H__
#define __MOGHARIMEASURE_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/MeshTypeDefs.h"
#include "Mesh/Tuples.h"
#include "Mesh/Mesh.h"

#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"

namespace AWT
{
   template <class T>
   class MoghariMeasure : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MoghariMeasure> P;

   protected:
      MoghariMeasure( );
      virtual ~MoghariMeasure( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      struct LocalStatistics
      {
         LocalStatistics( )
         {
            covariance.fill( 0 );
            mean.fill( 0 );
            scale = 1;
            npts = 0;
         }

         vnl_matrix_fixed<T,3,3> eigenvectors;
         vnl_matrix_fixed<T,3,3> covariance;
         vnl_vector_fixed<T,3> mean;

         T         scale;

         MeshIndex npts;
      };

      vnl_matrix_fixed<T,3,3> calculateEigenvectors( const vnl_matrix<T>& m );

      void calculateLocalStatistics( typename Mesh<T>::P mesh, const MeshIndex v, const T radius, LocalStatistics* lstats );

      void calculateForVertex( typename Mesh<T>::P mesh, const MeshIndex v, const T radius, vnl_vector<T>& out_Matrix );
      
      typename Tuples<T>::P calculate( typename Mesh<T>::P mesh, const T radius );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __MOGHARIMEASURE_H__