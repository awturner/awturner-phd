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
#include "DeformationAccumulator.h"

#include "NonRigidAlignment.h"
#include "ControlPoint.h"
#include "ControlPointSet.h"

#include "Useful/PrintMacros.h"



template <class T>
struct AWT::DeformationAccumulator<T>::D
{
   T                                 m_TestPoint[3];
   T                                 m_Deformation[3];

   unsigned int                      m_NumberOfControlPoints;

   typename ControlPointSet<T>::P    m_Cps;
};

template <class T>
AWT::DeformationAccumulator<T>::DeformationAccumulator( typename ControlPointSet<T>::P cps )
{
   m_D = new D;

   m_D->m_Cps = cps;

   reset( );
}

template <class T>
AWT::DeformationAccumulator<T>::~DeformationAccumulator( )
{
   delete m_D;
}

template <class T>
typename AWT::DeformationAccumulator<T>::P AWT::DeformationAccumulator<T>::getInstance( typename ControlPointSet<T>::P cps )
{
   AUTOGETINSTANCE( DeformationAccumulator, ( cps ) );
}

template <class T>
std::string AWT::DeformationAccumulator<T>::getClassName() const
{
   return "AWT::DeformationAccumulator<T>";
}

template <class T>
void AWT::DeformationAccumulator<T>::reset( )
{
   for ( int i = 0; i < 3; ++i )
      m_D->m_Deformation[i] = 0;

   m_D->m_NumberOfControlPoints = 0;
}

template <class T>
unsigned int AWT::DeformationAccumulator<T>::getNumberOfControlPoints( ) const
{
   return m_D->m_NumberOfControlPoints;
}

template <class T>
void AWT::DeformationAccumulator<T>::controlPointFound( const AWT::MeshIndex controlIndex )
{
   // Need to calculate the contribution to the gradient that
   // moving this point in each of the K coordinate directions
   // will have

   ++m_D->m_NumberOfControlPoints;

   ControlPoint<T>::P cp = ( *m_D->m_Cps )->getControlPoint( controlIndex );

   T weight = cp->getInfluence( m_D->m_TestPoint );

   for ( int i = 0; i < 3; ++i )
   {
      m_D->m_Deformation[i] += weight * cp->getValue( i );
   }
}

template <class T>
void AWT::DeformationAccumulator<T>::getDeformedPoint( T* out_Point ) const
{
   for ( int i = 0; i < 3; ++i )
      out_Point[i] = m_D->m_TestPoint[i] + m_D->m_Deformation[i];
}

template <class T>
void AWT::DeformationAccumulator<T>::getDeformation( T* out_Point ) const
{
   for ( int i = 0; i < 3; ++i )
      out_Point[i] = m_D->m_Deformation[i];
}

template <class T>
void AWT::DeformationAccumulator<T>::getTestPoint( T* out_Point ) const
{
   for ( int i = 0; i < 3; ++i )
      out_Point[i] = m_D->m_TestPoint[i];
}

template <class T>
void AWT::DeformationAccumulator<T>::setTestPoint( const T* in_Point )
{
   for ( int i = 0; i < 3; ++i )
      m_D->m_TestPoint[i] = in_Point[i];

   modified( );
}

template class AWT::DeformationAccumulator<double>;
template class AWT::DeformationAccumulator<float>;
