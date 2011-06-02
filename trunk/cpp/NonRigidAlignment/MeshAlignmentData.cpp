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
#include "MeshAlignmentData.h"

#include "Mesh/Mesh.h"
#include "MeshAlignmentDataListener.h"
#include "Mesh/MeshImpl.h"



template <class T>
struct AWT::MeshAlignmentData<T>::D
{
   std::vector< typename MeshAlignmentDataListener<T>::P > m_Listeners;
   //CompositeMeshAlignmentDataListener<T> m_Listeners;

   typename Mesh<T>::P m_MovingPoints;
   typename Mesh<T>::P m_FixedPoints;

   typename Mesh<T>::P m_MovingPointsDeformed;
   typename Mesh<T>::P m_FixedPointsDeformed;
};

template <class T>
AWT::MeshAlignmentData<T>::MeshAlignmentData( ) 
{ 
   m_D = new D;
}

template <class T>
AWT::MeshAlignmentData<T>::~MeshAlignmentData( )
{
   delete m_D;
}

template <class T>
typename AWT::MeshAlignmentData<T>::P AWT::MeshAlignmentData<T>::getInstance( )
{
   AUTOGETINSTANCE( MeshAlignmentData<T>, ( ) );
}

template <class T>
GETNAMEMACRO( AWT::MeshAlignmentData<T> );

template <class T>
typename AWT::Mesh<T>::P AWT::MeshAlignmentData<T>::getMovingPointsOriginal( ) const
{
   return *m_D->m_MovingPoints;
}

template <class T>
typename AWT::Mesh<T>::P AWT::MeshAlignmentData<T>::getFixedPointsOriginal( ) const
{
   return *m_D->m_FixedPoints;
}

template <class T>
typename AWT::Mesh<T>::P AWT::MeshAlignmentData<T>::getMovingPoints( ) const
{
   return *m_D->m_MovingPointsDeformed;
}

template <class T>
typename AWT::Mesh<T>::P AWT::MeshAlignmentData<T>::getFixedPoints( ) const
{
   return *m_D->m_FixedPointsDeformed;
}

template <class T>
void AWT::MeshAlignmentData<T>::setMovingPoints( typename AWT::Mesh<T>::P movingPoints )
{
   m_D->m_MovingPoints = movingPoints;

   // This is making a copy of the data
   m_D->m_MovingPointsDeformed = MeshImpl<T>::getInstance( m_D->m_MovingPoints );

   {
      for ( std::vector< typename MeshAlignmentDataListener<T>::P >::iterator it = m_D->m_Listeners.begin( );
            it != m_D->m_Listeners.end( );
            ++it)
      {
         (*it).getData( )->movingPointsSet( this );
      }
   }
   //m_D->m_Listeners.execute( this, &MeshAlignmentDataListener<T>::movingPointsSet );
}

template <class T>
void AWT::MeshAlignmentData<T>::setFixedPoints( typename AWT::Mesh<T>::P fixedPoints )
{
   m_D->m_FixedPoints = fixedPoints;

   // This is making a copy of the data
   m_D->m_FixedPointsDeformed = MeshImpl<T>::getInstance( m_D->m_FixedPoints );

   {
      for ( std::vector< typename MeshAlignmentDataListener<T>::P >::iterator it = m_D->m_Listeners.begin( );
            it != m_D->m_Listeners.end( );
            ++it)
      {
         (*it).getData( )->fixedPointsSet( this );
      }
   }
   //m_D->m_Listeners.execute( this, &MeshAlignmentDataListener<T>::fixedPointsSet );
}

template <class T>
void AWT::MeshAlignmentData<T>::addListener( typename MeshAlignmentDataListener<T>::P madl )
{
   m_D->m_Listeners.push_back( madl );
}

template <class T>
void AWT::MeshAlignmentData<T>::removeListener( typename MeshAlignmentDataListener<T>::P list )
{
   std::vector< typename MeshAlignmentDataListener<T>::P >::iterator iter = m_D->m_Listeners.begin( );
   
   while ( iter != m_D->m_Listeners.end( ) )
   {
      if ( (*iter).getDataConst( ) == *list )
         m_D->m_Listeners.erase( iter );
      else
         ++iter;
   }
   
}

template class AWT::MeshAlignmentData<double>;
template class AWT::MeshAlignmentData<float>;
