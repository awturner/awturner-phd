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
#include "MeshData.h"

#include <map>
#include "Mesh.h"
#include "Tuples.h"


#include "Useful/Exception.h"

template <class T>
struct AWT::MeshData<T>::D
{
   std::map<std::string, typename Tuples<T>::P > m_VertexData;
   std::map<std::string, typename Tuples<T>::P > m_FaceData;

   Mesh<T>* m_Mesh;
};

template <class T>
AWT::MeshData<T>::MeshData( Mesh<T>* mesh )
{
   m_D = new D;

   m_D->m_Mesh = mesh;
}

template <class T>
AWT::MeshData<T>::~MeshData( )
{
   // Don't delete m_Mesh, we're just keeping it for lookup
   delete m_D;
}

template <class T>
typename AWT::MeshData<T>::P AWT::MeshData<T>::getInstance( Mesh<T>* mesh )
{
   AUTOGETINSTANCE( AWT::MeshData<T>, ( mesh ) );
}

template <class T>
GETNAMEMACRO( AWT::MeshData<T> );

template <class T>
void AWT::MeshData<T>::addVertexData( const std::string& label, typename AWT::Tuples<T>::P data )
{
   if ( data->getNumberOfPoints( ) != m_D->m_Mesh->getNumberOfVertices( ) )
      AWTEXCEPTIONTHROW( "Incorrect number of points!" );

   m_D->m_VertexData[label] = data;
}

template <class T>
void AWT::MeshData<T>::addFaceData( const std::string& label, typename AWT::Tuples<T>::P data )
{
   if ( data->getNumberOfPoints( ) != m_D->m_Mesh->getNumberOfFaces( ) )
      AWTEXCEPTIONTHROW( "Incorrect number of points!" );

   m_D->m_FaceData[label] = data;
}

template <class T>
bool AWT::MeshData<T>::hasVertexData( const std::string& label ) const
{
   return m_D->m_VertexData.count( label ) != 0;
}

template <class T>
bool AWT::MeshData<T>::hasFaceData( const std::string& label ) const
{
   return m_D->m_FaceData.count( label ) != 0;
}

template <class T>
typename AWT::Tuples<T>::P AWT::MeshData<T>::getVertexData( const std::string& label )
{
   std::map<std::string, typename Tuples<T>::P >::iterator f = m_D->m_VertexData.find( label );

   if ( f != m_D->m_VertexData.end( ) )
   {
      return (*f).second;
   }
   else
   {
      return 0;
   }
}

template <class T>
typename AWT::Tuples<T>::P AWT::MeshData<T>::getFaceData( const std::string& label )
{
   std::map<std::string, typename Tuples<T>::P >::iterator f = m_D->m_FaceData.find( label );

   if ( f != m_D->m_FaceData.end( ) )
   {
      return (*f).second;
   }
   else
   {
      return 0;
   }
}

template <class T>
void AWT::MeshData<T>::removeVertexData( const std::string& label )
{
   m_D->m_VertexData.erase( label );
}

template <class T>
void AWT::MeshData<T>::removeFaceData( const std::string& label )
{
   m_D->m_FaceData.erase( label );
}

template <class T>
typename AWT::Mesh<T>* AWT::MeshData<T>::getMesh( )
{
   return m_D->m_Mesh;
}

template class AWT::MeshData<double>;
template class AWT::MeshData<float>;
