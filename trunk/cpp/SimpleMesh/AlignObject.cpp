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
#include "AlignObject.h"

using namespace AWT::SimpleMesh;

AWT::SimpleMesh::AlignObject::AlignObject( const Index samples )
: nsamples( samples )
{
}

Vector AWT::SimpleMesh::AlignObject::getParameters( ) const
{
   return params;
}

void AWT::SimpleMesh::AlignObject::incrementParameters( const Vector& inc )
{
   setParameters( params + inc );
}

void AWT::SimpleMesh::AlignObject::setParameters( const Vector& params )
{
   this->params.update( params );
}

Mesh::P AWT::SimpleMesh::AlignObject::getMesh( )
{
   return mesh;
}

void AWT::SimpleMesh::AlignObject::resample( )
{
   samples.clear( );

   for ( Index i = 0; i < nsamples; ++i )
      samples.push_back( sampler->sample( ) );
}

PIWs& AWT::SimpleMesh::AlignObject::getSamples( )
{
   return samples;
}