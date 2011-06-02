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
#include "ControlPointSearcher.h"

#include "ObjectEKDTree/OEKDSearcher.h"
#include "ObjectEKDTree/OEKDTreeBranch.h"

#include "ControlPointSetTreeData.h"
#include "ControlPointSearcherCallback.h"



template <class T>
struct AWT::ControlPointSearcher<T>::D
{
   T                                m_TestPoint[3];
   MeshIndex                        m_TestPointIndex;

   typename ControlPointSearcherCallback<T>::P m_Callback;

   typename ControlPointSetTreeData<T>::P data;   
};

template <class T>
AWT::ControlPointSearcher<T>::ControlPointSearcher( )
{
   m_D = new D;
}

template <class T>
AWT::ControlPointSearcher<T>::~ControlPointSearcher( )
{
   delete m_D;
}

template <class T>
typename AWT::ControlPointSearcher<T>::P AWT::ControlPointSearcher<T>::getInstance( )
{
   AUTOGETINSTANCE( ControlPointSearcher<T>, ( ) );
}

template <class T>
GETNAMEMACRO( AWT::ControlPointSearcher<T> );

template <class T>
void AWT::ControlPointSearcher<T>::setCallback( typename ControlPointSearcherCallback<T>::P callback )
{
   m_D->m_Callback = callback;
}

template <class T>
void AWT::ControlPointSearcher<T>::calculateBoxDistanceBounds2( typename OEKDTree::OEKDTreeBranch<T,3>* in_Branch, OEKDTree::SqDistBounds<T>& bounds  ) const
{
   boxChecked();

   int ax;
   for ( ax = 0; ax < 3; ++ax )
   {
      if ( in_Branch->getMinimumBound( ax ) > m_D->m_TestPoint[ax] )
         break;

      if ( in_Branch->getMaximumBound( ax ) < m_D->m_TestPoint[ax] )
         break;
   }

   bounds.lower = ( ax == 3 ) ? 0 : std::numeric_limits<T>::infinity( );
   bounds.upper = std::numeric_limits<T>::infinity( );
}

template <class T>
void AWT::ControlPointSearcher<T>::checkObject( const int in_Index )
{
   objectChecked();
   
   for ( int ax = 0; ax < 3; ++ax )
   {
      // Possible equality because influence is zero on the boundary

      if ( m_D->data->getMinimumBound( in_Index, ax ) >= m_D->m_TestPoint[ax] )
         return;

      if ( m_D->data->getMaximumBound( in_Index, ax ) <= m_D->m_TestPoint[ax] )
         return;
   }
   // We now know that the test point is inside the control point's domain of influence...

   if ( *m_D->m_Callback != 0 )
      m_D->m_Callback->controlPointFound( in_Index );
}

template <class T>
bool AWT::ControlPointSearcher<T>::shouldCheck( AWT::OEKDTree::OEKDTreeBranch<T,3> *branch, const AWT::OEKDTree::SqDistBounds<T> &bounds) const
{
   return bounds.lower == 0;
}

template <class T>
void AWT::ControlPointSearcher<T>::setTestPoint( const MeshIndex in_Index, const T* in_TestPoint  )
{
   for ( int i = 0; i < 3; i++ )
      m_D->m_TestPoint[i] = in_TestPoint[i];

   m_D->m_TestPointIndex = in_Index;
}

template <class T>
void AWT::ControlPointSearcher<T>::getTestPoint( T* out_TestPoint ) const
{
   for ( int i = 0; i < 3; i++ )
      out_TestPoint[i] = m_D->m_TestPoint[i];
}

template <class T>
typename AWT::ControlPointSetTreeData<T>::P AWT::ControlPointSearcher<T>::getData( )
{
   return m_D->data;
}

template <class T>
void AWT::ControlPointSearcher<T>::setData( typename AWT::OEKDTree::OEKDTreeData<T,3>::P data )
{
   m_D->data = checkType<AWT::OEKDTree::OEKDTreeData<T,3>,ControlPointSetTreeData<T>>( data );
   modified();
}

template class AWT::ControlPointSearcher<double>;
template class AWT::ControlPointSearcher<float>;