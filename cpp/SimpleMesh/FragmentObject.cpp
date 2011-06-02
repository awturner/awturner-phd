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
#include "FragmentObject.h"

#include "MeshFunctions.h"

#include "vnl/algo/vnl_svd.h"

using namespace AWT::SimpleMesh;

struct AWT::SimpleMesh::FragmentObject::D
{
   MeshEdgeSearchFilter::P edgeFilter;

   Transformation trans;
   Transformation inv;

   Transformation preJacs[7];

   double wiggleSize;

   static void orthoInvert(const Transformation& fwd, Transformation& inv)
   {
      inv.set_column(3, 0.0);
      inv.set_row(3, 0.0);
      inv(3,3) = 1;

      for (Index r = 0; r < 3; ++r)
      {
         for (Index c = 0; c < 3; ++c)
         {
            // Transpose the rotation block
            inv(c, r) = fwd(r, c);

            // Decrement the translation block
            inv(r,3) -= fwd(c,r) * fwd(c,3);
         }
      }
   }

   static void calculateTransformation(const Vector& v, Transformation& mat)
   {
      Vector q(4);
      q.set(v.data_block());
      q.normalize();

      mat(0,0) = q(0) * q(0) + q(1) * q(1) - q(2) * q(2) - q(3) * q(3);
      mat(0,1) = 2 * q(1) * q(2) - 2 * q(0) * q(3);
      mat(0,2) = 2 * q(1) * q(3) + 2 * q(0) * q(2);

      mat(1,0) = 2 * q(1) * q(2) + 2 * q(0) * q(3);
      mat(1,1) = q(0) * q(0) - q(1) * q(1) + q(2) * q(2) - q(3) * q(3);
      mat(1,2) = 2 * q(2) * q(3) - 2 * q(0) * q(1);       

      mat(2,0) = 2 * q(1) * q(3) - 2 * q(0) * q(2);
      mat(2,1) = 2 * q(2) * q(3) + 2 * q(0) * q(1);
      mat(2,2) = q(0) * q(0) - q(1) * q(1) - q(2) * q(2) + q(3) * q(3);

      mat(0,3) = v(4);
      mat(1,3) = v(5);
      mat(2,3) = v(6);

      mat.set_row(3, 0.0);
      mat(3,3) = 1;
   }

   void updateTransformations(const Vector& setPoint)
   {
      // Calculate the forward and reverse transformation
      calculateTransformation(setPoint, trans);
      orthoInvert(trans, inv);

      // preJacs[i] * a sample point = the gradient of that point wrt element i
      Vector v(7);
      v.update(setPoint);
      Transformation Tp, Tn;
      for (Index i = 0; i < 7; ++i)
      {
         v[i] = setPoint[i] + wiggleSize;
         calculateTransformation(v, Tp);

         v[i] = setPoint[i] - wiggleSize;
         calculateTransformation(v, Tn);

         // Finite-difference approximation to the gradient
         preJacs[i] = (Tp - Tn) / (2*wiggleSize);

         v.update(setPoint);
      }
   }
};

AWT::SimpleMesh::FragmentObject::FragmentObject(Mesh::P frag, Sampler::P sampler, Index nsamples)
: AlignObject(nsamples)
{
   m_D = new D;

   // This is the finite difference
   m_D->wiggleSize = 1e-3;

   this->mesh    = frag;
   this->sampler = sampler;

   this->searcher = Search::getInstance(this->mesh, true);
   //PRINTVBL(this->mesh->nf);
   //PRINTVBL(this->mesh->nv);

   // Create this so we can know if a correspondence is on the fragment's edge
   m_D->edgeFilter = MeshEdgeSearchFilter::getInstance(this->mesh);

   // Initialize the parameters
   params.set_size(7);
   params.fill(0);
   params(0) = 1;

   setParameters(params);
}

AWT::SimpleMesh::FragmentObject::~FragmentObject()
{
   delete m_D;
}

AWT::SimpleMesh::FragmentObject::P AWT::SimpleMesh::FragmentObject::getInstance(Mesh::P frag, Sampler::P sampler, Index nsamples)
{
   AUTOGETINSTANCE(AWT::SimpleMesh::FragmentObject, (frag, sampler, nsamples));
}

GETNAMEMACRO(AWT::SimpleMesh::FragmentObject);

void AWT::SimpleMesh::FragmentObject::setParameters(const Vector& params)
{
   AlignObject::setParameters(params);

   Vector q(4);
   q.set(params.data_block());
   q.normalize();

   this->params.update(q, 0);

   m_D->updateTransformations(this->params);
}

PointIndexWeights AWT::SimpleMesh::FragmentObject::search(const Point& pnt, const Point& nml, SearchFilter::P filter)
{
   PointIndexWeights piw = searcher->search(pnt, nml, filter);

   return piw;
}

Transformation AWT::SimpleMesh::FragmentObject::getTransformation() const
{
   return m_D->trans;
}

Transformation AWT::SimpleMesh::FragmentObject::getInverseTransformation() const
{
   return m_D->inv;
}

void AWT::SimpleMesh::FragmentObject::calculateJacobian(const PointIndexWeights& piw, Matrix& jac)
{
   ensureSize(jac, 4, 7);

   jac.fill(0);
   for (Index i = 0; i < 7; ++i)
      jac.set_column(i, m_D->preJacs[i] * piw.p);
}

SearchFilter::P AWT::SimpleMesh::FragmentObject::getEdgeFilter()
{
   return m_D->edgeFilter;
}