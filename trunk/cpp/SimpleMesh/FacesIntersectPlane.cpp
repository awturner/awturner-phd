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
#include "FacesIntersectPlane.h"

#include "TreeData.h"
#include "Mesh.h"
#include "MeshFunctions.h"

#include "Useful/ArrayVectorFunctions.h"
#include "MeshSearching/BoxQueries.h"
#include "MeshSearching/FaceQueries.h"

using namespace AWT;
using namespace AWT::SimpleMesh;
using namespace AWT::OEKDTree;

struct AWT::SimpleMesh::FacesIntersectPlane::D
{
   Point testPlane;
   std::vector<IndexLine> foundPoints;
};

AWT::SimpleMesh::FacesIntersectPlane::FacesIntersectPlane( )
{
   m_D = new D;

   reset( );
}

AWT::SimpleMesh::FacesIntersectPlane::~FacesIntersectPlane( )
{
   delete m_D;
}

AWT::SimpleMesh::FacesIntersectPlane::P AWT::SimpleMesh::FacesIntersectPlane::getInstance( )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::FacesIntersectPlane, ( ) );
}

GETNAMEMACRO( AWT::SimpleMesh::FacesIntersectPlane );

void AWT::SimpleMesh::FacesIntersectPlane::calculateBoxDistanceBounds2( OEKDTreeBranch<double,3>* in_Branch, SqDistBounds<double>& bounds ) const
{
   boxChecked();

   // Get the 8 vertices of the box and see which side of the plane they are on
   Point p;
   int countSigns = 0;
   for ( Index v = 0; v < 8; ++v )
   {
      for ( Index ax = 0; ax < 3; ++ax )
      {
         p(ax) = (v & (1 << ax)) ? in_Branch->getMinimumBound(ax) : in_Branch->getMaximumBound(ax);
      }
      p(3) = 1;

      if ( dot( p, m_D->testPlane, 4 ) >= 0 )
         ++countSigns;
   }

   bounds.lower = (countSigns != 8 && countSigns != 0) ? 0 : FAR_FAR_AWAY;
   bounds.upper = FAR_FAR_AWAY;
}

void AWT::SimpleMesh::FacesIntersectPlane::checkObject( const int in_Index )
{
   objectChecked();

   Point vs[3];
   getFaceVertices( mesh, in_Index, vs );

   // Calculate the sign of the dot product between this face and the plane
   int countSigns = 0;
   for ( Index i = 0; i < 3; ++i )
      countSigns += dot( vs[i], m_D->testPlane, 4 ) >= 0;

   if (countSigns != 3 && countSigns != 0)
   {
      IndexLine il;

      il.i = in_Index;

      int psFound = 0;
      for ( Index i = 0; i < 3 && psFound < 2; ++i )
      {
         double den = dot( vs[i]-vs[(i+1)%3], m_D->testPlane, 4 );
         if (den != 0)
         {
            double alpha = dot( vs[i], m_D->testPlane, 4 ) / den;

            if (alpha >= 0 && alpha <= 1)
               il.p[psFound++] = vs[i] + alpha * (vs[(i+1)%3]-vs[i]);
         }
      }

      if (psFound == 2)
      {
         // The triangle vertices straddle the plane, so there is an intersection
         m_D->foundPoints.push_back(il);
      }
   }
}

bool AWT::SimpleMesh::FacesIntersectPlane::shouldCheck( AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const AWT::OEKDTree::SqDistBounds<double>& bounds ) const
{
   return bounds.lower == 0;
}

void AWT::SimpleMesh::FacesIntersectPlane::setTestPlane( const Point in_Plane  )
{
   m_D->testPlane = in_Plane;
}

Point AWT::SimpleMesh::FacesIntersectPlane::getTestPlane( ) const
{
   return m_D->testPlane;
}

void AWT::SimpleMesh::FacesIntersectPlane::reset( )
{
   SearchAgent::reset();
   m_D->foundPoints.clear();
}

std::vector<IndexLine>& AWT::SimpleMesh::FacesIntersectPlane::getFaces( )
{
   return m_D->foundPoints;
}
