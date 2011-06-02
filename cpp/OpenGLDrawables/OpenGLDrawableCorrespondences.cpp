#include "OpenGLDrawableCorrespondences.h"


#include "Mesh/Mesh.h"

#include "NonRigidAlignment/CorrespondenceFinder.h"
#include "DrawableFramework/DrawMaterial.h"

#include "OpenGLHeaders.h"
#include <limits>



template <class T>
struct AWT::OpenGLDrawableCorrespondences<T>::D
{
   typename CorrespondenceFinder<T>::P m_CorrFinder;
   DrawMaterial::P m_Material;

   double m_Bounds[6];
};

template <class T>
AWT::OpenGLDrawableCorrespondences<T>::OpenGLDrawableCorrespondences( typename AWT::CorrespondenceFinder<T>::P corr )
{
   m_D = new D;

   setCorrespondences( corr );
}

template <class T>
AWT::OpenGLDrawableCorrespondences<T>::~OpenGLDrawableCorrespondences( )
{
   delete m_D;
}

template <class T>
typename AWT::OpenGLDrawableCorrespondences<T>::P AWT::OpenGLDrawableCorrespondences<T>::getInstance( typename CorrespondenceFinder<T>::P corr )
{
   AUTOGETINSTANCE( AWT::OpenGLDrawableCorrespondences<T>, ( corr ) );
}

template <class T>
GETNAMEMACRO( AWT::OpenGLDrawableCorrespondences<T> );

template <class T>
void AWT::OpenGLDrawableCorrespondences<T>::setCorrespondences(typename AWT::CorrespondenceFinder<T>::P corr)
{
   m_D->m_CorrFinder = corr;

   setTimeObject( corr );
}

template <class T>
AWT::ModifiedTime AWT::OpenGLDrawableCorrespondences<T>::getChildModifiedTime( )
{
   return getTimeObjectModified( );
}

template <class T>
void AWT::OpenGLDrawableCorrespondences<T>::buildList( AWT::DrawContext::P context )
{
   if ( *m_D->m_CorrFinder == 0 )
      return;
   
   T vtxA[3];
   T vtxB[3];
   MeshIndex idx;
   T mid[3];

   glDisable( GL_LIGHTING );
   glEnable( GL_COLOR_MATERIAL );

   glBegin( GL_LINES );
   for ( MeshIndex v = 0; v < m_D->m_CorrFinder->getNumberOfCorrespondences( ); ++v )
   {
      bool isValid = m_D->m_CorrFinder->getCorrespondence( v, vtxA, vtxB, idx );

      if ( !isValid )
         continue;

      for ( int i = 0; i < 3; ++i )
         mid[i] = ( vtxA[i] + vtxB[i] ) / 2;
      
      glColor3f( 1.f, 0.f, 0.f );
      glVertex3Tv<T>( vtxA );
      glVertex3Tv<T>( mid );

      glColor3f( 0.f, 1.f, 0.f );
      glVertex3Tv<T>( vtxB );
      glVertex3Tv<T>( mid );
   }
   glEnd( );
}

template <class T>
void AWT::OpenGLDrawableCorrespondences<T>::updateBounds( )
{
   m_D->m_Bounds[0] = m_D->m_Bounds[2] = m_D->m_Bounds[4] =  std::numeric_limits<double>::infinity( );
   m_D->m_Bounds[1] = m_D->m_Bounds[3] = m_D->m_Bounds[5] = -std::numeric_limits<double>::infinity( );

   T vtxA[3];
   T vtxB[3];
   MeshIndex idx;

   for ( MeshIndex v = 0; v < m_D->m_CorrFinder->getNumberOfCorrespondences( ); ++v )
   {
      bool isValid = m_D->m_CorrFinder->getCorrespondence( v, vtxA, vtxB, idx );
      if ( !isValid ) continue;

      updateBoundsForVertex( vtxA );
      updateBoundsForVertex( vtxB );
   }
}

template <class T>
void AWT::OpenGLDrawableCorrespondences<T>::updateBoundsForVertex(T *vtx)
{
   for ( int i = 0; i < 3; ++i )
   {
      m_D->m_Bounds[2*i+0] = std::min<double>( m_D->m_Bounds[2*i+0], vtx[i] );
      m_D->m_Bounds[2*i+1] = std::max<double>( m_D->m_Bounds[2*i+1], vtx[i] );
   }
}

template <class T>
double AWT::OpenGLDrawableCorrespondences<T>::getBoundImpl(unsigned int bound)
{
   return m_D->m_Bounds[bound];
}

template <class T>
AWT::DrawMaterial::P AWT::OpenGLDrawableCorrespondences<T>::getMaterial( )
{
   return m_D->m_Material;
}

template <class T>
void AWT::OpenGLDrawableCorrespondences<T>::setMaterial( AWT::DrawMaterial::P mat )
{
   m_D->m_Material = mat;
}

template class AWT::OpenGLDrawableCorrespondences<double>;
template class AWT::OpenGLDrawableCorrespondences<float>;