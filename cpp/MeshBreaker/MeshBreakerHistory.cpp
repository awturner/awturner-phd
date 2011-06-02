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
#include "MeshBreakerHistory.h"

template <class T>
struct AWT::MeshBreakerHistory<T>::D
{
   typename AWT::MeshBreaker<T>::Symbol m_Symbol;
   AWT::MeshIndex m_F;
   AWT::MeshIndex m_G;
   AWT::MeshIndex m_DistalVertex;
   AWT::MeshIndex m_LeftNeighbour;
   AWT::MeshIndex m_RightNeighbour;
};

template <class T>
AWT::MeshBreakerHistory<T>::MeshBreakerHistory( const typename AWT::MeshBreaker<T>::Symbol sym, const AWT::MeshIndex f, const AWT::MeshIndex g, const AWT::MeshIndex distVert, const AWT::MeshIndex lneigh, const AWT::MeshIndex rneigh  )
{
   m_D = new D;

   m_D->m_Symbol = sym;
   m_D->m_F = f;
   m_D->m_G = g;
   m_D->m_DistalVertex = distVert;
   m_D->m_LeftNeighbour = lneigh;
   m_D->m_RightNeighbour = rneigh;
}

template <class T>
AWT::MeshBreakerHistory<T>::~MeshBreakerHistory( )
{
   delete m_D;
}

template <class T>
typename AWT::MeshBreakerHistory<T>::P AWT::MeshBreakerHistory<T>::getInstance( const typename AWT::MeshBreaker<T>::Symbol sym, const AWT::MeshIndex f, const AWT::MeshIndex g, const AWT::MeshIndex distVert, const AWT::MeshIndex lneigh, const AWT::MeshIndex rneigh )
{
   AUTOGETINSTANCE( AWT::MeshBreakerHistory<T>, ( sym, f, g, distVert, lneigh, rneigh ) );
}

template <class T>
GETNAMEMACRO( AWT::MeshBreakerHistory<T> );

template <class T>
typename AWT::MeshBreaker<T>::Symbol AWT::MeshBreakerHistory<T>::getSymbol( ) const
{
   return m_D->m_Symbol;
}

template <class T>
AWT::MeshIndex AWT::MeshBreakerHistory<T>::getF( ) const
{
   return m_D->m_F;
}

template <class T>
AWT::MeshIndex AWT::MeshBreakerHistory<T>::getG( ) const
{
   return m_D->m_G;
}

template <class T>
AWT::MeshIndex AWT::MeshBreakerHistory<T>::getDistalVertex( ) const
{
   return m_D->m_DistalVertex;
}

template <class T>
AWT::MeshIndex AWT::MeshBreakerHistory<T>::getLeftNeighbour( ) const
{
   return m_D->m_LeftNeighbour;
}

template <class T>
AWT::MeshIndex AWT::MeshBreakerHistory<T>::getRightNeighbour( ) const
{
   return m_D->m_RightNeighbour;
}


template class AWT::MeshBreakerHistory<double>;
template class AWT::MeshBreakerHistory<float>;