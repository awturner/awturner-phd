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


