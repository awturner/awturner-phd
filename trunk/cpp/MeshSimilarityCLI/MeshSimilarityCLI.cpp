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
#include "DeformableMeshFitting/DeformableMeshFittingStateMachine.h"
#include "Mesh/MeshSimilarity.h"

#include <iostream>

using namespace AWT;

int main(int argc, char** argv)
{
   for (int i = 1; i < argc; ++i)
   {
      StateMachineStack::P sms = StateMachineStack::getInstance();

      DeformableMeshFittingStateMachine<double>::P sm = DeformableMeshFittingStateMachine<double>::getInstance(sms);

      PRINTVBL(argv[i]);
      sm->load(argv[i]);

      try
      {
         MeshSimilarity<double>::P meshSim = MeshSimilarity<double>::getInstance(sm->getDeformableMeshFitting()->typename Mesh<T>::P meshA, typename Mesh<T>::P meshB, const T aodThresh, const unsigned int subdivs, const bool inCorr = false);

         std::cout << argv[i] << "\t" << meshSim->getMeanDistance() << "\t" << meshSim->getRmsDistance() << "\t"
            << meshSim->getMaximumDistance() << "\t" << meshSim->getAreaOfDeviation() << std::endl;
      }
      catch (AWT::Exception ex)
      {
         PRINTVBL(ex);
      }
      catch (...)
      {
         DEBUGMACRO("An exception was caught");
      }
   }
}