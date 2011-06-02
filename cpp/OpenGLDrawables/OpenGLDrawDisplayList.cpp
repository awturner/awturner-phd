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
#include "OpenGLDrawDisplayList.h"

#include "DrawableFramework/DrawableImpl.h"

#include "Mesh/Mesh.h"

#include "OpenGLHeaders.h"

#include <limits>

struct AWT::OpenGLDrawDisplayList::D
{
   // These are things to do with the display lists
   GLuint        m_MeshList;
   bool          m_RecreateList;
   bool          m_First;
};

AWT::OpenGLDrawDisplayList::OpenGLDrawDisplayList( )
{
   //DEBUGMACRO("Created");
   m_D = new D;

   m_D->m_RecreateList = true;
   m_D->m_First        = true;
}

AWT::OpenGLDrawDisplayList::~OpenGLDrawDisplayList( )
{
   //DEBUGMACRO("Destroyed");
   if ( !m_D->m_First )
   {
      glDeleteLists( m_D->m_MeshList, 1 );
   }

   delete m_D;
}

void AWT::OpenGLDrawDisplayList::beforeCallList( AWT::DrawContext::P context )
{
   // Stub function so that you don't have to override
}


void AWT::OpenGLDrawDisplayList::afterCallList( AWT::DrawContext::P context )
{
   // Stub function so that you don't have to override
}


void AWT::OpenGLDrawDisplayList::getBounds( double* out_Bounds )
{
   if ( getChildModifiedTime( ) > getModifiedTime( ) || m_D->m_RecreateList )
   {
      updateBounds( );

      // Although we've modified the bounds, we don't want
      // to miss out on updating the display list
      m_D->m_RecreateList = true;
   }

   for ( unsigned int i = 0; i < 6; ++i )
      out_Bounds[i] = getBoundImpl( i );
}


void AWT::OpenGLDrawDisplayList::drawImpl( AWT::DrawContext::P context )
{
   // Mutex* mutex = AWT::Drawable::getMutex( );

   bool hasMutex = true; //mutex->tryAcquire( this );

   beforeCallList( context );

   // This checks to see if the list needs to be rebuilt
   if ( hasMutex && ( getChildModifiedTime( ) > getModifiedTime( ) || m_D->m_RecreateList ) )
   {
      if ( !m_D->m_First )
      {
         glDeleteLists( m_D->m_MeshList, 1 );
      }
      else
      {
         m_D->m_First = false;
      }

      m_D->m_MeshList = glGenLists( 1 );
      
      glNewList( m_D->m_MeshList, GL_COMPILE_AND_EXECUTE );

      buildList( context );

      glEndList( );

      modified( );
      m_D->m_RecreateList = false;
   }
   else
   {
      glCallList( m_D->m_MeshList );
   }

   afterCallList( context );
   
   if ( hasMutex )
   {
      //mutex->release( this );
   }
}


