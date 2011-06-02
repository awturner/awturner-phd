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
#include "SampledMeshData.h"

#include "SamplePoints.h"
#include "SampledTuples.h"



#include <map>

template <class T>
struct AWT::SampledMeshData<T>::D
{
   typedef std::map< std::string, typename SampledTuples<T>::P > SampledMap;
   SampledMap m_FaceData;
   SampledMap m_VertexData;

   typename MeshData<T>::P m_MeshData;
   SamplePoints::P m_SampPts;
};

template <class T>
AWT::SampledMeshData<T>::SampledMeshData( typename AWT::MeshData<T>::P data, AWT::SamplePoints::P sampPts )
: AWT::MeshData<T>::MeshData( data->getMesh( ) )
{
   m_D = new D;

   m_D->m_MeshData = data;

   m_D->m_SampPts = sampPts;
}

template <class T>
AWT::SampledMeshData<T>::~SampledMeshData( )
{
   delete m_D;
}

template <class T>
typename AWT::SampledMeshData<T>::P AWT::SampledMeshData<T>::getInstance( typename AWT::MeshData<T>::P data, AWT::SamplePoints::P sampPts )
{
   AUTOGETINSTANCE( AWT::SampledMeshData<T>, ( data, sampPts ) );
}

template <class T>
GETNAMEMACRO( AWT::SampledMeshData<T> );

template <class T>
void AWT::SampledMeshData<T>::addVertexData( const std::string& label, typename AWT::Tuples<T>::P data )
{
   DEBUGMACRO( "Not implemented for this class" );
}

template <class T>
void AWT::SampledMeshData<T>::addFaceData( const std::string& label, typename AWT::Tuples<T>::P data )
{
   DEBUGMACRO( "Not implemented for this class" );
}

template <class T>
bool AWT::SampledMeshData<T>::hasVertexData( const std::string& label ) const
{
   return m_D->m_MeshData->hasVertexData( label );
}

template <class T>
bool AWT::SampledMeshData<T>::hasFaceData( const std::string& label ) const
{
   return m_D->m_MeshData->hasFaceData( label );
}

template <class T>
typename AWT::Tuples<T>::P AWT::SampledMeshData<T>::getVertexData( const std::string& label )
{
   if ( m_D->m_MeshData->hasVertexData( label ) )
   {
      D::SampledMap::iterator it = m_D->m_VertexData.find( label );

      if ( it == m_D->m_VertexData.end( ) )
      {
         SampledTuples<T>::P sampTuples = SampledTuples<T>::getInstance( m_D->m_MeshData->getVertexData( label ), m_D->m_SampPts );

         m_D->m_VertexData[label] = SampledTuples<T>::P( sampTuples );

         return sampTuples;
      }
      else
      {
         return *it->second;
      }
   }
   else
   {
      D::SampledMap::iterator it = m_D->m_VertexData.find( label );

      m_D->m_VertexData.erase( it, it );

      return 0;
   }
}

template <class T>
typename AWT::Tuples<T>::P AWT::SampledMeshData<T>::getFaceData( const std::string& label )
{
   if ( m_D->m_MeshData->hasFaceData( label ) )
   {
      D::SampledMap::iterator it = m_D->m_FaceData.find( label );

      if ( it == m_D->m_FaceData.end( ) )
      {
         SampledTuples<T>::P sampTuples = SampledTuples<T>::getInstance( m_D->m_MeshData->getFaceData( label ), m_D->m_SampPts );

         m_D->m_FaceData[label] = SampledTuples<T>::P( sampTuples );

         return sampTuples;
      }
      else
      {
         return *it->second;
      }
   }
   else
   {
      D::SampledMap::iterator it = m_D->m_FaceData.find( label );

      m_D->m_FaceData.erase( it, it );

      return 0;
   }
}

template <class T>
void AWT::SampledMeshData<T>::removeVertexData( const std::string& label )
{
   DEBUGMACRO( "Not implemented for this class" );
}

template <class T>
void AWT::SampledMeshData<T>::removeFaceData( const std::string& label )
{
   DEBUGMACRO( "Not implemented for this class" );
}

template class AWT::SampledMeshData<double>;
template class AWT::SampledMeshData<float>;