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
#ifndef __VERTICESINCYLINDER_H__
#define __VERTICESINCYLINDER_H__

#include "Mesh.h"

#include "SearchAgent.h"
#include "SearchFilter.h"

#include <vector>

namespace AWT
{
   namespace SimpleMesh
   {
      class VerticesInCylinder : public SearchAgent
      {
      public:
         typedef ManagedAutoPointer<VerticesInCylinder> P;

      protected:
         VerticesInCylinder( );
         virtual ~VerticesInCylinder( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         virtual void calculateBoxDistanceBounds2( OEKDTreeBranch<double,3>* in_Branch, SqDistBounds<double>& bounds ) const;
         virtual bool shouldCheck( AWT::OEKDTree::OEKDTreeBranch<double,3>* in_Branch, const SqDistBounds<double>& bounds ) const;

         virtual void checkObject( const int in_Index );

         virtual void setCentre( const Point in_TestPoint  );
         virtual Point getCentre( ) const;

         virtual void setAxis( const Point in_TestPoint  );
         virtual Point getAxis( ) const;

         virtual void setRadius( const double outer, const double inner );
         virtual void getRadius( double& outer, double& inner ) const;

         virtual void setEnds( const double upper, const double lower );
         virtual void getEnds( double& upper, double& lower ) const;

         virtual std::vector<Index>& getVertices( );

         virtual void reset( );

         virtual void setData( AWT::OEKDTree::OEKDTreeData<double,3>::P data );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __VERTICESINCYLINDER_H__