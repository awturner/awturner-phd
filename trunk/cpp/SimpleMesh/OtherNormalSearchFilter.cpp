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
#include "OtherNormalSearchFilter.h"

#include "MeshFunctions.h"

struct AWT::SimpleMesh::OtherNormalSearchFilter::D
{
   Point testPoint;
   Mesh::P mesh;

   double cosineThreshold;
};

AWT::SimpleMesh::OtherNormalSearchFilter::OtherNormalSearchFilter(Mesh::P mesh)
{
   m_D = new D;

   m_D->mesh = mesh;
   m_D->cosineThreshold = 0.707;
}

AWT::SimpleMesh::OtherNormalSearchFilter::~OtherNormalSearchFilter()
{
   delete m_D;
}

AWT::SimpleMesh::OtherNormalSearchFilter::P AWT::SimpleMesh::OtherNormalSearchFilter::getInstance(Mesh::P mesh)
{
   AUTOGETINSTANCE(AWT::SimpleMesh::OtherNormalSearchFilter, (mesh));
}

GETNAMEMACRO(AWT::SimpleMesh::OtherNormalSearchFilter);

void AWT::SimpleMesh::OtherNormalSearchFilter::setTestPoint(const AWT::SimpleMesh::Point p)
{
   m_D->testPoint = p;
   modified();
}

bool AWT::SimpleMesh::OtherNormalSearchFilter::check(const Point p, const Index i) const
{
   return true;

   // Need to look up the normal on the other mesh
   Point nml = calculateFaceNormal(m_D->mesh, i);

   Point delta = m_D->testPoint - p;
   if (delta.squared_magnitude() < 1)
      return true;

   return inner_product<double>(delta.normalize(), nml) >= m_D->cosineThreshold;
}

AWT::SimpleMesh::Point AWT::SimpleMesh::OtherNormalSearchFilter::getTestPoint() const
{
   return m_D->testPoint;
}

AWT::SimpleMesh::Mesh::P AWT::SimpleMesh::OtherNormalSearchFilter::getTestMesh() const
{
   return m_D->mesh;
}

void AWT::SimpleMesh::OtherNormalSearchFilter::setCosineThreshold(const double v)
{
   if (v != m_D->cosineThreshold)
   {
      m_D->cosineThreshold = v;
      modified();
   }
}

double AWT::SimpleMesh::OtherNormalSearchFilter::getCosineThreshold() const
{
   return m_D->cosineThreshold;
}