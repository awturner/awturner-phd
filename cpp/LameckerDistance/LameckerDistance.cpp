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
#include "Mesh/Mesh.h"
#include "Mesh/MeshSimilarity.h"
#include "Mesh/VTKMeshLoader.h"
#include "Useful/ProfTimer.h"

using namespace AWT;

typedef double T;

int main(int argc, char** argv)
{
   Mesh<T>::P meshA = VTKMeshLoader<T>::load(argv[1]);
   Mesh<T>::P meshB = VTKMeshLoader<T>::load(argv[2]);

   const T thresh = atof(argv[3]);
   const unsigned int subdivs = atoi(argv[4]);
   const unsigned int incorr  = atoi(argv[5]);

   MeshSimilarity<T>::P meshSim = MeshSimilarity<T>::getInstance(meshA, meshB, thresh, subdivs, incorr != 0);

   std::cout << "similarity = {" << std::endl;

   std::cout << "\t" << "meshes = {" << std::endl;
   std::cout << "\t\t" << "\"" << argv[1] << "\"," << std::endl;
   std::cout << "\t\t" << "\"" << argv[2] << "\"" << std::endl;
   std::cout << "\t" << "}," << std::endl;

   std::cout << "\t" << "mean = " << meshSim->getMeanDistance() << "," << std::endl;
   std::cout << "\t" << "rms  = " << meshSim->getRmsDistance() << "," << std::endl;
   std::cout << "\t" << "max  = " << meshSim->getMaximumDistance() << "," << std::endl;
   std::cout << "\t" << "aod  = " << meshSim->getAreaOfDeviation() << "," << std::endl;

   std::cout << "\t" << "aodthresh  = " << meshSim->getAreaOfDeviationThreshold() << "," << std::endl;
   std::cout << "\t" << "subdivs    = " << meshSim->getNumberOfSubdivisions() << std::endl;

   std::cout << "}" << std::endl;
   
   /*
   for (unsigned int subdivs = 0; subdivs < 15; ++subdivs)
   {
      ProfilingTimer timer;
      DEBUGMACRO(subdivs);

      timer.start();
      MeshSimilarity<T>::P meshSim = MeshSimilarity<T>::getInstance(meshA, meshB, thresh, subdivs, incorr != 0);
      timer.stop();

      std::cout << subdivs << "\t" << meshSim->getMeanDistance() << "\t" << meshSim->getRmsDistance() << "\t" << meshSim->getMaximumDistance() << "\t" << meshSim->getAreaOfDeviation() << "\t" << meshSim->getAreaOfDeviationThreshold() << "\t" << timer.getDurationInSecs() << std::endl;
   }
   */
}