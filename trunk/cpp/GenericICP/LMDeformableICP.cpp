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
#include "LMDeformableICP.h"

#include "Mesh/TuplesFunctions.h"
#include "Mesh/MeshFunctions.h"

#include "Useful/PrintMacros.h"
#include "Useful/ArrayVectorFunctions.h"

struct AWT::LMMultiDeformableICP::D
{
   std::vector< LMDeformableICP::P > m_Fragments;

   double (*m_Func)(double);
};

using namespace AWT;

AWT::LMMultiDeformableICP::LMMultiDeformableICP(unsigned int nvars, unsigned int nresid, double (*func)(double))
: vnl_least_squares_function(nvars, nresid, vnl_least_squares_function::no_gradient)
{
   m_D = new D;

   m_D->m_Func = func;
}

AWT::LMMultiDeformableICP::~LMMultiDeformableICP()
{
   delete m_D;
}

AWT::LMMultiDeformableICP::P AWT::LMMultiDeformableICP::getInstance(unsigned int nvars, unsigned int nresid, double (*func)(double))
{
   AUTOGETINSTANCE(AWT::LMMultiDeformableICP, (nvars, nresid, func));
}

GETNAMEMACRO(AWT::LMMultiDeformableICP);

void AWT::LMMultiDeformableICP::addPair(Tuples<double>::P movable, Tuples<double>::P movableNormals, AddTuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3])
{
   m_D->m_Fragments.push_back(LMDeformableICP_PointPlane::getInstance(movable, movableNormals, fixed, fixedNormals, centroid, m_D->m_Func));
}

void AWT::LMMultiDeformableICP::f(const vnl_vector<double>& x, vnl_vector<double>& residuals)
{
   // This is how many model parameters there are
   const unsigned int nparams = x.size() - 7*m_D->m_Fragments.size();

   vnl_vector<double> modelParams(nparams);
   for (unsigned int i = 0; i < nparams; ++i)
      modelParams[i] = x[ 7*m_D->m_Fragments.size() + i ];

   unsigned int residPtr = 0;
   for (unsigned int f = 0; f < m_D->m_Fragments.size(); ++f)
   {
      vnl_vector<double> xx(7 + nparams);

      for (unsigned int i = 0; i < 7; ++i)
         xx(i + 0) = x(7*f + i);

      // Get the model parameters
      for (unsigned int i = 0; i < nparams; ++i)
         xx(i + 7) = modelParams(i);

      LMDeformableICP::P frag = m_D->m_Fragments[f];

      vnl_vector<double> resids(frag->get_number_of_residuals());

      frag->f(xx, resids);

      for (unsigned int i = 0; i < resids.size(); ++i)
         residuals[ i + residPtr ] = resids[ i ];

      residPtr += frag->get_number_of_residuals();
   }
}

struct AWT::LMDeformableICP::D
{
   Tuples<double>::P m_Movable;
   Tuples<double>::P m_MovableNormals;

   AddTuples<double>::P m_Fixed;
   Tuples<double>::P m_FixedNormals;

   double m_Centroid[3];

   double (*m_Func)(double);
};

AWT::LMDeformableICP::LMDeformableICP(Tuples<double>::P movable, Tuples<double>::P movableNormals, AddTuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)(double))
: vnl_least_squares_function(7 + fixed->getNumberOfComponents(), movable->getNumberOfPoints(), vnl_least_squares_function::no_gradient)
{
   m_D = new D;

   m_D->m_Movable = movable;
   m_D->m_MovableNormals = movableNormals;

   m_D->m_Fixed = fixed;
   m_D->m_FixedNormals = fixedNormals;

   m_D->m_Centroid[0] = centroid[0];
   m_D->m_Centroid[1] = centroid[1];
   m_D->m_Centroid[2] = centroid[2];

   m_D->m_Func = func;
}

AWT::LMDeformableICP_PointPoint::LMDeformableICP_PointPoint(Tuples<double>::P movable, Tuples<double>::P movableNormals, AddTuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)(double))
: LMDeformableICP(movable, movableNormals, fixed, fixedNormals, centroid, func)
{
}

AWT::LMDeformableICP_PointPlane::LMDeformableICP_PointPlane(Tuples<double>::P movable, Tuples<double>::P movableNormals, AddTuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)(double))
: LMDeformableICP(movable, movableNormals, fixed, fixedNormals, centroid, func)
{
}

AWT::LMDeformableICP_PointPoint::~LMDeformableICP_PointPoint()
{
}

AWT::LMDeformableICP_PointPlane::~LMDeformableICP_PointPlane()
{
}

AWT::LMDeformableICP::~LMDeformableICP()
{
   delete m_D;
}

AWT::LMDeformableICP::P AWT::LMDeformableICP_PointPoint::getInstance(Tuples<double>::P movable, Tuples<double>::P movableNormals, AddTuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)(double))
{
   AUTOGETINSTANCE(AWT::LMDeformableICP_PointPoint, (movable, movableNormals, fixed, fixedNormals, centroid, func));
}

AWT::LMDeformableICP::P AWT::LMDeformableICP_PointPlane::getInstance(Tuples<double>::P movable, Tuples<double>::P movableNormals, AddTuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)(double))
{
   AUTOGETINSTANCE(AWT::LMDeformableICP_PointPlane, (movable, movableNormals, fixed, fixedNormals, centroid, func));
}

GETNAMEMACRO(AWT::LMDeformableICP_PointPoint);
GETNAMEMACRO(AWT::LMDeformableICP_PointPlane);

vnl_matrix_fixed<double,4,4> AWT::LMDeformableICP::makeTransformation(const vnl_vector<double>& params, const double centroid[3])
{
   const double axis[] = { params[0], params[1], params[2] };
   const double theta  = params[3];
   const double translate[] = { params[4], params[5], params[6] };

   // Rotate about the centroid so that the movements due to rotation are smallest
   return TuplesFunctions<double>::rotationTranslationAboutPoint(axis, theta, centroid, translate);
}

void AWT::LMDeformableICP_PointPoint::f(const vnl_vector<double>& params, vnl_vector<double>& residuals)
{
   vnl_vector<double> movablePose(7);
   for (MeshIndex i = 0; i < 7; ++i)
      movablePose(i) = params(i);

   vnl_matrix_fixed<double,4,4> R = makeTransformation(params, m_D->m_Centroid);

   vnl_vector<double> deformableWeights(params.size() - 7);
   for (MeshIndex i = 0; i < params.size() - 7; ++i)
      deformableWeights(i) = params(i + 7);

   m_D->m_Fixed->setScale(deformableWeights.data_block());

   vnl_vector<double> vecFixed(4);
   vnl_vector<double> vecMovable(4);

   for (MeshIndex i = 0, imax = m_D->m_Movable->getNumberOfPoints(); i < imax; ++i)
   {
      m_D->m_Movable->getPoint(i, vecMovable.data_block());
      m_D->m_Fixed->getPoint(i, vecFixed.data_block());

      vecMovable(3) = vecFixed(3) = 1;

      vecMovable = R * vecMovable;

      // This is the linear residual
      const double residual = (vecFixed - vecMovable).magnitude();

      residuals(i) = m_D->m_Func(residual);
   }
}

#include "Mesh/AreaAveragedNormalCalculator.h"
#include "Useful/ArrayVectorFunctions.h"

void AWT::LMDeformableICP_PointPlane::f(const vnl_vector<double>& params, vnl_vector<double>& residuals)
{
   vnl_vector<double> movablePose = vectorslice(params, 0, 7);
   vnl_vector<double> deformableWeights = vectorslice(params, 7, m_D->m_Fixed->getNumberOfComponents());

   vnl_matrix_fixed<double,4,4> R = makeTransformation(movablePose, m_D->m_Centroid);

   m_D->m_Fixed->setScale(deformableWeights.data_block());
   
   vnl_vector<double> vecFixed(4);
   vnl_vector<double> vecMovable(4);
   vnl_vector<double> nmlFixed(4);

   for (MeshIndex i = 0, imax = m_D->m_Movable->getNumberOfPoints(); i < imax; ++i)
   {
      m_D->m_Movable->getPoint(i, vecMovable.data_block());

      m_D->m_Fixed->getPoint(i, vecFixed.data_block());
      m_D->m_FixedNormals->getPoint(i, nmlFixed.data_block());

      vecMovable(3) = vecFixed(3) = 1;
      nmlFixed(3) = 0;

      vecMovable = R * vecMovable;

      // This is the point-to-plane residual
      const double residual = dot_product<double>(vecFixed - vecMovable, nmlFixed);

      // This is the linear residual
      residuals(i) = m_D->m_Func(residual);
   }

   //const double rms = sqrt(residuals.squared_magnitude() / m_D->m_Movable->getNumberOfPoints());
   //PRINTVBL(rms);
}