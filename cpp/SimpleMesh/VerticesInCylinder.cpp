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
#include "VerticesInCylinder.h"

#include "TreeData.h"
#include "Mesh.h"
#include "MeshFunctions.h"

#include "Useful/ArrayVectorFunctions.h"
#include "Useful/Noise.h"
#include "MeshSearching/BoxQueries.h"
#include "MeshSearching/FaceQueries.h"

#include "vnl/vnl_matlab_filewrite.h"

using namespace AWT;
using namespace AWT::SimpleMesh;
using namespace AWT::OEKDTree;

struct AWT::SimpleMesh::VerticesInCylinder::D
{
   Point centre;
   Point axis;

   Transformation trans;

   double Rmax2, Rmin2;
   double Zmax, Zmin;

   std::vector<Index> foundPoints;

   std::vector<Index> convex;

   void calculateTransformation()
   {
      // Pick a vector which isn't parallel to axis
      Point p1;
      Point p2;

      while (true)
      {
         for (Index ax = 0; ax < 3; ++ax)
            p2(ax) = Noise<double>::randu(-1,0);
         p2(3) = 0;

         cross(p2.data_block(), axis.data_block(), p1.data_block());
         p1(3) = 0;

         if (p1.squared_magnitude() != 0)
            break;
      }

      p1.normalize();

      // Pick the third vector
      cross(axis.data_block(), p1.data_block(), p2.data_block());
      p2(3) = 0;

      p2.normalize();

      // Make the rotation portion
      trans.set_row(0, p1);
      trans.set_row(1, p2);
      trans.set_row(2, axis);
      trans.set_row(3, 0.0);
      trans(3,3) = 1;

      // Now calculate the translation
      Point t = -trans*centre;
      trans.set_column(3,t);
      trans(3,3) = 1;
   }

   void findConvexVertices()
   {
      //DEBUGLINE;
      Points vs(4,8);

      convex.clear();

      for (Index i = 0; i < 8; ++i)
      {
         for (Index ax = 0; ax < 3; ++ax)
            vs(ax, i) = 2*((i & (1 << ax)) ? 1 : 0) - 1;

         vs(3,i) = 1;
      }

      // Flatten to the plane
      vs = trans * vs;
      vs.set_row(2,0.0);

      //PRINTVBLMATLAB(vs);

      // Now, use gift-wrapping to determine the convex set
      Index first = 0;
      for (Index i = 1; i < 8; ++i)
         if (vs(0,i) < vs(0,first))
            first = i;

      //PRINTVBL(first);
      convex.push_back(first);
      //PAUSE;

      while (true)
      {
         Index currConvex = convex.back();

         Index leftMost = (currConvex == 0) ? 1 : 0;
         for (Index j = 0; j < 8; ++j)
         {
            if (j != currConvex)
            {
               Point d1 = vs.get_column(leftMost) - vs.get_column(currConvex);
               Point d2 = vs.get_column(j)        - vs.get_column(currConvex);

               if ((d1(0)*d2(1) - d1(1)*d2(0)) < 0)
                  leftMost = j;
            }
         }

         if (leftMost == convex[0])
            break;

         convex.push_back(leftMost);
         //PRINTVBL(leftMost);
         //PAUSE;
      }
      //DEBUGLINE;
   }
};

AWT::SimpleMesh::VerticesInCylinder::VerticesInCylinder()
{
   m_D = new D;

   // cylinder pointing in the Z direction
   m_D->centre.fill(0); m_D->centre(3) = 1;
   m_D->axis.fill(0);   m_D->axis(0) = 1;

   m_D->calculateTransformation();
   //m_D->findConvexVertices();

   m_D->Zmin = -std::numeric_limits<double>::max();
   m_D->Zmax =  std::numeric_limits<double>::max();

   m_D->Rmin2 = 0;
   m_D->Rmax2 = std::numeric_limits<double>::max();

   reset();
}

AWT::SimpleMesh::VerticesInCylinder::~VerticesInCylinder()
{
   delete m_D;
}

AWT::SimpleMesh::VerticesInCylinder::P AWT::SimpleMesh::VerticesInCylinder::getInstance()
{
   AUTOGETINSTANCE(AWT::SimpleMesh::VerticesInCylinder, ());
}

GETNAMEMACRO(AWT::SimpleMesh::VerticesInCylinder);

void AWT::SimpleMesh::VerticesInCylinder::calculateBoxDistanceBounds2(OEKDTreeBranch<double,3>* in_Branch, SqDistBounds<double>& bounds) const
{
   boxChecked();

   /*
   bounds.lower = 0;
   bounds.upper = FAR_FAR_AWAY;
   return;
   */

   Points Vs(4,8);
   Points vs(4,8);

   /*
   DEBUGMACRO("Bounds");
   for (Index ax = 0; ax < 3; ++ax)
      DEBUGMACRO(ax << "\t" << in_Branch->getMinimumBound(ax) << "\t" << in_Branch->getMaximumBound(ax));
   */

   for (Index i = 0; i < 8; ++i)
   {
      for (Index ax = 0; ax < 3; ++ax)
         Vs(ax, i) = (i & (1 << ax)) ? in_Branch->getMaximumBound(ax) : in_Branch->getMinimumBound(ax);

      Vs(3,i) = 1;
   }

   // Transform to the plane
   vs = m_D->trans * Vs;

   Point  pClosest, pFurthest;

   // Check the z values
   double minZ =  FAR_FAR_AWAY;
   double maxZ = -FAR_FAR_AWAY;
   for (Index i = 0; i < 8; ++i)
   {
      minZ = std::min(minZ, vs(2,i));
      maxZ = std::max(maxZ, vs(2,i));

      vs(2,i) = 0;
   }

   if (maxZ < m_D->Zmin || minZ > m_D->Zmax)
   {
      //DEBUGMACRO("Z ranges do not intersect");
      // The box doesn't intersect the ends, so don't check it
      bounds.upper = bounds.lower = FAR_FAR_AWAY;
   }
   else
   {
      // Go around the convex hull and try to find the closest point on each segment
      
      double dClosest = FAR_FAR_AWAY, dFurthest = 0;
      bool side = true;
      Point crossP;

      const Index nconvex = m_D->convex.size();
      for (Index i = 0; i < nconvex; ++i)
      {
         const Point x0 = vs.get_column(m_D->convex[i]);
         const Point xx = -x0;
         const Point bb = vs.get_column(m_D->convex[(i+1)%nconvex]) - x0;

         const double lambda = std::min(std::max(dot(xx,bb,3) / dot(bb,bb,3),0.0),1.0);

         Point p = x0 + lambda * bb;
         const double dd = p.squared_magnitude();

         if (dd < dClosest)
         {
            dClosest = dd;
            pClosest = p;
         }

         if (dd > dFurthest)
         {
            dFurthest = dd;
            pFurthest = p;
         }

         // For checking if it's inside the convex hull
         //cross(xx.data_block(), bb.data_block(), crossP.data_block());
         side &= (xx(0)*bb(1) - xx(1)*bb(0)) < 0;
      }

      if (dFurthest < m_D->Rmin2)
      {
         //DEBUGMACRO("Furthert vertex is inside the inner cylinder");
         // If the farthest vertex is inside the inner cylinder, it doesn't intersect the range
         bounds.upper = bounds.lower = FAR_FAR_AWAY;
      }
      else if (dClosest < m_D->Rmax2)
      {
         // If the closest vertex is inside the outer radius, accept it
         // Note that this will accept some unnecessary boxes
         bounds.lower = 0;
         bounds.upper = FAR_FAR_AWAY;
      }
      else
      {
         bounds.lower = FAR_FAR_AWAY;
         bounds.upper = FAR_FAR_AWAY;
      }

      if (side)
      {
         //DEBUGMACRO("Centre is inside convex hull");
         pClosest.fill(0);
         pClosest(3) = 1;

         bounds.lower = 0;
         bounds.upper = FAR_FAR_AWAY;
      }
   }

   /*
   {
      vnl_matlab_filewrite fw("vic.mat");

      vnl_vector<double> cvs(m_D->convex.size());
      for (Index i = 0; i < m_D->convex.size(); ++i)
         cvs(i) = 1 + m_D->convex[i];

      fw.write(vs, "vs");
      fw.write(Vs, "Vs");
      fw.write(pClosest, "Pc");
      fw.write(m_D->trans, "T");
      fw.write(sqrt(m_D->Rmax2), "R");
      fw.write(sqrt(m_D->Rmin2), "r");
      fw.write(cvs, "convex");
      fw.write(m_D->Zmin, "Zmin");
      fw.write(m_D->Zmax, "Zmax");

      fw.write(bounds.lower, "Bl");
      fw.write(bounds.lower, "Bu");
   }
   PRINTVBL(bounds.lower);
   PAUSE;
   */
}

void AWT::SimpleMesh::VerticesInCylinder::checkObject(const int in_Index)
{
   objectChecked();

   // Get the vertex and immediately translate it into the cylinder's coord system
   Point p = m_D->trans * mesh->getVertex(in_Index);

   if (p(2) >= m_D->Zmin && p(2) <= m_D->Zmax)
   {
      // Good start - it's between the end planes
      
      const double r2 = p(0)*p(0) + p(1)*p(1);
      if (r2 >= m_D->Rmin2 && r2 <= m_D->Rmax2)
      {
         // Even better - it's inside the cylinder
         m_D->foundPoints.push_back(in_Index);
      }
   }
}

bool AWT::SimpleMesh::VerticesInCylinder::shouldCheck(AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const SqDistBounds<double>& bounds) const
{
   return bounds.lower == 0;;
}

void AWT::SimpleMesh::VerticesInCylinder::setCentre(const Point centre )
{
   m_D->centre = centre;
   m_D->calculateTransformation();
}

Point AWT::SimpleMesh::VerticesInCylinder::getCentre() const
{
   return m_D->centre;
}

void AWT::SimpleMesh::VerticesInCylinder::setAxis(const Point axis )
{
   m_D->axis = axis;
   m_D->calculateTransformation();
   m_D->findConvexVertices();
}

Point AWT::SimpleMesh::VerticesInCylinder::getAxis() const
{
   return m_D->axis;
}

void AWT::SimpleMesh::VerticesInCylinder::setRadius(const double Rmax, const double Rmin)
{
   m_D->Rmax2 = Rmax * Rmax;
   m_D->Rmin2 = Rmin * Rmin;
}

void AWT::SimpleMesh::VerticesInCylinder::getRadius(double& Rmax, double& Rmin) const
{
   Rmax = sqrt(m_D->Rmax2);
   Rmin = sqrt(m_D->Rmin2);
}

void AWT::SimpleMesh::VerticesInCylinder::setEnds(const double Zmax, const double Zmin)
{
   m_D->Zmax = Zmax;
   m_D->Zmin = Zmin;
}

void AWT::SimpleMesh::VerticesInCylinder::getEnds(double& Zmax, double& Zmin) const
{
   Zmax = m_D->Zmax;
   Zmin = m_D->Zmin;
}

void AWT::SimpleMesh::VerticesInCylinder::reset()
{
   SearchAgent::reset();
   m_D->foundPoints.clear();
}

std::vector<Index>& AWT::SimpleMesh::VerticesInCylinder::getVertices()
{
   return m_D->foundPoints;
}