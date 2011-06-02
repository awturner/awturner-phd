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
#include "DrawableControlPoints.h"

#include "DrawableFramework/DrawableImpl.h"

#include "Mesh/MeshTypeDefs.h"

#include "NonRigidAlignment/ControlPoint.h"
#include "NonRigidAlignment/ControlPointSet.h"

#include "OpenGLHeaders.h"
#include <cmath>


#include "DrawableFramework/DrawMaterial.h"

template <class T>
struct AWT::DrawableControlPoints<T>::D
{
   DrawMaterial::P                  m_Material;
   typename ControlPointSet<T>::P   m_Cps;
   
   T m_DrawThresh;
};

template <class T>
AWT::DrawableControlPoints<T>::DrawableControlPoints( typename ControlPointSet<T>::P cps )
{
   m_D = new D;

   m_D->m_Cps = cps;

   m_D->m_DrawThresh = static_cast<T>( 0.1*0.1 );
}

template <class T>
AWT::DrawableControlPoints<T>::~DrawableControlPoints( )
{
   delete m_D;
}

template <class T>
typename AWT::DrawableControlPoints<T>::P AWT::DrawableControlPoints<T>::getInstance( typename ControlPointSet<T>::P cps )
{
   AUTOGETINSTANCE( DrawableControlPoints, ( cps ) );
}

template <class T>
std::string AWT::DrawableControlPoints<T>::getClassName( ) const
{
   return "AWT::DrawableControlPoints";
}

template <class T>
void AWT::DrawableControlPoints<T>::getBounds( double* out_Bounds )
{
   T pos[3];
   T val[3];

   for ( MeshIndex v = 0; v < m_D->m_Cps->getNumberOfObjects( ); ++v )
   {
      m_D->m_Cps->getControlPoint( v )->getPosition( pos );
      m_D->m_Cps->getControlPoint( v )->getValue( val );

      if ( v == 0 )
      {
         for ( int ax = 0; ax < 3; ++ax )
         {
            out_Bounds[2*ax+0] = std::min<double>( pos[ax], pos[ax]+val[ax] );
            out_Bounds[2*ax+1] = std::max<double>( pos[ax], pos[ax]+val[ax] );
         }
      }
      else
      {
         for ( int ax = 0; ax < 3; ++ax )
         {
            out_Bounds[2*ax+0] = std::min<double>( out_Bounds[2*ax+0], std::min<double>( pos[ax], pos[ax]+val[ax] ) );
            out_Bounds[2*ax+1] = std::max<double>( out_Bounds[2*ax+1], std::max<double>( pos[ax], pos[ax]+val[ax] ) );
         }
      }
   }
}

template <class T>
void AWT::DrawableControlPoints<T>::drawImpl( AWT::DrawContext::P context )
{
   glDisable( GL_LIGHTING );
   
   glPointSize( 3.f );
   
   T pos[3];
   T val[3];

   glBegin( GL_LINES );

   glColor3f( 0.f, 1.f, 1.f );
   for ( MeshIndex v = 0; v < m_D->m_Cps->getNumberOfObjects( ); ++v )
   {
      if ( m_D->m_Cps->getControlPoint( v )->isUsed( ) )
      {
         m_D->m_Cps->getControlPoint( v )->getPosition( pos );
         m_D->m_Cps->getControlPoint( v )->getValue( val );

         T len = 0;
         for ( int i = 0; i < 3; ++i )
         {
            len += val[i]*val[i];
            val[i] += pos[i];
         }

         if ( len > 1e-2 )
         {
            glVertex3Tv( pos );
            glVertex3Tv( val );
         }
      }
   }

   glEnd( );

   glBegin( GL_POINTS );

   glColor3f( 0.f, 0.f, 1.f );
   for ( MeshIndex v = 0; v < m_D->m_Cps->getNumberOfObjects( ); ++v )
   {
      if ( m_D->m_Cps->getControlPoint( v )->isUsed( ) )
      {
         m_D->m_Cps->getControlPoint( v )->getPosition( pos );
         m_D->m_Cps->getControlPoint( v )->getValue( val );

         T len = 0;
         for ( int i = 0; i < 3; ++i )
         {
            len += val[i]*val[i];
         }

         if ( len > m_D->m_DrawThresh )
         {
            glVertex3Tv( pos );
         }
      }
   }

   glEnd( );
}

template <class T>
void AWT::DrawableControlPoints<T>::setDrawThreshold( const T in_Val )
{
   m_D->m_DrawThresh = in_Val*in_Val;
}

template <class T>
T AWT::DrawableControlPoints<T>::getDrawThreshold( ) const
{
   return sqrt( m_D->m_DrawThresh );
}

template <class T>
AWT::DrawMaterial::P AWT::DrawableControlPoints<T>::getMaterial( )
{
   return m_D->m_Material;
}

template <class T>
void AWT::DrawableControlPoints<T>::setMaterial( AWT::DrawMaterial::P mat )
{
   m_D->m_Material = mat;
}

template class AWT::DrawableControlPoints<double>;
template class AWT::DrawableControlPoints<float>;
