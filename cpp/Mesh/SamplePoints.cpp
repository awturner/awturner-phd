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
#include "SamplePoints.h"

#include <vector>

#include "Useful/PrintMacros.h"

struct AWT::SamplePoints::D
{
   std::vector<AWT::MeshIndex> m_SamplePoints;
};

AWT::SamplePoints::SamplePoints( AWT::MeshIndex npoints )
{
   m_D = new D;

   m_D->m_SamplePoints.reserve( npoints );
}

AWT::SamplePoints::~SamplePoints( )
{
   delete m_D;
}

GETNAMEMACRO( AWT::SamplePoints );

AWT::SamplePoints::P AWT::SamplePoints::getInstance( AWT::MeshIndex npoints )
{
   AUTOGETINSTANCE( SamplePoints, (npoints) );
}

const AWT::MeshIndex AWT::SamplePoints::operator[]( AWT::MeshIndex i ) const
{
   return m_D->m_SamplePoints[i];
}

AWT::MeshIndex AWT::SamplePoints::getNumberOfSamples( ) const
{
   return static_cast<MeshIndex>( m_D->m_SamplePoints.size( ) );
}

AWT::MeshIndex AWT::SamplePoints::getSampleIndex( AWT::MeshIndex i ) const
{
   return m_D->m_SamplePoints[i];
}

void AWT::SamplePoints::ensureSize( AWT::MeshIndex i )
{
   m_D->m_SamplePoints.reserve( i );
   modified( );
}

void AWT::SamplePoints::add( AWT::MeshIndex i )
{
   m_D->m_SamplePoints.push_back( i );
   modified( );
}

void AWT::SamplePoints::clear( )
{
   m_D->m_SamplePoints.clear( );
   modified( );
}

