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
#include "Sampler.h"
#include "Useful/ColouredConsole.h"

#include "MeshFunctions.h"
#include "Useful/Noise.h"

using namespace AWT::SimpleMesh;

AWT::SimpleMesh::Sampler::Sampler(Mesh::P mesh)
{
   this->mesh = mesh;

   cumAreas = new double[ mesh->nf ];
   for (Index i = 0; i < mesh->nf; ++i)
      cumAreas[i] = 0;
}

AWT::SimpleMesh::Sampler::~Sampler()
{
   delete [] cumAreas;
}

Mesh::P AWT::SimpleMesh::Sampler::getMesh()
{
   return mesh;
}

AWT::SimpleMesh::PointIndexWeights AWT::SimpleMesh::Sampler::sample()
{
   PointIndexWeights ret;

   double v = Noise<double>::randu(0, 1);

   // Now, need to work out which face this falls on
   Index lower = 0;
   Index upper = mesh->nf;

   while (upper - lower > 1)
   {
      Index mid = (lower + upper) / 2;

      if (cumAreas[mid] >= v)
         upper = mid;
      else
         lower = mid;
   }

   while (cumAreas[lower] == cumAreas[lower+1])
      ++lower;

   //PRINTVBL(v);
   //DEBUGMACRO(lower+0 << ": " << cumAreas[lower+0]);
   //DEBUGMACRO(lower+1 << ": " << cumAreas[lower+1]);
   //BLANKLINE;

   ret.i = lower;

   // Now, pick a point on this triangle
   ret.w(0) = Noise<double>::randu(0, 1);
   ret.w(1) = Noise<double>::randu(0, 1);

   if (ret.w(0) + ret.w(1) > 1)
   {
      ret.w(0) = 1-ret.w(0);
      ret.w(1) = 1-ret.w(1);
   }

   Point vs[3];
   getFaceVertices(mesh, ret.i, vs);
   
   ret.p = (1 - ret.w(0) - ret.w(1))*vs[0] + ret.w(0)*vs[1] + ret.w(1)*vs[2];
   
   return ret;
}
