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
#ifndef __LMICP_H__
#define __LMICP_H__

#include "ObjectManagement/ManagedObject.h"

#include "vnl/vnl_least_squares_function.h"

#include "Mesh/Tuples.h"
#include "vnl/vnl_matrix_fixed.h"

namespace AWT
{
   class LMICP : public ManagedObject, public vnl_least_squares_function
   {
   public:
      typedef ManagedAutoPointer<LMICP> P;

   protected:
      LMICP(Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)(double));
      virtual ~LMICP();

   public:
      static vnl_matrix_fixed<double,4,4> makeTransformation(const vnl_vector<double>& params, const double centroid[3]);
   protected:
      struct D;
      D* m_D;

   };

   class LMICP_PointPoint : public LMICP
   {
   public:
      typedef ManagedAutoPointer<LMICP> P;

   protected:
      LMICP_PointPoint(Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)(double));
      virtual ~LMICP_PointPoint();

   public:
      static P getInstance(Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)(double));
      std::string getClassName() const;

      virtual void f(const vnl_vector<double>& x, vnl_vector<double>& residuals);
   };

   class LMICP_PointPlane : public LMICP
   {
   public:
      typedef ManagedAutoPointer<LMICP> P;

   protected:
      LMICP_PointPlane(Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)(double));
      virtual ~LMICP_PointPlane();

   public:
      static P getInstance(Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)(double));
      std::string getClassName() const;

      virtual void f(const vnl_vector<double>& x, vnl_vector<double>& residuals);
   };
}

#endif // __LMICP_H__