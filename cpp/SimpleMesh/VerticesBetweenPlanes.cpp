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
#include "VerticesBetweenPlanes.h"

#include "TreeData.h"
#include "Mesh.h"
#include "MeshFunctions.h"

#include "Useful/ArrayVectorFunctions.h"
#include "MeshSearching/BoxQueries.h"
#include "MeshSearching/FaceQueries.h"

using namespace AWT;
using namespace AWT::SimpleMesh;
using namespace AWT::OEKDTree;

struct AWT::SimpleMesh::VerticesBetweenPlanes::D
{
   Point* planes;
   Index* counts;
   Index nplanes;

   std::vector<Index> foundPoints;
};

AWT::SimpleMesh::VerticesBetweenPlanes::VerticesBetweenPlanes(const Index nplanes)
{
   m_D = new D;

   m_D->nplanes = nplanes;
   m_D->planes = new Point[nplanes];
   m_D->counts = new Index[nplanes];

   reset();
}

AWT::SimpleMesh::VerticesBetweenPlanes::~VerticesBetweenPlanes()
{
   delete [] m_D->planes;
   delete [] m_D->counts;
   delete m_D;
}

AWT::SimpleMesh::VerticesBetweenPlanes::P AWT::SimpleMesh::VerticesBetweenPlanes::getInstance(const Index nplanes)
{
   AUTOGETINSTANCE(AWT::SimpleMesh::VerticesBetweenPlanes, (nplanes));
}

GETNAMEMACRO(AWT::SimpleMesh::VerticesBetweenPlanes);

void AWT::SimpleMesh::VerticesBetweenPlanes::calculateBoxDistanceBounds2(OEKDTreeBranch<double,3>* in_Branch, SqDistBounds<double>& bounds) const
{
   boxChecked();

   // Check where the box vertices are in relation to the planes
   memset(m_D->counts, 0, m_D->nplanes * sizeof(Index));
   
   Point p;
   for (Index i = 0; i < 8; ++i)
   {
      for (Index ax = 0; ax < 3; ++ax)
         p(ax) = (i & (1 <<ax)) ? in_Branch->getMaximumBound(ax) : in_Branch->getMinimumBound(ax);
      p(3) = 1;

      for (Index ax = 0; ax < m_D->nplanes; ++ax)
         m_D->counts[ax] += dot(p, m_D->planes[ax], 4) >= 0;
   }

   bool want = true;

   for (Index i = 0; i < 2 && want; ++i)
      want = m_D->counts[i] != 0;

   bounds.lower = want ? 0 : FAR_FAR_AWAY;
   bounds.upper = FAR_FAR_AWAY;
}

void AWT::SimpleMesh::VerticesBetweenPlanes::checkObject(const int in_Index)
{
   objectChecked();

   Point p = mesh->getVertex(in_Index);

   bool want = true;
   for (Index i = 0; i < 2 && want; ++i)
      want = dot(p,m_D->planes[i],4) >= 0;

   if (want)
   {
      m_D->foundPoints.push_back(in_Index);
   }
}

bool AWT::SimpleMesh::VerticesBetweenPlanes::shouldCheck(AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const SqDistBounds<double>& bounds) const
{
   return bounds.lower == 0;
}

void AWT::SimpleMesh::VerticesBetweenPlanes::setTestPlane(const Index i, const Point& in_TestPlane)
{
   m_D->planes[i] = in_TestPlane;
}

Point AWT::SimpleMesh::VerticesBetweenPlanes::getTestPlane(const Index i) const
{
   return m_D->planes[i];
}

void AWT::SimpleMesh::VerticesBetweenPlanes::reset()
{
   SearchAgent::reset();
   m_D->foundPoints.clear();
}

std::vector<Index>& AWT::SimpleMesh::VerticesBetweenPlanes::getVertices()
{
   return m_D->foundPoints;
}