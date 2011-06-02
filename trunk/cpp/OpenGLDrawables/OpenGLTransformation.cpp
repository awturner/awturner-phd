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
#include "OpenGLTransformation.h"

#include "Maths/Constants.h"

#include "Mesh/Pose.h"

#include "OpenGLHeaders.h"
#include <limits>


template <class T>
struct AWT::OpenGLTransformation<T>::D
{
   void refreshMatrix( )
   {
      typename Pose<T>::PoseMatrix mat = m_Pose->getMatrix( );

      for ( unsigned int c = 0; c < 4; ++c )
         for ( unsigned int r = 0; r < 4; ++r )
            m_Matrix[ 4*c + r ] = mat( r, c );

      PRINTVBL( m_Pose->getMatrix( ) );
      PRINTVEC( m_Matrix, 16 );
   }

   Drawable::P m_Drawable;
   bool      m_Visible;

   typename Pose<T>::P m_Pose;
   T         m_Matrix[16];

   DrawableAssembly* m_Parent;
};

template <class T>
AWT::OpenGLTransformation<T>::OpenGLTransformation( Drawable::P d, typename Pose<T>::P pose )
{
   m_D = new D;
   m_D->m_Parent = 0;

   m_D->m_Visible  = true;

   setDrawable( d );

   setPose( pose );
}

template <class T>
AWT::OpenGLTransformation<T>::~OpenGLTransformation( )
{
   delete m_D;
}

template <class T>
std::string AWT::OpenGLTransformation<T>::getClassName( ) const
{
   return "AWT::OpenGLTransformation";
}

template <class T>
typename AWT::OpenGLTransformation<T>::P AWT::OpenGLTransformation<T>::getInstance( Drawable::P d, typename Pose<T>::P pose )
{
   AUTOGETINSTANCE( OpenGLTransformation<T>, ( d, pose ) );
}

template <class T>
void AWT::OpenGLTransformation<T>::draw( AWT::DrawContext::P context, const bool transparentPass )
{
   if ( *m_D->m_Drawable == 0 )
      return;

   if ( m_D->m_Pose->getModifiedTime( ) > getModifiedTime( ) )
   {
      m_D->refreshMatrix( );
      modified( );
   }

   glPushMatrix( );

   glMultMatrixT( m_D->m_Matrix );

   m_D->m_Drawable->draw( context, transparentPass );

   glPopMatrix( );
}

template <class T>
void AWT::OpenGLTransformation<T>::setVisible( const bool v )
{
   if ( v != m_D->m_Visible )
   {
      m_D->m_Visible = v;
      modified( );
   }
}

template <class T>
bool AWT::OpenGLTransformation<T>::isVisible( ) const
{
   return m_D->m_Visible;
}

template <class T>
void AWT::OpenGLTransformation<T>::getBounds( double* out_Bounds )
{
   if ( *m_D->m_Drawable == 0 )
   {
      for ( int i = 0; i < 6; ++i )
         out_Bounds[i] = std::numeric_limits<double>::quiet_NaN( );
   }
   else
   {
      m_D->m_Drawable->getBounds( out_Bounds );
   }
}

template <class T>
void AWT::OpenGLTransformation<T>::setDrawable( AWT::Drawable::P d )
{
   if ( *d != *m_D->m_Drawable )
   {
      m_D->m_Drawable = d;
      modified( );
   }
}

template <class T>
AWT::Drawable::P AWT::OpenGLTransformation<T>::getDrawable( )
{
   return m_D->m_Drawable;
}

template <class T>
void AWT::OpenGLTransformation<T>::setPose( typename Pose<T>::P pose )
{
   m_D->m_Pose = pose;
   modified( );
}

template <class T>
typename AWT::Pose<T>::P AWT::OpenGLTransformation<T>::getPose( )
{
   return m_D->m_Pose;
}

template <class T>
void AWT::OpenGLTransformation<T>::setParent( AWT::DrawableAssembly* p )
{
   m_D->m_Parent = p;
   modified( );
}

template <class T>
AWT::DrawableAssembly* AWT::OpenGLTransformation<T>::getParent( )
{
   return m_D->m_Parent;
}

template class AWT::OpenGLTransformation<double>;
template class AWT::OpenGLTransformation<float>;
