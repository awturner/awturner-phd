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
#include "AlignParametric/CatesRegularizer.h"
#include "AlignParametric/CatesParticleSurface.h"

#include "Mesh/MeshImpl.h"
#include "Mesh/MeshGenerator.h"

#include "Useful/PrintMacros.h"
#include "Useful/Noise.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_matlab_filewrite.h"

using namespace AWT;
using namespace AWT::AlignParametric;

int main(int argc, char** argv)
{
   MeshType::P plane = MeshImpl<T>::getInstance(0, 0);
   AWT::MeshGenerator<T>::generateTorus(plane, 2, 1, 8, 8);

   /*
   T vtx[3];
   MESH_EACHVERTEX(plane, v)
   {
      plane->getVertex(v, vtx);

      for (MeshIndex i = 0; i < 3; ++i)
         vtx[i] = (vtx[i]-0.5) * 100;
      //vtx[2] = 0;

      plane->setVertex(v, vtx);
   }
   */

   vnl_matrix<T> samps(3, 1000);
   samps.set_row(2, 0.0);
   
   for (MeshIndex i = 0; i < samps.cols(); ++i)
   {
      for (MeshIndex ax = 0; ax < 3; ++ax)
         samps(ax, i) = Noise<T>::randu(-4,4);

      samps(2, i) = Noise<T>::randu(-2,2);
   }
   
   TuplesType::P samples = TuplesImpl<T>::getInstance(3, samps.cols());
   for (MeshIndex i = 0; i < samps.cols(); ++i)
   {
      samples->setPoint(i,samps.get_column(i).data_block());
   }

   const ValueRange<T> sigmaRange = { 0.05, 1000 };

   CatesParticleSurface::P cps = CatesParticleSurface::getInstance(plane, samples, 2500, sigmaRange);
   cps->initialize();
   cps->getRegularizer()->setDistanceType(CatesRegularizer::DT_EUCLIDEAN);

   Transformation trans; trans.set_identity();
   MatrixType reg;

   while (true)
   {
      // Copy the samples into the matrix, in order that figures can be checked in MATLAB
      cps->getSamples(samps);
      //PRINTVBLMATLAB(samps);

      PRINTVBL(cps->regularizationCost());
      cps->regularizationGradient(reg, trans);

      MatrixType controls;
      cps->getParameters(controls);
      //PRINTVBLNL(reg);

      const double sig = cps->getRegularizer()->getMinSigma();
      const double timestep = sig*sig / 2;

      {
         vnl_matlab_filewrite fw("catesdebug.mat");
         fw.write(controls, "curr");
         fw.write(controls + timestep*reg, "upd");
      }

      PAUSE;
      cps->setParameters(controls + timestep*reg);
   }
}