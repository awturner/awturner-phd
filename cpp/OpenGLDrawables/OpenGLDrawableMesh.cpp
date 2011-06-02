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
#include "OpenGLDrawableMesh.h"

#include "DrawableFramework/DrawMaterial.h"

#include "Mesh/AreaAveragedNormalCalculator.h"
#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"

#include "OpenGLHeaders.h"

#include "Useful/PrintMacros.h"

#include <limits>



template <class T>
struct AWT::OpenGLDrawableMesh<T>::D
{
   typename Mesh<T>::P m_Mesh;
   DrawMaterial::P m_Material;
   
   DrawMeshType        m_DrawAs;
   float               m_PointSize;
   float               m_LineWidth;

   bool                m_AutoNormals;
   bool                m_HideBackFaces;

   double        m_Bounds[6];
};

template <class T>
AWT::OpenGLDrawableMesh<T>::OpenGLDrawableMesh(typename AWT::Mesh<T>::P mesh)
{
   m_D = new D;

   m_D->m_PointSize = m_D->m_LineWidth = 1.f;
   m_D->m_DrawAs = AWT::DRAWAS_SOLID;

   m_D->m_AutoNormals   = true;
   m_D->m_HideBackFaces = true;

   setMesh(mesh);
}

template <class T>
AWT::OpenGLDrawableMesh<T>::~OpenGLDrawableMesh()
{
   delete m_D;
}

template <class T>
typename AWT::OpenGLDrawableMesh<T>::P AWT::OpenGLDrawableMesh<T>::getInstance(typename AWT::Mesh<T>::P mesh)
{
   AUTOGETINSTANCE(AWT::OpenGLDrawableMesh<T>, (mesh));
}

template <class T>
GETNAMEMACRO(AWT::OpenGLDrawableMesh<T>);

template <class T>
typename AWT::Mesh<T>::P AWT::OpenGLDrawableMesh<T>::getMesh()
{
   return m_D->m_Mesh;
}

template <class T>
void AWT::OpenGLDrawableMesh<T>::setMesh(typename AWT::Mesh<T>::P mesh)
{
   m_D->m_Mesh = mesh;
   setTimeObject(mesh);
   modified();
}

template <class T>
AWT::DrawMeshType AWT::OpenGLDrawableMesh<T>::getDrawAs() const
{
   return m_D->m_DrawAs;
}

template <class T>
void AWT::OpenGLDrawableMesh<T>::setDrawAs(const AWT::DrawMeshType d)
{
   if (d != m_D->m_DrawAs)
   {
      m_D->m_DrawAs = d;
      modified();
   }
}

template <class T>
AWT::ModifiedTime AWT::OpenGLDrawableMesh<T>::getChildModifiedTime()
{
   return getTimeObjectModified();
}

template <class T>
void AWT::OpenGLDrawableMesh<T>::beforeCallList(AWT::DrawContext::P context)
{
   glLineWidth(m_D->m_LineWidth);
   glPointSize(m_D->m_PointSize);

   if (*m_D->m_Mesh->getVertexNormals() != 0)
   {
      switch (m_D->m_DrawAs)
      {
      case DRAWAS_POINTS:
         glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
         break;
      case DRAWAS_LINES:
         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
         break;
      case DRAWAS_SOLID:
         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
         break;
      }
   }
   else
   {
      glPointSize(m_D->m_PointSize);
      glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
   }
}

template <class T>
void AWT::OpenGLDrawableMesh<T>::buildList(AWT::DrawContext::P context)
{
   DEBUGMACRO("Horrible TexCoords HACK");

   T vtx[3];
   T nml[3];
   MeshIndex vs[3];
   
   m_D->m_Bounds[0] = m_D->m_Bounds[2] = m_D->m_Bounds[4] =  std::numeric_limits<T>::infinity();
   m_D->m_Bounds[1] = m_D->m_Bounds[3] = m_D->m_Bounds[5] = -std::numeric_limits<T>::infinity();

   if (*m_D->m_Mesh->getFaceNormals() == 0 || *m_D->m_Mesh->getVertexNormals() == 0)
      glDisable(GL_LIGHTING);

   if (m_D->m_AutoNormals && m_D->m_Mesh->getModifiedTime() < this->getModifiedTime())
   {
      if (*m_D->m_Mesh->getFaces() != 0 && m_D->m_Mesh->getNumberOfFaces() != 0)
      {
         AreaAveragedNormalCalculator<T>::getInstance()->calculateNormalsAndSet(m_D->m_Mesh);
      }
   }

   Tuples<T>::P vertNorms = m_D->m_Mesh->getVertexNormals();
   Tuples<T>::P faceNorms = m_D->m_Mesh->getFaceNormals();
   Tuples<T>::P texCoords = m_D->m_Mesh->getTextureCoords();

   m_D->m_Mesh->lock(this);

   DrawMaterial::P drawMat = getMaterial();

   if (*drawMat != 0)
   {
      drawMat->prepare();

      if (!glIsEnabled(GL_LIGHTING))
      {
         switch (m_D->m_DrawAs)
         {
         case DRAWAS_POINTS:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
         case DRAWAS_LINES:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
         case DRAWAS_SOLID:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
         }
      }
   }

   if (*vertNorms == 0 && *faceNorms == 0)
   {
      glDisable(GL_LIGHTING);
      glEnable(GL_COLOR_MATERIAL);
   }

   if (m_D->m_HideBackFaces)
   {
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
   }
   else
   {
      glDisable(GL_CULL_FACE);
   }

   if (*m_D->m_Mesh->getFaces() == 0 || m_D->m_Mesh->getNumberOfFaces() == 0)
   {
      glBegin(GL_POINTS);
      //for (MeshIndex v = 0; v < m_D->m_Mesh->getNumberOfVertices(); ++v)
      MESH_EACHVERTEX(m_D->m_Mesh, v)
      {
         m_D->m_Mesh->getVertex(v, vtx);
         
         if (*vertNorms != 0)
         {
            vertNorms->getPoint(v, nml);
            glNormal3Tv<T>(nml);
         }

         if (*texCoords != 0)
         {
            texCoords->getPoint(v, nml);
            glTexCoord2Tv<T>(nml);
         }

         drawMat->tweak(DrawMaterial::TWEAK_VERTEX, v);
         glVertex3Tv<T>(vtx);

         updateBoundsForVertex(vtx);
      }
      glEnd();
   }
   else
   {
      glBegin(GL_TRIANGLES);

      MESH_EACHFACE(m_D->m_Mesh, f)
      {
         m_D->m_Mesh->getFaceIndices(f, vs);
         drawMat->tweak(DrawMaterial::TWEAK_FACE, f);

         if (*faceNorms != 0)
         {
            faceNorms->getPoint(f, nml);
            glNormal3Tv<T>(nml);
         }
         
         for (MeshIndex d = 0; d < 3; ++d)
         {
            if (*vertNorms != 0)
            {
               vertNorms->getPoint(vs[d], nml);
               glNormal3Tv<T>(nml);
            }

            if (*texCoords != 0)
            {
               texCoords->getPoint(vs[d], nml);
               glTexCoord2Tv<T>(nml);
            }

            m_D->m_Mesh->getVertex(vs[d], vtx);

            drawMat->tweak(DrawMaterial::TWEAK_VERTEX, vs[d]);

            glVertex3Tv<T>(vtx);

            for (int i = 0; i < 3; ++i)
            {
               m_D->m_Bounds[2*i+0] = std::min<double>(m_D->m_Bounds[2*i+0], vtx[i]);
               m_D->m_Bounds[2*i+1] = std::max<double>(m_D->m_Bounds[2*i+1], vtx[i]);
            }

         }
      }
      glEnd();
   }

   if (*drawMat != 0)
      drawMat->unprepare();

   m_D->m_Mesh->unlock(this);

   modified();
}

template <class T>
void AWT::OpenGLDrawableMesh<T>::afterCallList(AWT::DrawContext::P context)
{
   glDisable(GL_CULL_FACE);
}

template <class T>
void AWT::OpenGLDrawableMesh<T>::updateBounds()
{
   m_D->m_Bounds[0] = m_D->m_Bounds[2] = m_D->m_Bounds[4] =  std::numeric_limits<double>::infinity();
   m_D->m_Bounds[1] = m_D->m_Bounds[3] = m_D->m_Bounds[5] = -std::numeric_limits<double>::infinity();

   T vtxa[3], vtxb[3], vtxc[3];

   //for (MeshIndex f = 0; f < m_D->m_Mesh->getNumberOfFaces(); ++f)
   MESH_EACHFACE(m_D->m_Mesh, f)
   {
      m_D->m_Mesh->getFace(f, vtxa, vtxb, vtxc);
      updateBoundsForVertex(vtxa);
      updateBoundsForVertex(vtxb);
      updateBoundsForVertex(vtxc);
   }
}

template <class T>
void AWT::OpenGLDrawableMesh<T>::updateBoundsForVertex(T* vtx)
{
   for (int i = 0; i < 3; ++i)
   {
      m_D->m_Bounds[2*i+0] = std::min<double>(m_D->m_Bounds[2*i+0], vtx[i]);
      m_D->m_Bounds[2*i+1] = std::max<double>(m_D->m_Bounds[2*i+1], vtx[i]);
   }
}

template <class T>
double AWT::OpenGLDrawableMesh<T>::getBoundImpl(unsigned int i)
{
   return m_D->m_Bounds[i];
}

template <class T>
AWT::DrawMaterial::P AWT::OpenGLDrawableMesh<T>::getMaterial()
{
   return m_D->m_Material;
}

template <class T>
void AWT::OpenGLDrawableMesh<T>::setMaterial(AWT::DrawMaterial::P mat)
{
   m_D->m_Material = mat;
}

template <class T>
float AWT::OpenGLDrawableMesh<T>::getPointSize() const
{
   return m_D->m_PointSize;
}

template <class T>
void AWT::OpenGLDrawableMesh<T>::setPointSize(const float v)
{
   if (v >= 0 && v != m_D->m_PointSize)
   {
      m_D->m_PointSize = v;
      modified();
   }
}

template <class T>
float AWT::OpenGLDrawableMesh<T>::getLineWidth() const
{
   return m_D->m_LineWidth;
}

template <class T>
void AWT::OpenGLDrawableMesh<T>::setLineWidth(const float v)
{
   if (v >= 0 && v != m_D->m_LineWidth)
   {
      m_D->m_LineWidth = v;
      modified();
   }
}

template <class T>
bool AWT::OpenGLDrawableMesh<T>::isAutoNormals() const
{
   return m_D->m_AutoNormals;
}

template <class T>
void AWT::OpenGLDrawableMesh<T>::setAutoNormals(const bool v)
{
   if (v != m_D->m_AutoNormals)
   {
      m_D->m_AutoNormals = v;
      modified();
   }
}

template <class T>
bool AWT::OpenGLDrawableMesh<T>::isHideBackFaces() const
{
   return m_D->m_HideBackFaces;
}

template <class T>
void AWT::OpenGLDrawableMesh<T>::setHideBackFaces(const bool v)
{
   if (v != m_D->m_HideBackFaces)
   {
      m_D->m_HideBackFaces = v;
      modified();
   }
}

template class AWT::OpenGLDrawableMesh<double>;
template class AWT::OpenGLDrawableMesh<float>;