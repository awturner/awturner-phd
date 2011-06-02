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
#include "SamplingGrid.h"

#include "vnl/algo/vnl_svd.h"
#include "Maths/BSpline.h"
#include "LuaPlus.h"
#include <vector>
#include "Useful/PrintMacros.h"

#include <fstream>

#define TYPEDDIV(T,x,y) (static_cast<T>(x)/static_cast<T>(y))

namespace AWT
{
   namespace AlignParametric
   {
      SamplingGrid::SamplingGrid(const Idx _ns[2], const Idx _nk[2])
      {
         DEBUGLINE;
         for (Idx i = 0; i < 2; ++i)
         {
            ns[i] = _ns[i];
            nk[i] = _nk[i];
         }
         PRINTVEC(ns, 2);
         PRINTVEC(nk, 2);

         const double kernelscale[] = {
            TYPEDDIV(double, 1, nk[0]),
            TYPEDDIV(double, 1, nk[1]),
         };

         const Idx nsamples = ns[0] * ns[1];
         const Idx nkernels = nk[0] * nk[1];

         // The matrix of control values is nice and easy to set up - set it to zero
         // Each row stores x,y component of control value
         controlValues[0].set_size(nkernels);
         controlValues[1].set_size(nkernels);

         // The weights matrix is constructed eigOpt that displacement = ws * controlValues
         // i.e. the row corresponds to the sample, column corresponds to kernel
         weightings.set_size(nsamples, nkernels);
         BSpline<double> bspline(256);

         double spos[2];
         double kpos[2];
         double deltas[2];
         for (Idx r = 0; r < nsamples; ++r)
         {
            getPointPosition(ns, r, spos);

            for (Idx c = 0; c < nkernels; ++c)
            {
               getPointPosition(nk, c, kpos);

               Idx i;
               double w = 1;
               for (i = 0; i < 2; ++i)
               {
                  deltas[i] = spos[i] - kpos[i];

                  if (deltas[i] < -0.5)
                     deltas[i] += 1;
                  else if (deltas[i] > 0.5)
                     deltas[i] -= 1;

                  if (abs(deltas[i]) >= 2*kernelscale[i])
                     break;

                  w *= bspline.evaluate(deltas[i]/kernelscale[i]);
               }

               if (i == 2)
               {
                  weightings(r, c) = w;
               }
            }
         }
      }

      SamplingGrid::~SamplingGrid()
      {
      }

      void SamplingGrid::getNSamples(Idx ns[2]) const
      {
         ns[0] = this->ns[0];
         ns[1] = this->ns[1];
      }

      void SamplingGrid::getNKernels(Idx nk[2]) const
      {
         nk[0] = this->nk[0];
         nk[1] = this->nk[1];
      }

      void SamplingGrid::getPositions(Displacements positions[2]) const
      {
         getDisplacements(positions);

         positions[0].fill(0);
         positions[1].fill(0);

         double pos[2];
         for (Idx i = 0; i < ns[0]*ns[1]; ++i)
         {
            getPointPosition(ns, i, pos);

            positions[0][i] += pos[0];
            positions[1][i] += pos[1];
         }
      }

      // Calculate the displacement from start position
      void SamplingGrid::getDisplacements(Displacements displacements[2]) const
      {
         for (int i = 0; i < 2; ++i)
         {
            displacements[i].set_size(weightings.rows());
            weightings.mult(controlValues[i], displacements[i]);
         }
      }

      void SamplingGrid::getPointPosition(const Idx ns[2], const Idx i, double pos[2])
      {
         pos[0] = TYPEDDIV(double, i % ns[0], ns[0]);
         pos[1] = TYPEDDIV(double, i / ns[0], ns[1]);
      }

      void SamplingGrid::calculateFromOther(const SamplingGrid& other)
      {
         if (other.ns[0] != this->ns[0] || other.ns[1] != this->ns[1])
            throw "Different number of samples";

         // Calculate the displacements on the other grid
         Displacements otherDisplacements[2];
         other.getDisplacements(otherDisplacements);

         // (Alternative to below is to set up and solve a LSQR system)
         // Convert this weight matrix into a full matrix
         KernelWeightsFull fullWs(weightings.rows(), weightings.cols());
         fullWs.fill(0);

         weightings.reset();
         while (weightings.next())
            fullWs(weightings.getrow(), weightings.getcolumn()) = weightings.value();

         vnl_svd<double> svd(fullWs);
         for (Idx i = 0; i < 2; ++i)
            svd.solve(otherDisplacements[i].data_block(), controlValues[i].data_block());
      }

      const KernelWeights& SamplingGrid::getWeights() const
      {
         return weightings;
      }

      ControlValues& SamplingGrid::getParameters(Idx axis)
      {
         return controlValues[axis];
      }
   }
}

AWT::AlignParametric::SamplingGrid::P AWT::AlignParametric::SamplingGrid::getInstance(const AWT::AlignParametric::Idx _ns[], const AWT::AlignParametric::Idx _nk[])
{
   AUTOGETINSTANCE(AWT::AlignParametric::SamplingGrid, (_ns, _nk));
}

GETNAMEMACRO(AWT::AlignParametric::SamplingGrid);
