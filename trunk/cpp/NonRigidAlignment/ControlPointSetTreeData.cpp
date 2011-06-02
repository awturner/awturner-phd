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
#include "ControlPointSetTreeData.h"

#include <vector>

#include "ControlPoint.h"

#include "Useful/PrintMacros.h"

template <class T>
struct AWT::ControlPointSetTreeData<T>::D
{
   std::vector<typename ControlPoint<T>::P>* m_ControlPoints;
};

template <class T>
AWT::ControlPointSetTreeData<T>::ControlPointSetTreeData( std::vector<typename AWT::ControlPoint<T>::P>* v )
{
   m_D = new D;
   m_D->m_ControlPoints = v;
}

template <class T>
AWT::ControlPointSetTreeData<T>::~ControlPointSetTreeData( )
{
   delete m_D;
}

template <class T>
typename AWT::ControlPointSetTreeData<T>::P AWT::ControlPointSetTreeData<T>::getInstance( std::vector<typename ControlPoint<T>::P>* vec )
{
   AUTOGETINSTANCE( ControlPointSetTreeData<T>, ( vec ) );
}

template <class T>
GETNAMEMACRO( AWT::ControlPointSetTreeData<T> );

template <class T>
AWT::OEKDTree::ObjectIndex AWT::ControlPointSetTreeData<T>::getNumberOfObjects( ) const
{
   return static_cast<AWT::OEKDTree::ObjectIndex>( m_D->m_ControlPoints->size( ) );
}

template <class T>
T AWT::ControlPointSetTreeData<T>::getValue( AWT::OEKDTree::ObjectIndex objectIndex, AWT::OEKDTree::AxisIndex axis ) const
{
   const ControlPoint<T>* cp = *((*m_D->m_ControlPoints)[objectIndex]);

   return cp->getPosition( axis );
}

template <class T>
T AWT::ControlPointSetTreeData<T>::getCoordinate( AWT::OEKDTree::ObjectIndex objectIndex, AWT::OEKDTree::AxisIndex axis ) const
{
   const ControlPoint<T>* cp = *((*m_D->m_ControlPoints)[objectIndex]);

   return cp->getPosition( axis );
}

template <class T>
void AWT::ControlPointSetTreeData<T>::getPosition( const AWT::OEKDTree::ObjectIndex objectIndex, T* pos ) const
{
}

template <class T>
T AWT::ControlPointSetTreeData<T>::getMinimumBound( AWT::OEKDTree::ObjectIndex objectIndex, AWT::OEKDTree::AxisIndex axis ) const
{
   const ControlPoint<T>* cp = *((*m_D->m_ControlPoints)[objectIndex]);

   return cp->getPosition( axis ) - 2*cp->getScale( );
}

template <class T>
T AWT::ControlPointSetTreeData<T>::getMaximumBound( AWT::OEKDTree::ObjectIndex objectIndex, AWT::OEKDTree::AxisIndex axis ) const
{
   const ControlPoint<T>* cp = *((*m_D->m_ControlPoints)[objectIndex]);

   return cp->getPosition( axis ) + 2*cp->getScale( );
}

template class AWT::ControlPointSetTreeData<double>;
template class AWT::ControlPointSetTreeData<float>;