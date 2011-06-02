#ifndef __DRAWABLEFACTORY_H__
#define __DRAWABLEFACTORY_H__

#include "ObjectManagement/ManagedObject.h"

#include "DrawAxis.h"
#include "DrawColour.h"
#include "DrawMaterial.h"
#include "DrawableAssembly.h"
#include "DrawableImpl.h"
#include "DrawableLight.h"
#include "DrawableMesh.h"
#include "DrawableMeshNormals.h"
#include "DrawablePoints.h"
#include "DrawableQuiver.h"
#include "DrawableWorld.h"
#include "Transformation.h"

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"
#include "ObjectEKDTree/AxisAlignedBoundingBox.h"

namespace AWT
{
   template <class T>
   class DrawableFactory : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<DrawableFactory> P;

   public:
      virtual DrawableWorld::P                   createWorld( ) = 0;

      virtual DrawableLight::P                   createLight( Light::P light ) = 0;

      virtual typename DrawableAssembly::P       createAssembly( ) = 0;

      virtual typename DrawableMesh<T>::P        createMesh( typename Mesh<T>::P mesh ) = 0;

      virtual typename DrawableMeshNormals<T>::P createMeshNormals( typename Mesh<T>::P mesh ) = 0;

      virtual typename Transformation<T>::P      createTransformation( Drawable::P drawable ) = 0;

      virtual typename Transformation<T>::P      createTransformation( Drawable::P drawable, typename Pose<T>::P pose ) = 0;

      virtual DrawMaterial::P                    createMaterial( const std::string& name ) = 0;

      virtual DrawMaterial::P                    createMaterial( const float* ambient, const float* diffuse, const float* specular, const float shininess, const float opacity = 1.f ) = 0;

      virtual DrawMaterial::P                    createTexture( const unsigned char* data, const unsigned int w, const unsigned int h, const bool lighting = true ) = 0;

      virtual DrawColour::P                      createColour( const float r, const float g, const float b, const float a, const bool lighting ) = 0;

      virtual DrawColour::P                      createColour( const float* rgba, const bool lighting ) = 0;

      virtual DrawableImpl::P                    createAABB( typename AxisAlignedBoundingBox<T,3>::P box ) = 0;

      virtual typename DrawablePoints<T>::P      createPoints( typename Tuples<T>::P points ) = 0;

      virtual typename DrawableQuiver<T>::P      createQuiver( typename Tuples<T>::P points, typename Tuples<T>::P points2 ) = 0;

      virtual DrawAxis::P                        createAxis( const double scale ) = 0;
   };
}

#endif // __DRAWABLEFACTORY_H__