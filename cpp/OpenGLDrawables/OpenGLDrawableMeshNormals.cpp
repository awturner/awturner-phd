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
#include "OpenGLDrawableMeshNormals.h"

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"
#include "DrawableFramework/DrawMaterial.h"

#include "OpenGLHeaders.h"

#include <limits>

template <class T>
struct AWT::OpenGLDrawableMeshNormals<T>::D
{
   void drawFaceNormals(AWT::DrawContext::P context)
   {
      if (*m_Mesh == 0 || *m_Mesh->getVertexNormals() == 0)
         return;

      T centroid[3];
      T nml[3];

      glDisable(GL_LIGHTING);
      glEnable(GL_COLOR_MATERIAL);

      MeshIndex vs[3];
      T vtx[3];

      glBegin(GL_LINES);
      //for (MeshIndex f = 0; f < m_Mesh->getNumberOfFaces(); ++f)
      MESH_EACHFACE(m_Mesh, f)
      {
         for (MeshIndex d = 0; d < 3; ++d)
            centroid[d] = 0;

         m_Mesh->getFaceIndices(f, vs);
   
         for (MeshIndex d = 0; d < 3; ++d)
         {
            m_Mesh->getVertex(vs[d], vtx);

            centroid[0] += vtx[0];
            centroid[1] += vtx[1];
            centroid[2] += vtx[2];
         }

         for (MeshIndex d = 0; d < 3; ++d)
            centroid[d] /= 3;

         m_Mesh->getFaceNormals()->getPoint(f, nml);
         
         for (MeshIndex d = 0; d < 3; ++d)
            nml[d] = m_Scale*nml[d] + centroid[d];

         glVertex3Tv(centroid);
         glVertex3Tv(nml);
      }
      glEnd();
   }

   void drawVertexNormals(AWT::DrawContext::P context)
   {
      if (*m_Mesh == 0 || *m_Mesh->getVertexNormals() == 0)
         return;

      T centroid[3];
      T nml[3];

      glDisable(GL_LIGHTING);
      glEnable(GL_COLOR_MATERIAL);

      glColor3f(0.f, 1.f, 0.f);

      glBegin(GL_LINES);
      //for (MeshIndex f = 0; f < m_Mesh->getNumberOfVertices(); ++f)
      MESH_EACHVERTEX(m_Mesh, f)
      {
         m_Mesh->getVertex(f, centroid);
         m_Mesh->getVertexNormals()->getPoint(f, nml);

         for (MeshIndex d = 0; d < 3; ++d)
            nml[d] = m_Scale*nml[d] + centroid[d];

         glVertex3Tv(centroid);
         glVertex3Tv(nml);
      }
      glEnd();
   }

   typename Mesh<T>::P m_Mesh;
   DrawMaterial::P m_Material;
   DrawNormalsType m_DrawType;
   T m_Scale;
   double m_Bounds[6];
};

template <class T>
AWT::OpenGLDrawableMeshNormals<T>::OpenGLDrawableMeshNormals(typename AWT::Mesh<T>::P mesh)
{
   m_D = new D;

   m_D->m_DrawType = AWT::NORMALS_VERTEX;
   m_D->m_Scale    = 1;

   setMesh(mesh);
}

template <class T>
AWT::OpenGLDrawableMeshNormals<T>::~OpenGLDrawableMeshNormals()
{
   delete m_D;
}

template <class T>
typename AWT::OpenGLDrawableMeshNormals<T>::P AWT::OpenGLDrawableMeshNormals<T>::getInstance(typename AWT::Mesh<T>::P mesh)
{
   AUTOGETINSTANCE(AWT::OpenGLDrawableMeshNormals<T>, (mesh));
}

template <class T>
AWT::ModifiedTime AWT::OpenGLDrawableMeshNormals<T>::getChildModifiedTime()
{
   return getTimeObjectModified();
}

template <class T>
GETNAMEMACRO(AWT::OpenGLDrawableMeshNormals<T>);

template <class T>
typename AWT::Mesh<T>::P AWT::OpenGLDrawableMeshNormals<T>::getMesh()
{
   return m_D->m_Mesh;
}

template <class T>
void AWT::OpenGLDrawableMeshNormals<T>::setMesh(typename AWT::Mesh<T>::P mesh)
{
   m_D->m_Mesh = mesh;
   setTimeObject(mesh);
   modified();
}

template <class T>
AWT::DrawNormalsType AWT::OpenGLDrawableMeshNormals<T>::getDrawNormalsType() const
{
   return m_D->m_DrawType;
}

template <class T>
void AWT::OpenGLDrawableMeshNormals<T>::setDrawNormalsType(const AWT::DrawNormalsType da)
{
   m_D->m_DrawType = da;
   modified();
}

template <class T>
T AWT::OpenGLDrawableMeshNormals<T>::getScale() const
{
   return m_D->m_Scale;
}

template <class T>
void AWT::OpenGLDrawableMeshNormals<T>::setScale(const T scale)
{
   m_D->m_Scale = scale;
   modified();
}

template <class T>
void AWT::OpenGLDrawableMeshNormals<T>::buildList(AWT::DrawContext::P context)
{
   switch (m_D->m_DrawType)
   {
   case AWT::NORMALS_FACE:
      m_D->drawFaceNormals(context);
      break;
   case AWT::NORMALS_VERTEX:
      m_D->drawVertexNormals(context);
      break;
   }
}

template <class T>
void AWT::OpenGLDrawableMeshNormals<T>::updateBounds()
{
   T centroid[3];

   for (int ax = 0; ax < 3; ++ax)
   {
      m_D->m_Bounds[2*ax+0] =  std::numeric_limits<double>::infinity();
      m_D->m_Bounds[2*ax+1] = -std::numeric_limits<double>::infinity();
   }

   //for (MeshIndex f = 0; f < m_D->m_Mesh->getNumberOfVertices(); ++f)
   MESH_EACHVERTEX(m_D->m_Mesh, v)
   {
      m_D->m_Mesh->getVertex(v, centroid);

      for (int ax = 0; ax < 3; ++ax)
      {
         m_D->m_Bounds[2*ax+0] = std::min<double>(m_D->m_Bounds[2*ax+0], centroid[ax]);
         m_D->m_Bounds[2*ax+1] = std::min<double>(m_D->m_Bounds[2*ax+1], centroid[ax]);
      }
   }
   
}

template <class T>
double AWT::OpenGLDrawableMeshNormals<T>::getBoundImpl(unsigned int i)
{
   return m_D->m_Bounds[i];
}

template <class T>
AWT::DrawMaterial::P AWT::OpenGLDrawableMeshNormals<T>::getMaterial()
{
   return m_D->m_Material;
}

template <class T>
void AWT::OpenGLDrawableMeshNormals<T>::setMaterial(AWT::DrawMaterial::P mat)
{
   m_D->m_Material = mat;
}

template class AWT::OpenGLDrawableMeshNormals<double>;
template class AWT::OpenGLDrawableMeshNormals<float>;