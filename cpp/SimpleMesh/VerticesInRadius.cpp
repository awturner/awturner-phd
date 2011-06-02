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
#include "VerticesInRadius.h"

#include "TreeData.h"
#include "Mesh.h"
#include "MeshFunctions.h"

#include "Useful/ArrayVectorFunctions.h"
#include "MeshSearching/BoxQueries.h"
#include "MeshSearching/FaceQueries.h"

using namespace AWT;
using namespace AWT::SimpleMesh;
using namespace AWT::OEKDTree;

struct AWT::SimpleMesh::VerticesInRadius::D
{
   Point testPoint;

   double outerRadius2;
   double innerRadius2;

   std::vector<Index> foundPoints;
};

AWT::SimpleMesh::VerticesInRadius::VerticesInRadius()
{
   m_D = new D;
   reset();
}

AWT::SimpleMesh::VerticesInRadius::~VerticesInRadius()
{
   delete m_D;
}

AWT::SimpleMesh::VerticesInRadius::P AWT::SimpleMesh::VerticesInRadius::getInstance()
{
   AUTOGETINSTANCE(AWT::SimpleMesh::VerticesInRadius, ());
}

GETNAMEMACRO(AWT::SimpleMesh::VerticesInRadius);

void AWT::SimpleMesh::VerticesInRadius::calculateBoxDistanceBounds2(OEKDTreeBranch<double,3>* in_Branch, SqDistBounds<double>& bounds) const
{
   boxChecked();

   bounds.lower = BoxQueries<double>::getClosestDistance2(m_D->testPoint.data_block(), in_Branch, FAR_FAR_AWAY);
   bounds.upper = BoxQueries<double>::getFarthestDistance2(m_D->testPoint.data_block(), in_Branch, FAR_FAR_AWAY);
}

void AWT::SimpleMesh::VerticesInRadius::checkObject(const int in_Index)
{
   objectChecked();

   Point p = mesh->getVertex(in_Index);
   const double thisDistance2 = deltaNormSquared(m_D->testPoint.data_block(), p.data_block(), 3);

   // This is a bit minging because this is essentially another search filter... oh well
   if (thisDistance2 <= m_D->outerRadius2 && thisDistance2 >= m_D->innerRadius2)
   {
      m_D->foundPoints.push_back(in_Index);
   }
}

bool AWT::SimpleMesh::VerticesInRadius::shouldCheck(AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const SqDistBounds<double>& bounds) const
{
   return (bounds.upper >= m_D->innerRadius2) && (m_D->outerRadius2 >= bounds.lower);
}

void AWT::SimpleMesh::VerticesInRadius::setTestPoint(const Point in_TestPoint )
{
   m_D->testPoint = in_TestPoint;
}

Point AWT::SimpleMesh::VerticesInRadius::getTestPoint() const
{
   return m_D->testPoint;
}

void AWT::SimpleMesh::VerticesInRadius::setInnerRadius(const double v)
{
   m_D->innerRadius2 = v * v;
}

double AWT::SimpleMesh::VerticesInRadius::getInnerRadius() const
{
   return sqrt(m_D->innerRadius2);
}

void AWT::SimpleMesh::VerticesInRadius::setOuterRadius(const double v)
{
   m_D->outerRadius2 = v * v;
}

double AWT::SimpleMesh::VerticesInRadius::getOuterRadius() const
{
   return sqrt(m_D->outerRadius2);
}

void AWT::SimpleMesh::VerticesInRadius::reset()
{
   SearchAgent::reset();
   m_D->foundPoints.clear();
}

std::vector<Index>& AWT::SimpleMesh::VerticesInRadius::getVertices()
{
   return m_D->foundPoints;
}