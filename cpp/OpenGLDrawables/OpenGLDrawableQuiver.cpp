#include "OpenGLDrawableQuiver.h"

#include "Useful/ColouredConsole.h"
#include "OpenGLHeaders.h"
#include <limits>
#include "Mesh/TuplesFunctions.h"

using namespace AWT;

template <class T>
struct AWT::OpenGLDrawableQuiver<T>::D
{
   typename Tuples<T>::P points;
   typename Tuples<T>::P points2;

   bool relative;
   float pointSize;
   float lineWidth;

   DrawMaterial::P material;

   double bounds[6];
};

template <class T>
AWT::OpenGLDrawableQuiver<T>::OpenGLDrawableQuiver( typename Tuples<T>::P points, typename Tuples<T>::P points2 )
{
   m_D = new D;

   m_D->points = points;
   m_D->points2 = points2;

   m_D->relative = true;

   m_D->pointSize = 3.f;
   m_D->lineWidth = 1.f;

   updateBounds();
}

template <class T>
AWT::OpenGLDrawableQuiver<T>::~OpenGLDrawableQuiver( )
{
   delete m_D;
}

template <class T>
typename AWT::OpenGLDrawableQuiver<T>::P AWT::OpenGLDrawableQuiver<T>::getInstance( typename Tuples<T>::P points, typename Tuples<T>::P points2 )
{
   AUTOGETINSTANCE( AWT::OpenGLDrawableQuiver<T>, ( points, points2 ) );
}

template <class T>
GETNAMEMACRO( AWT::OpenGLDrawableQuiver<T> );

template <class T>
typename Tuples<T>::P AWT::OpenGLDrawableQuiver<T>::getData( )
{
   return m_D->points;
}

template <class T>
void AWT::OpenGLDrawableQuiver<T>::setData( typename Tuples<T>::P points )
{
   if ( *points != *m_D->points )
   {
      m_D->points = points;
      modified();
   }
}

template <class T>
typename Tuples<T>::P AWT::OpenGLDrawableQuiver<T>::getData2( )
{
   return m_D->points2;
}

template <class T>
void AWT::OpenGLDrawableQuiver<T>::setData2( typename Tuples<T>::P points2 )
{
   if ( *points2 != *m_D->points2 )
   {
      m_D->points2 = points2;
      modified();
   }
}

template <class T>
bool AWT::OpenGLDrawableQuiver<T>::isRelative( ) const
{
   return m_D->relative;
}

template <class T>
void AWT::OpenGLDrawableQuiver<T>::setRelative( const bool v )
{
   if ( v != m_D->relative )
   {
      m_D->relative = v;
      modified();
   }
}

template <class T>
float AWT::OpenGLDrawableQuiver<T>::getPointSize( )
{
   return m_D->pointSize;
}

template <class T>
void AWT::OpenGLDrawableQuiver<T>::setPointSize( const float v )
{
   if ( v != m_D->pointSize )
   {
      m_D->pointSize = v;
      modified();
   }
}

template <class T>
float AWT::OpenGLDrawableQuiver<T>::getLineWidth( )
{
   return m_D->lineWidth;
}

template <class T>
void AWT::OpenGLDrawableQuiver<T>::setLineWidth( const float v )
{
   if ( v != m_D->lineWidth )
   {
      m_D->lineWidth = v;
      modified();
   }
}

template <class T>
DrawMaterial::P AWT::OpenGLDrawableQuiver<T>::getMaterial( )
{
   return m_D->material;
}

template <class T>
void AWT::OpenGLDrawableQuiver<T>::setMaterial( DrawMaterial::P mat )
{
   if ( *m_D->material != *mat )
   {
      m_D->material = mat;
      modified();
   }
}

template <class T>
ModifiedTime AWT::OpenGLDrawableQuiver<T>::getChildModifiedTime( )
{
   return getTimeObjectModified( );

   return std::max<ModifiedTime>( 
      m_D->points->getModifiedTime( ),
      m_D->points2->getModifiedTime( )
   );
}

template <class T>
void AWT::OpenGLDrawableQuiver<T>::buildList( DrawContext::P context )
{
   GLenum draws[2];
   unsigned int ndraws = 0;

   if ( m_D->lineWidth != 0.f )
   {
      draws[ndraws++] = GL_LINES;
      glLineWidth( m_D->lineWidth );
   }

   if ( m_D->pointSize != 0.f )
   {
      draws[ndraws++] = GL_POINTS;
      glPointSize( m_D->pointSize );
   }

   m_D->material->prepare( );

   glDisable( GL_LIGHTING );

   T vtx[3];
   T vtx2[3];

   MeshIndex p = std::min<MeshIndex>( m_D->points->getNumberOfPoints(), m_D->points2->getNumberOfPoints() );

   for ( unsigned int d = 0; d < ndraws; ++d )
   {
      glBegin( draws[d] );

      for ( MeshIndex v = 0; v < p; ++v )
      {
         m_D->points->getPoint( v, vtx );
         m_D->points2->getPoint( v, vtx2 );

         // If this is a relative quiver, need to add on the first poitn
         if ( m_D->relative )
         {
            for ( MeshIndex ax = 0; ax < 3; ++ax )
               vtx2[ax] += vtx[ax];
         }

         m_D->material->tweak( DrawMaterial::TWEAK_VERTEX, v );

         glVertex3Tv( vtx );
         glVertex3Tv( vtx2 );
      }

      glEnd( );
   }

   m_D->material->unprepare( );
}

template <class T>
void AWT::OpenGLDrawableQuiver<T>::updateBounds( )
{
   double bounds[8], bounds2[8];
   TuplesFunctions<T>::calculateBounds( m_D->points, bounds );
   TuplesFunctions<T>::calculateBounds( m_D->points2, bounds2 );

   m_D->bounds[0] = m_D->bounds[2] = m_D->bounds[4] =  std::numeric_limits<double>::infinity();
   m_D->bounds[1] = m_D->bounds[3] = m_D->bounds[5] = -std::numeric_limits<double>::infinity();
   TuplesFunctions<T>::expandBounds( bounds, m_D->bounds );
   TuplesFunctions<T>::expandBounds( bounds2, m_D->bounds );
}

template <class T>
double AWT::OpenGLDrawableQuiver<T>::getBoundImpl( unsigned int bound )
{
   return m_D->bounds[bound];
}

template class AWT::OpenGLDrawableQuiver<double>;
template class AWT::OpenGLDrawableQuiver<float>;