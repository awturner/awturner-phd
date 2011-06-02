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
#include "FixedMethods.h"

#include "CommonMethods.h"

#include "AlignParametric/FixedOptimizer.h"
#include "Mesh/VTKMeshLoader.h"

using namespace AWT;
using namespace AWT::AlignParametric;
using namespace XmlRpc;

void remoteMethod(AWT::AlignParametric::Fixed::initialize)
{
   // Clear any existing state
   CommonMethods::reset();

   DEBUGMACRO("Creating an \"optimizer\"");
   eigOpt = FixedOptimizer::getInstance();
}

void remoteMethod(AWT::AlignParametric::Fixed::addMesh)
{
   const std::string filename = params[0];

   DEBUGMACRO("Adding surface from " << filename);

   MeshType::P modelMesh = VTKMeshLoader<T>::load(filename.c_str(), false);

   FixedOptimizer* sto = dynamic_cast<FixedOptimizer*>(*eigOpt);

   DEBUGMACRO("Adding to optimizer");
   SampledSurface::P surf = sto->addMesh(modelMesh);
   sampledSurfaces.push_back(surf);
   
   DEBUGMACRO("Done.");
}

void AWT::AlignParametric::Fixed::addMethods(AWT::RemoteMethods& methods)
{
   methods.push_back(RemoteMethod("fixed.initialize", initialize));
   methods.push_back(RemoteMethod("fixed.addMesh", addMesh));
}