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
#include "CoulombicOptimizer.h"

#include "Useful/MatlabEngine.h"

#include "Mesh/MeshImpl.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/VNLTuples.h"

using namespace AWT;
using namespace AWT::AlignParametric;

AWT::AlignParametric::CoulombicOptimizer::CoulombicOptimizer( MeshIndex N )
: ParticleOptimizer( N )
{
}

AWT::AlignParametric::CoulombicOptimizer::~CoulombicOptimizer( )
{
}

AWT::AlignParametric::CoulombicOptimizer::P AWT::AlignParametric::CoulombicOptimizer::getInstance( MeshIndex N )
{
   AUTOGETINSTANCE( AWT::AlignParametric::CoulombicOptimizer, ( N ) );
}

GETNAMEMACRO( AWT::AlignParametric::CoulombicOptimizer );

ParticleSurface::P AWT::AlignParametric::CoulombicOptimizer::addMesh( MeshType::P mesh, TuplesType::P particles )
{
   checkNumberOfPoints( particles );
   
   CoulombicParticleSurface::P surf = CoulombicParticleSurface::getInstance( mesh, particles, getNumberOfParticlesPerSurface() );
   this->_add( surf );

   return surf;
}
