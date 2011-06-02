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
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"
#include "RandomMeshSlicer/RandomMeshSlicer.h"

#include <iostream>
#include <time.h>

#include "Useful/Noise.h"

using namespace AWT;

typedef double T;

int main(int argc, char** argv)
{
   if (argc < 4)
   {
      std::cerr << "Usage: RandomMeshSlicerCLI <mesh filename> <# slice planes> <printf for fragment filenames>" << std::endl;
      DEBUGLINEANDEXIT(1);
   }

   Noise<double>::timeSeed();

   Mesh<T>::P mesh = VTKMeshLoader<T>::load(argv[1]);

   unsigned int nplanes = atoi(argv[2]);

   if (nplanes > 5)
   {
      std::cerr << "Too many planes! (n <= 5)" << std::endl;
      DEBUGLINEANDEXIT(1);
   }
   
   RandomMeshSlicer<T>::P rms = RandomMeshSlicer<T>::getInstance(mesh, nplanes);
   std::cerr << "Created " << rms->getNumberOfFragments() << " fragments." << std::endl;

   char buffer[80];
   for (unsigned int i = 0; i < rms->getNumberOfFragments(); ++i)
   {
      Mesh<T>::P frag = rms->getFragment(i);

      sprintf_s(buffer, argv[3], i);

      VTKMeshWriter<T>::write(frag, buffer);
   }
}