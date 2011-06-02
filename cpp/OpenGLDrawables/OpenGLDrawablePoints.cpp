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
#include "OpenGLDrawablePoints.h"

#include "Mesh/Tuples.h"

#include "OpenGLHeaders.h"
#include "OpenGLSimpleColour.h"
#include <limits>

template <class T>
struct AWT::OpenGLDrawablePoints<T>::D
{
   bool isFinite( T vtx[3] )
   {
      for ( unsigned int i = 0; i < 3; ++i )
         if ( abs(vtx[i]) == std::numeric_limits<T>::infinity( ) || vtx[i] != vtx[i] )
            return false;

      return true;
   }

   DrawMaterial::P m_Material;
   typename Tuples<T>::P m_Points;
   typename Tuples<T>::P m_Normals;

   float  m_LineWidth;
   float  m_PointSize;
   bool   m_Closed;
   bool   m_Contiguous;
   double m_Bounds[6];
};

template <class T>
AWT::OpenGLDrawablePoints<T>::OpenGLDrawablePoints( typename Tuples<T>::P points )
{
   m_D = new D;

   m_D->m_PointSize = 3.f;

   m_D->m_LineWidth  = 0.f;
   m_D->m_Closed     = false;
   m_D->m_Contiguous = true;

   setData( points );

   updateBounds( );

   m_D->m_Material = OpenGLSimpleColour::getInstance( 1.f, 0.f, 0.f, 1.f, false );
}

template <class T>
AWT::OpenGLDrawablePoints<T>::~OpenGLDrawablePoints( )
{
   delete m_D;
}

template <class T>
typename AWT::OpenGLDrawablePoints<T>::P AWT::OpenGLDrawablePoints<T>::getInstance( typename Tuples<T>::P points )
{
   AUTOGETINSTANCE( AWT::OpenGLDrawablePoints<T>, ( points ) );
}

template <class T>
GETNAMEMACRO( AWT::OpenGLDrawablePoints<T> );

template <class T>
typename AWT::Tuples<T>::P AWT::OpenGLDrawablePoints<T>::getData( )
{
   return m_D->m_Points;
}

template <class T>
void AWT::OpenGLDrawablePoints<T>::setData( typename AWT::Tuples<T>::P points )
{
   m_D->m_Points = points;
   setTimeObject( points );
   modified( );
}

template <class T>
typename AWT::Tuples<T>::P AWT::OpenGLDrawablePoints<T>::getNormals( )
{
   return m_D->m_Normals;
}

template <class T>
void AWT::OpenGLDrawablePoints<T>::setNormals( typename AWT::Tuples<T>::P points )
{
   m_D->m_Normals = points;
}

template <class T>
float AWT::OpenGLDrawablePoints<T>::getPointSize( )
{
   return m_D->m_PointSize;
}

template <class T>
void AWT::OpenGLDrawablePoints<T>::setPointSize( const float s )
{
   m_D->m_PointSize = std::max( 0.f, s );
   modified( );
}

template <class T>
AWT::ModifiedTime AWT::OpenGLDrawablePoints<T>::getChildModifiedTime( )
{
   return getTimeObjectModified( );
}

template <class T>
void AWT::OpenGLDrawablePoints<T>::buildList( AWT::DrawContext::P context )
{
   T vtx[3];

   m_D->m_Material->prepare( );

   GLenum draws[2];
   unsigned int ndraws = 0;

   if ( m_D->m_PointSize != 0.f || m_D->m_Points->getNumberOfPoints( ) < 2 )
   {
      draws[ndraws++] = GL_POINTS;
      glPointSize( m_D->m_PointSize );
   }

   if ( m_D->m_LineWidth != 0.f && m_D->m_Points->getNumberOfPoints( ) >= 2  ) 
   {
      if ( m_D->m_Contiguous )
         draws[ndraws++] = ( m_D->m_Closed ) ? GL_LINE_LOOP : GL_LINE_STRIP;
      else
         draws[ndraws++] = GL_LINES;

      glLineWidth( m_D->m_LineWidth );
   }

   if ( *m_D->m_Normals == 0 )
      glDisable( GL_LIGHTING );

   T nml[3];
   for ( unsigned int d = 0; d < ndraws; ++d )
   {
      unsigned int cnt = 0;

      glBegin( draws[d] );

      for ( MeshIndex v = 0, vmax = m_D->m_Points->getNumberOfPoints( ); v < vmax; ++v )
      {
         m_D->m_Points->getPoint( v, vtx );

         if ( m_D->isFinite( vtx ) )
         {
            m_D->m_Material->tweak( DrawMaterial::TWEAK_VERTEX, v );

            if ( *m_D->m_Normals != 0 )
            {
               m_D->m_Normals->getPoint( v, nml );
               glNormal3Tv( nml );
            }

            glVertex3Tv( vtx );
            ++cnt;
         }
      }

      glEnd( );
   }

   m_D->m_Material->unprepare( );
}
   
template <class T>
void AWT::OpenGLDrawablePoints<T>::updateBounds( )
{
   for ( int ax = 0; ax < 3; ++ax )
   {
      m_D->m_Bounds[2*ax+0] =  std::numeric_limits<double>::infinity( );
      m_D->m_Bounds[2*ax+1] = -std::numeric_limits<double>::infinity( );
   }

   T vtx[4];
   m_D->m_Points->getPoint( 0, vtx );

   for ( MeshIndex v = 0, vmax = m_D->m_Points->getNumberOfPoints( ); v < vmax; ++v )
   {
      m_D->m_Points->getPoint( v, vtx );

      if ( m_D->isFinite( vtx ) )
      {
         for ( int ax = 0; ax < 3; ++ax )
         {
            m_D->m_Bounds[2*ax+0] = std::min<double>( m_D->m_Bounds[2*ax+0], vtx[ax] );
            m_D->m_Bounds[2*ax+1] = std::max<double>( m_D->m_Bounds[2*ax+1], vtx[ax] );
         }   
      }
   }
}

template <class T>
double AWT::OpenGLDrawablePoints<T>::getBoundImpl( unsigned int bound )
{
   return m_D->m_Bounds[bound];
}

template <class T>
AWT::DrawMaterial::P AWT::OpenGLDrawablePoints<T>::getMaterial( )
{
   return m_D->m_Material;
}

template <class T>
void AWT::OpenGLDrawablePoints<T>::setMaterial( DrawMaterial::P mat )
{
   m_D->m_Material = mat;
}

template <class T>
float AWT::OpenGLDrawablePoints<T>::getLineWidth( )
{
   return m_D->m_LineWidth;
}

template <class T>
void AWT::OpenGLDrawablePoints<T>::setLineWidth( const float v )
{
   if ( v != m_D->m_LineWidth )
   {
      m_D->m_LineWidth = v;
      modified( );
   }
}

template <class T>
bool AWT::OpenGLDrawablePoints<T>::isClosed( ) const
{
   return m_D->m_Closed;
}

template <class T>
void AWT::OpenGLDrawablePoints<T>::setClosed( const bool v )
{
   if ( v != m_D->m_Closed )
   {
      m_D->m_Closed = v;
      modified( );
   }
}

template <class T>
bool AWT::OpenGLDrawablePoints<T>::isContiguous( ) const
{
   return m_D->m_Contiguous;
}

template <class T>
void AWT::OpenGLDrawablePoints<T>::setContiguous( const bool v )
{
   if ( v != m_D->m_Contiguous )
   {
      m_D->m_Contiguous = v;
      modified( );
   }
}

template class AWT::OpenGLDrawablePoints<double>;
template class AWT::OpenGLDrawablePoints<float>;
