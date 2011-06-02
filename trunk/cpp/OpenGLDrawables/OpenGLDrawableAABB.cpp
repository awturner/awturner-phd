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
#include "OpenGLDrawableAABB.h"



#include "ObjectEKDTree/AxisAlignedBoundingBox.h"

#include "OpenGLHeaders.h"

#include "DrawableFramework/DrawMaterial.h"

template <class T>
struct AWT::OpenGLDrawableAABB<T>::D
{
   double m_Bounds[6];
   typename AxisAlignedBoundingBox<T,3>::P m_Box;
   DrawMaterial::P m_Material;
};

template <class T>
AWT::OpenGLDrawableAABB<T>::OpenGLDrawableAABB( typename AxisAlignedBoundingBox<T,3>::P box )
{
   m_D = new D;

   m_D->m_Box = box;

   setTimeObject( box );
}

template <class T>
AWT::OpenGLDrawableAABB<T>::~OpenGLDrawableAABB( )
{
   delete m_D;
}

template <class T>
typename AWT::OpenGLDrawableAABB<T>::P AWT::OpenGLDrawableAABB<T>::getInstance( typename AWT::AxisAlignedBoundingBox<T,3>::P box )
{
   AUTOGETINSTANCE( AWT::OpenGLDrawableAABB<T>, ( box ) );
}

template <class T>
GETNAMEMACRO( AWT::OpenGLDrawableAABB<T> );

template <class T>
AWT::AxisAlignedBoundingBox<T,3>* AWT::OpenGLDrawableAABB<T>::getBox( )
{
   return *m_D->m_Box;
}

template <class T>
AWT::ModifiedTime AWT::OpenGLDrawableAABB<T>::getChildModifiedTime( )
{
   return getTimeObjectModified( );
}

template <class T>
void AWT::OpenGLDrawableAABB<T>::buildList( AWT::DrawContext::P context )
{
   T minVtx[3];
   T maxVtx[3];

   for ( unsigned char ax = 0; ax < 3; ++ax )
   {
      minVtx[ax] = m_D->m_Box->getMinimumBound( ax );
      maxVtx[ax] = m_D->m_Box->getMaximumBound( ax );
   }

   glBegin( GL_QUADS );

   glNormal3T<T>( 0, 0, -1 );
   glVertex3T( minVtx[0], maxVtx[1], minVtx[2] );
   glVertex3T( maxVtx[0], maxVtx[1], minVtx[2] );
   glVertex3T( maxVtx[0], minVtx[1], minVtx[2] );
   glVertex3T( minVtx[0], minVtx[1], minVtx[2] );

   glNormal3T<T>( 0, 0, 1 );
   glVertex3T( minVtx[0], minVtx[1], maxVtx[2] );
   glVertex3T( maxVtx[0], minVtx[1], maxVtx[2] );
   glVertex3T( maxVtx[0], maxVtx[1], maxVtx[2] );
   glVertex3T( minVtx[0], maxVtx[1], maxVtx[2] );

   glNormal3T<T>( 0, -1, 0 );
   glVertex3T( minVtx[0], minVtx[1], minVtx[2] );
   glVertex3T( maxVtx[0], minVtx[1], minVtx[2] );
   glVertex3T( maxVtx[0], minVtx[1], maxVtx[2] );
   glVertex3T( minVtx[0], minVtx[1], maxVtx[2] );

   glNormal3T<T>( 0, 1, 0 );
   glVertex3T( minVtx[0], maxVtx[1], maxVtx[2] );
   glVertex3T( maxVtx[0], maxVtx[1], maxVtx[2] );
   glVertex3T( maxVtx[0], maxVtx[1], minVtx[2] );
   glVertex3T( minVtx[0], maxVtx[1], minVtx[2] );

   glNormal3T<T>( -1, 0, 0 );
   glVertex3T( minVtx[0], minVtx[1], maxVtx[2] );
   glVertex3T( minVtx[0], maxVtx[1], maxVtx[2] );
   glVertex3T( minVtx[0], maxVtx[1], minVtx[2] );
   glVertex3T( minVtx[0], minVtx[1], minVtx[2] );

   glNormal3T<T>( 1, 0, 0 );
   glVertex3T( maxVtx[0], minVtx[1], minVtx[2] );
   glVertex3T( maxVtx[0], maxVtx[1], minVtx[2] );
   glVertex3T( maxVtx[0], maxVtx[1], maxVtx[2] );
   glVertex3T( maxVtx[0], minVtx[1], maxVtx[2] );
   
   glEnd( );

   modified( );
}

template <class T>
void AWT::OpenGLDrawableAABB<T>::updateBounds( )
{
   for ( int ax = 0; ax < 3; ++ax )
   {
      m_D->m_Bounds[2*ax + 0] = m_D->m_Box->getMinimumBound( ax );
      m_D->m_Bounds[2*ax + 1] = m_D->m_Box->getMaximumBound( ax );
   }
}

template <class T>
double AWT::OpenGLDrawableAABB<T>::getBoundImpl(unsigned int bound)
{
   return m_D->m_Bounds[bound];
}


template <class T>
AWT::DrawMaterial::P AWT::OpenGLDrawableAABB<T>::getMaterial( )
{
   return m_D->m_Material;
}

template <class T>
void AWT::OpenGLDrawableAABB<T>::setMaterial( AWT::DrawMaterial::P mat )
{
   m_D->m_Material = mat;
}

template class AWT::OpenGLDrawableAABB<double>;
template class AWT::OpenGLDrawableAABB<float>;