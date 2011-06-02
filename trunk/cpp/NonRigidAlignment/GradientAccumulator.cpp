#include "GradientAccumulator.h"

#include "NonRigidAlignment.h"
#include "Mesh/TuplesImpl.h"
#include "ControlPoint.h"
#include "ControlPointSet.h"
#include "MeshAlignmentData.h"
#include "Mesh/Mesh.h"



template <class T>
struct AWT::GradientAccumulator<T>::D
{
   MeshIndex                m_FaceIndex;

   T                        m_MovingPoint[3];
   T                        m_FixedPoint[3];

   typename NonRigidAlignment<T>::P    m_Nra;
   typename Tuples<T>::P               m_Gradient;
};

template <class T>
AWT::GradientAccumulator<T>::GradientAccumulator( typename NonRigidAlignment<T>::P data )
{
   m_D = new D;

   m_D->m_Nra = data;

   T def[] = { 0, 0, 0 };
   m_D->m_Gradient = TuplesImpl<T>::getInstance( 3, def, data->getControlPoints( )->getNumberOfObjects( ) );

   for ( MeshIndex v = 0; v < data->getControlPoints( )->getNumberOfObjects( ); ++v )
      m_D->m_Gradient->setPoint( v, def );
}

template <class T>
AWT::GradientAccumulator<T>::~GradientAccumulator( )
{
   delete m_D;
}

template <class T>
typename AWT::GradientAccumulator<T>::P AWT::GradientAccumulator<T>::getInstance( typename AWT::NonRigidAlignment<T>::P nra )
{
   AUTOGETINSTANCE( GradientAccumulator, ( nra ) );
}

template <class T>
GETNAMEMACRO( AWT::GradientAccumulator<T> );

template <class T>
void AWT::GradientAccumulator<T>::controlPointFound( const AWT::MeshIndex controlIndex )
{
   // Need to calculate the contribution to the gradient that
   // moving this point in each of the 3 coordinate directions
   // will have

   ControlPoint<T>::P cp = m_D->m_Nra->getControlPoints( )->getControlPoint( controlIndex );

   Mesh<T>::P fixed         = m_D->m_Nra->getMeshAlignment( )->getFixedPoints( );
   Mesh<T>::P fixedOriginal = m_D->m_Nra->getMeshAlignment( )->getFixedPointsOriginal( );

   // Get the three vertices at the corners
   T pointA[3], pointAOriginal[3];
   T pointB[3], pointBOriginal[3];
   T pointC[3], pointCOriginal[3];

   fixed->getFace( m_D->m_FaceIndex, pointA, pointB, pointC );
   fixedOriginal->getFace( m_D->m_FaceIndex, pointAOriginal, pointBOriginal, pointCOriginal );

   // These are the weights on the jacobians of the three points
   // It's just a matrix inversion
   T det = pointA[0]*pointB[1]*pointC[2]-pointA[0]*pointC[1]*pointB[2]-pointA[1]*pointB[0]*pointC[2]+pointA[1]*pointC[0]*pointB[2]+pointA[2]*pointB[0]*pointC[1]-pointA[2]*pointC[0]*pointB[1];

   if ( det == 0 )
      return;

   T weights[3];
   weights[0] = ( ( pointB[1]*pointC[2]-pointC[1]*pointB[2])*m_D->m_FixedPoint[0] + (-pointB[0]*pointC[2]+pointC[0]*pointB[2])*m_D->m_FixedPoint[1] + ( pointB[0]*pointC[1]-pointC[0]*pointB[1])*m_D->m_FixedPoint[2] ) / det;
   weights[1] = ( (-pointA[1]*pointC[2]+pointC[1]*pointA[2])*m_D->m_FixedPoint[0] + ( pointA[0]*pointC[2]-pointC[0]*pointA[2])*m_D->m_FixedPoint[1] + (-pointA[0]*pointC[1]+pointC[0]*pointA[1])*m_D->m_FixedPoint[2] ) / det;
   weights[2] = ( ( pointA[1]*pointB[2]-pointB[1]*pointA[2])*m_D->m_FixedPoint[0] + (-pointA[0]*pointB[2]+pointB[0]*pointA[2])*m_D->m_FixedPoint[1] + ( pointA[0]*pointB[1]-pointB[0]*pointA[1])*m_D->m_FixedPoint[2] ) / det;

   // This is a weighted sum of the jacobians at each of the face vertices
   T jacScale = (
        cp->getInfluence( pointAOriginal ) * weights[0]
      + cp->getInfluence( pointBOriginal ) * weights[1]
      + cp->getInfluence( pointCOriginal ) * weights[2]
   );

   for ( int axis = 0; axis < 3; ++axis )
   {
      T delta    = jacScale * 2 * ( m_D->m_FixedPoint[axis] - m_D->m_MovingPoint[axis] );
      T newValue = m_D->m_Gradient->getPointElement( controlIndex, axis ) + delta;
      m_D->m_Gradient->setPointElement( controlIndex, axis, newValue );
   }

   modified( );
}

template <class T>
AWT::Tuples<T>* AWT::GradientAccumulator<T>::getGradient( ) const
{
   return *m_D->m_Gradient;
}

template <class T>
void AWT::GradientAccumulator<T>::getMovingPoint( T* out_Point ) const
{
   for ( int i = 0; i < 3; ++i )
      out_Point[i] = m_D->m_MovingPoint[i];
}

template <class T>
void AWT::GradientAccumulator<T>::getFixedPoint( T* out_Point ) const
{
   for ( int i = 0; i < 3; ++i )
      out_Point[i] = m_D->m_FixedPoint[i];
}

template <class T>
void AWT::GradientAccumulator<T>::setMovingPoint( const T* in_Point )
{
   for ( int i = 0; i < 3; ++i )
      m_D->m_MovingPoint[i] = in_Point[i];

   modified( );
}

template <class T>
void AWT::GradientAccumulator<T>::setFixedPoint( const T* in_Point )
{
   for ( int i = 0; i < 3; ++i )
      m_D->m_FixedPoint[i] = in_Point[i];

   modified( );
}

template <class T>
AWT::MeshIndex AWT::GradientAccumulator<T>::getFaceIndex( ) const
{
   return m_D->m_FaceIndex;
}

template <class T>
void AWT::GradientAccumulator<T>::setFaceIndex( const AWT::MeshIndex idx )
{
   m_D->m_FaceIndex = idx;
   modified( );
}

template class AWT::GradientAccumulator<double>;
template class AWT::GradientAccumulator<float>;
