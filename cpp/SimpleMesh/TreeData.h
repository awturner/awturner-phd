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
#ifndef __SIMPLEMESHTREEDATA_H__
#define __SIMPLEMESHTREEDATA_H__

#include "ObjectEKDTree/OEKDTreeData.h"
#include "Mesh.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class TreeData : public OEKDTree::OEKDTreeData<double,3>
      {
      protected:
         TreeData(Mesh::P mesh, const bool _useFaces);
         virtual ~TreeData();

      public:
         typedef ManagedAutoPointer< TreeData > P;

         static P getInstance(Mesh::P mesh, const bool _useFaces);

      public:
         std::string getClassName() const;

         virtual OEKDTree::ObjectIndex getNumberOfObjects() const;

         virtual double getCoordinate(OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis) const;
         virtual void getPosition(const OEKDTree::ObjectIndex objectIndex, double* pos) const;

         virtual double getMinimumBound(OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis) const;
         virtual double getMaximumBound(OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis) const;

         virtual Mesh::P getMesh();

         const bool useFaces;

      protected:
         struct D;
         D* m_D;
      };
   }
}

#endif // __SIMPLEMESHTREEDATA_H__