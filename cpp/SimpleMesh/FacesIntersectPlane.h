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
#ifndef __FACESINTERSECTPLANE_H__
#define __FACESINTERSECTPLANE_H__

#include "Mesh.h"
#include "SearchAgent.h"

#include "SearchFilter.h"
#include <vector>

namespace AWT
{
   namespace SimpleMesh
   {
      struct IndexLine
      {
         Index i;
         Point p[2];
      };

      class FacesIntersectPlane : public SearchAgent
      {
      public:
         typedef ManagedAutoPointer<FacesIntersectPlane> P;

      protected:
         FacesIntersectPlane( );
         virtual ~FacesIntersectPlane( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         virtual void calculateBoxDistanceBounds2( OEKDTreeBranch<double,3>* in_Branch, SqDistBounds<double>& bounds ) const;

         virtual bool shouldCheck( OEKDTreeBranch<double,3>* in_Branch, const SqDistBounds<double>& bounds ) const;

         virtual void checkObject( const int in_Index );

         virtual void setTestPlane( const Point in_Plane );
         virtual Point getTestPlane( ) const;

         virtual std::vector<IndexLine>& getFaces( );

         virtual void reset( );

         virtual void setData( AWT::OEKDTree::OEKDTreeData<double,3>::P data );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __FACESINTERSECTPLANE_H__