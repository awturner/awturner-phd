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
#include "RandomMeshSampler.h"

#include "Mesh.h"
#include "SamplePoints.h"

template <class T>
struct AWT::RandomMeshSampler<T>::D
{
   MeshIndex m_NumberOfSamples;
};

template <class T>
AWT::RandomMeshSampler<T>::RandomMeshSampler( )
{
   m_D = new D;

   m_D->m_NumberOfSamples = 1000;
}

template <class T>
AWT::RandomMeshSampler<T>::~RandomMeshSampler( )
{
   delete m_D;
}

template <class T>
GETNAMEMACRO( AWT::RandomMeshSampler<T> );

template <class T>
typename AWT::RandomMeshSampler<T>::P AWT::RandomMeshSampler<T>::getInstance( )
{
   AUTOGETINSTANCE( RandomMeshSampler<T>, ( ) );
}

template <class T>
AWT::SamplePoints::P AWT::RandomMeshSampler<T>::sampleMesh( typename Mesh<T>::P mesh )
{
   AWT::SamplePoints::P samplePoints;

   if ( mesh->getNumberOfVertices( ) < m_D->m_NumberOfSamples )
   {
      samplePoints = AWT::SamplePoints::getInstance( mesh->getNumberOfVertices( ) );
      //for ( MeshIndex i = 0; i < mesh->getNumberOfVertices( ); ++i )
      MESH_EACHVERTEX( mesh, i )
         samplePoints->add( i );
   }
   else
   {
      samplePoints = AWT::SamplePoints::getInstance( m_D->m_NumberOfSamples );
      bool* used = new bool[ mesh->getNumberOfVertices( ) ];
      //for ( MeshIndex i = 0; i < mesh->getNumberOfVertices( ); ++i )
      MESH_EACHVERTEX( mesh, i )
         used[i] = false;

      for ( MeshIndex i = 0; i < m_D->m_NumberOfSamples; ++i )
      {
         MeshIndex ii;
         do
         {
            ii = ( rand( ) | ( rand( ) << 16 ) ) % mesh->getNumberOfVertices( );
         } while ( used[ii] );

         samplePoints->add( ii );
         used[ii] = true;
      }

      delete[] used;
   }

   return samplePoints;
}

template <class T>
void AWT::RandomMeshSampler<T>::setMaximumNumberOfSamples( const AWT::MeshIndex m )
{
   m_D->m_NumberOfSamples = m;
}

template <class T>
AWT::MeshIndex AWT::RandomMeshSampler<T>::getMaximumNumberOfSamples( ) const
{
   return m_D->m_NumberOfSamples;
}

template class AWT::RandomMeshSampler<double>;
template class AWT::RandomMeshSampler<float>;