#ifndef __OPENGLDRAWABLEFACTORY_H__
#define __OPENGLDRAWABLEFACTORY_H__

#include "DrawableFramework/DrawableFactory.h"
#include "DrawableFramework/DrawContext.h"

namespace AWT
{
   template <class T>
   class OpenGLDrawableFactory : public DrawableFactory<T>
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableFactory> P;

   protected:
      OpenGLDrawableFactory( DrawContext::P ctx );
      virtual ~OpenGLDrawableFactory( );

   public:
      static  P getInstance( DrawContext::P ctx );
      virtual std::string getClassName( ) const;

      virtual DrawableWorld::P createWorld( );

      virtual DrawableAssembly::P createAssembly( );

      virtual DrawableLight::P createLight( Light::P light );
      
      virtual typename Transformation<T>::P createTransformation( Drawable::P drawable );

      virtual typename Transformation<T>::P createTransformation( Drawable::P drawable, typename Pose<T>::P pose );

      virtual typename DrawableMesh<T>::P createMesh( typename Mesh<T>::P mesh );

      virtual DrawMaterial::P createMaterial( const std::string& name );

      virtual DrawMaterial::P createMaterial( const float* ambient, const float* diffuse, const float* specular, const float shininess, const float opacity = 1.f );

      virtual DrawMaterial::P createTexture( const unsigned char* data, const unsigned int w, const unsigned int h, const bool lighting = true );

      virtual DrawColour::P createColour( const float* rgba, const bool lighting );

      virtual DrawColour::P createColour( const float r, const float g, const float b, const float a, const bool lighting );

      virtual DrawableImpl::P createAABB( typename AxisAlignedBoundingBox<T,3>::P box );

      virtual typename DrawablePoints<T>::P createPoints( typename Tuples<T>::P points );

      virtual typename DrawableQuiver<T>::P createQuiver( typename Tuples<T>::P points, typename Tuples<T>::P points2 );

      virtual typename DrawableMeshNormals<T>::P createMeshNormals( typename Mesh<T>::P mesh );

      virtual DrawAxis::P createAxis( const double scale );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __OPENGLDRAWABLEFACTORY_H__