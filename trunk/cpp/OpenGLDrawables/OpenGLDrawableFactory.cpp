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
#include "OpenGLDrawableFactory.h"


#include "DrawableFramework/Light.h"

#include "Mesh/Pose.h"

#include "OpenGLDrawAxis.h"
#include "OpenGLDrawableAABB.h"
#include "DrawableFramework/DrawableAssembly.h"
#include "OpenGLDrawableLight.h"
#include "OpenGLDrawableMesh.h"
#include "OpenGLDrawableMeshNormals.h"
#include "OpenGLDrawablePoints.h"
#include "OpenGLDrawableQuiver.h"
#include "OpenGLDrawableWorld.h"
#include "OpenGLKilgardMaterial.h"
#include "OpenGLMaterial.h"
#include "OpenGLSimpleColour.h"
#include "OpenGLTexture.h"
#include "OpenGLTransformation.h"

template <class T>
struct AWT::OpenGLDrawableFactory<T>::D
{
   DrawContext::P ctx;
};

template <class T>
AWT::OpenGLDrawableFactory<T>::OpenGLDrawableFactory( DrawContext::P ctx )
{
   m_D = new D;

   m_D->ctx = ctx;
}

template <class T>
AWT::OpenGLDrawableFactory<T>::~OpenGLDrawableFactory( )
{
   delete m_D;
}

template <class T>
typename AWT::OpenGLDrawableFactory<T>::P AWT::OpenGLDrawableFactory<T>::getInstance( DrawContext::P ctx )
{
   AUTOGETINSTANCE( AWT::OpenGLDrawableFactory<T>, ( ctx ) );
}

template <class T>
GETNAMEMACRO( AWT::OpenGLDrawableFactory<T> );

template <class T>
AWT::DrawableWorld::P AWT::OpenGLDrawableFactory<T>::createWorld( )
{
   return OpenGLDrawableWorld::getInstance( m_D->ctx );
}

template <class T>
AWT::DrawableAssembly::P AWT::OpenGLDrawableFactory<T>::createAssembly( )
{
   return DrawableAssembly::getInstance( );
}

template <class T>
AWT::DrawableLight::P AWT::OpenGLDrawableFactory<T>::createLight( AWT::Light::P light )
{
   return OpenGLDrawableLight::getInstance( light );
}

template <class T>
typename AWT::Transformation<T>::P AWT::OpenGLDrawableFactory<T>::createTransformation( AWT::Drawable::P drawable )
{
   return OpenGLTransformation<T>::getInstance( drawable, Pose<T>::getInstance( ) );
}

template <class T>
typename AWT::Transformation<T>::P AWT::OpenGLDrawableFactory<T>::createTransformation( AWT::Drawable::P drawable, typename AWT::Pose<T>::P pose )
{
   return OpenGLTransformation<T>::getInstance( drawable, pose );
}

template <class T>
typename AWT::DrawableMesh<T>::P AWT::OpenGLDrawableFactory<T>::createMesh( typename AWT::Mesh<T>::P mesh )
{
   return OpenGLDrawableMesh<T>::getInstance( mesh );
}

template <class T>
AWT::DrawMaterial::P AWT::OpenGLDrawableFactory<T>::createMaterial( const std::string& name )
{
   return OpenGLKilgardMaterial::getInstance( name );
}

template <class T>
AWT::DrawColour::P AWT::OpenGLDrawableFactory<T>::createColour( const float* rgba, const bool lighting )
{
   return OpenGLSimpleColour::getInstance( rgba[0], rgba[1], rgba[2], rgba[3], lighting );
}

template <class T>
AWT::DrawMaterial::P AWT::OpenGLDrawableFactory<T>::createTexture( const unsigned char* data, const unsigned int w, const unsigned int h, const bool lighting = true )
{
   m_D->ctx->makeCurrent();
   return OpenGLTexture::getInstance( data, w, h, lighting );
}

template <class T>
AWT::DrawMaterial::P AWT::OpenGLDrawableFactory<T>::createMaterial( const float* ambient, const float* diffuse, const float* specular, const float shininess, const float opacity )
{
   return OpenGLMaterial::getInstance( ambient, diffuse, specular, shininess, opacity );
}

template <class T>
AWT::DrawColour::P AWT::OpenGLDrawableFactory<T>::createColour( const float r, const float g, const float b, const float a, const bool lighting )
{
   return OpenGLSimpleColour::getInstance( r, g, b, a, lighting );
}

template <class T>
AWT::DrawableImpl::P AWT::OpenGLDrawableFactory<T>::createAABB( typename AWT::AxisAlignedBoundingBox<T,3>::P box )
{
   return OpenGLDrawableAABB<T>::getInstance( box );
}

template <class T>
typename AWT::DrawablePoints<T>::P AWT::OpenGLDrawableFactory<T>::createPoints( typename AWT::Tuples<T>::P points )
{
   return OpenGLDrawablePoints<T>::getInstance( points );
}

template <class T>
typename AWT::DrawableQuiver<T>::P AWT::OpenGLDrawableFactory<T>::createQuiver( typename AWT::Tuples<T>::P points, typename AWT::Tuples<T>::P points2 )
{
   return OpenGLDrawableQuiver<T>::getInstance( points, points2 );
}

template <class T>
typename AWT::DrawableMeshNormals<T>::P AWT::OpenGLDrawableFactory<T>::createMeshNormals( typename Mesh<T>::P mesh )
{
   return OpenGLDrawableMeshNormals<T>::getInstance( mesh );
}

template <class T>
typename AWT::DrawAxis::P AWT::OpenGLDrawableFactory<T>::createAxis(const double scale)
{
   return OpenGLDrawAxis::getInstance( scale );
}

template class AWT::OpenGLDrawableFactory<double>;
template class AWT::OpenGLDrawableFactory<float>;