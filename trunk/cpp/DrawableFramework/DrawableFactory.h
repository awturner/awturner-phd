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