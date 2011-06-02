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
#include "DrawMeshBreakerHistory.h"
#define NOMINMAX
#include "OpenGLDrawables/OpenGLHeaders.h"

template <class T>
struct AWT::DrawMeshBreakerHistory<T>::D
{
   typename MeshBreaker<T>::P m_MeshBreaker;

   typename Mesh<T>::P m_Unvisited;

   double m_Bounds[6];

   MeshIndex m_Start;
   MeshIndex m_End;

   ModifiedTime m_MeshLastModified;
};

template <class T>
AWT::DrawMeshBreakerHistory<T>::DrawMeshBreakerHistory(typename MeshBreaker<T>::P mb)
{
   m_D = new D();

   m_D->m_MeshBreaker = mb;
   getBounds(m_D->m_Bounds);

   m_D->m_Unvisited = mb->getRegion(std::numeric_limits<MeshIndex>::max());

   m_D->m_Start = 0;
   m_D->m_End = mb->getHistoryLength();
}

template <class T>
AWT::DrawMeshBreakerHistory<T>::~DrawMeshBreakerHistory()
{
   delete m_D;
}

template <class T>
typename AWT::DrawMeshBreakerHistory<T>::P AWT::DrawMeshBreakerHistory<T>::getInstance(typename MeshBreaker<T>::P mb )
{
   AUTOGETINSTANCE(AWT::DrawMeshBreakerHistory<T>, (mb));
}

template <class T>
GETNAMEMACRO(AWT::DrawMeshBreakerHistory<T>);

template <class T>
void AWT::DrawMeshBreakerHistory<T>::setStart(AWT::MeshIndex v)
{
   if (v != m_D->m_Start)
   {
      v = std::max<MeshIndex>(0, std::min<MeshIndex>(m_D->m_MeshBreaker->getHistoryLength()-1, v));
      m_D->m_Start = v;
      modified();

      MeshIndex vs[3];

      Mesh<T>::P mesh = m_D->m_MeshBreaker->getMesh();
      mesh->getFaceIndices(m_D->m_MeshBreaker->getHistoryFace(v), vs);

      DEBUGMACRO("Setting start:");
      PRINTVBL2("Face", m_D->m_MeshBreaker->getHistoryFace(v));
      PRINTVEC(vs, 3);
   }
}

template <class T>
AWT::MeshIndex AWT::DrawMeshBreakerHistory<T>::getStart() const
{
   return m_D->m_Start;
}

template <class T>
void AWT::DrawMeshBreakerHistory<T>::setEnd(AWT::MeshIndex v)
{
   if (v != m_D->m_End)
   {
      v = std::max<MeshIndex>(0, std::min<MeshIndex>(m_D->m_MeshBreaker->getHistoryLength()-1, v));

      m_D->m_End = v;
      modified();
      
      MeshIndex vs[3], fs[30];

      Mesh<T>::P mesh = m_D->m_MeshBreaker->getMesh();
      mesh->getFaceIndices(m_D->m_MeshBreaker->getHistoryFace(v), vs);

      DEBUGMACRO("Setting end:");
      PRINTVBL2("Face", m_D->m_MeshBreaker->getHistoryFace(v));
      PRINTVEC(vs, 3);

      MeshConnectivity<T>::P conn = m_D->m_MeshBreaker->getConnectivity();

      MeshIndex nadj = conn->getAdjacentFaces(m_D->m_MeshBreaker->getHistoryFace(v), fs);

      for (MeshIndex i = 0; i < nadj; ++i)
      {
         mesh->getFaceIndices(fs[i], vs);
         DEBUGMACRO("Adj face " << i << ": " << fs[i]);
         PRINTVEC(vs, 3);
      }
   }
}

template <class T>
AWT::MeshIndex AWT::DrawMeshBreakerHistory<T>::getEnd() const
{
   return m_D->m_End;
}

template <class T>
AWT::DrawMaterial::P AWT::DrawMeshBreakerHistory<T>::getMaterial()
{
   return DrawMaterial::P();
}

template <class T>
void AWT::DrawMeshBreakerHistory<T>::setMaterial(AWT::DrawMaterial::P /*mat*/)
{
}

template <class T>
void AWT::DrawMeshBreakerHistory<T>::getBounds(double *bounds)
{
   DEBUGLINE;
   if (getModifiedTime() > m_D->m_MeshBreaker->getModifiedTime() || m_D->m_MeshLastModified < m_D->m_MeshBreaker->getModifiedTime())
   {
      Mesh<T>::P mesh = m_D->m_MeshBreaker->getMesh();

      for (int ax = 0; ax < 3; ++ax)
      {
         m_D->m_Bounds[2*ax+0] =  std::numeric_limits<double>::infinity();
         m_D->m_Bounds[2*ax+1] = -std::numeric_limits<double>::infinity();
      }

      T vtxA[3], vtxB[3], vtxC[3];

      for (MeshIndex h = m_D->m_Start; h <= m_D->m_End && h < m_D->m_MeshBreaker->getHistoryLength(); ++h)
      {
         MeshIndex f = m_D->m_MeshBreaker->getHistoryFace(h);

         mesh->getFace(f, vtxA, vtxB, vtxC);

         for (int ax = 0; ax < 3; ++ax)
         {
            m_D->m_Bounds[2*ax+0] = std::min<double>(m_D->m_Bounds[2*ax+0], vtxA[ax]);
            m_D->m_Bounds[2*ax+1] = std::max<double>(m_D->m_Bounds[2*ax+1], vtxA[ax]);

            m_D->m_Bounds[2*ax+0] = std::min<double>(m_D->m_Bounds[2*ax+0], vtxB[ax]);
            m_D->m_Bounds[2*ax+1] = std::max<double>(m_D->m_Bounds[2*ax+1], vtxB[ax]);

            m_D->m_Bounds[2*ax+0] = std::min<double>(m_D->m_Bounds[2*ax+0], vtxC[ax]);
            m_D->m_Bounds[2*ax+1] = std::max<double>(m_D->m_Bounds[2*ax+1], vtxC[ax]);
         }
      }

      /*
      T vtx[3];
      for (MeshIndex v = 0; v < mesh->getNumberOfVertices() ; ++v)
      {
         mesh->getVertex(v, vtx);

         for (int ax = 0; ax < 3; ++ax)
         {
            m_D->m_Bounds[2*ax+0] = std::min<double>(m_D->m_Bounds[2*ax+0], vtx[ax]);
            m_D->m_Bounds[2*ax+1] = std::max<double>(m_D->m_Bounds[2*ax+1], vtx[ax]);
         }
      }
      */

      m_D->m_MeshLastModified = m_D->m_MeshBreaker->getModifiedTime();
      modified();
   }

   for (int i = 0; i < 6; ++i)
      bounds[i] = m_D->m_Bounds[i];
}

#include "Useful/ArrayVectorFunctions.h"

template <class T>
void AWT::DrawMeshBreakerHistory<T>::drawImpl(AWT::DrawContext::P context)
{
   Mesh<T>::P mesh = m_D->m_MeshBreaker->getMesh();

   T vtxA[3], vtxB[3], vtxC[3];
   T nmlA[3], nmlB[3], nmlC[3];

   glEnable(GL_COLOR_MATERIAL);
   glEnable(GL_LIGHTING);
   glBegin(GL_TRIANGLES);

   //for (MeshIndex f = 0; f < m_D->m_Unvisited->getNumberOfFaces(); ++f)
   MESH_EACHFACE(m_D->m_Unvisited, f)
   {
      m_D->m_Unvisited->getFace(f, vtxA, vtxB, vtxC);

      glColor3f(1.f, 0.f, 1.f);

      for (int ax = 0; ax < 3; ++ax)
      {
         nmlA[ax] = vtxB[ax] - vtxA[ax];
         nmlB[ax] = vtxC[ax] - vtxA[ax];
      }

      cross(nmlA, nmlB, nmlC);

      if (dot(nmlC, nmlC, 3) != 0)
         normalize(nmlC, 3);

      glNormal3Tv(nmlC);

      glVertex3Tv(vtxA);
      glVertex3Tv(vtxB);
      glVertex3Tv(vtxC);
   }

   MeshIndex f = 0;

   bool anyDrawn = false;
   MeshIndex hLast = 0;

   for (MeshIndex h = m_D->m_Start; h < m_D->m_End && h < m_D->m_MeshBreaker->getHistoryLength(); ++h)
   {
      f = m_D->m_MeshBreaker->getHistoryFace(h);

      mesh->getFace(f, vtxA, vtxB, vtxC);

      switch (m_D->m_MeshBreaker->getHistorySymbol(h))
      {
      case AWT::MeshBreaker<T>::SYM_C:
         glColor3f(0.f, 0.f, 1.f);
         break;
      case AWT::MeshBreaker<T>::SYM_L:
         glColor3f(1.f, 0.f, 0.f);
         break;
      case AWT::MeshBreaker<T>::SYM_E:
         glColor3f(0.5f, 0.5f, 0.5f);
         break;
      case AWT::MeshBreaker<T>::SYM_R:
         glColor3f(0.f, 1.f, 0.f);
         break;
      case AWT::MeshBreaker<T>::SYM_S:
         glColor3f(1.f, 1.f, 0.f);
         break;
      }

      for (int ax = 0; ax < 3; ++ax)
      {
         nmlA[ax] = vtxB[ax] - vtxA[ax];
         nmlB[ax] = vtxC[ax] - vtxA[ax];
      }

      cross(nmlA, nmlB, nmlC);

      if (dot(nmlC, nmlC, 3) != 0)
         normalize(nmlC, 3);

      glNormal3Tv(nmlC);

      glVertex3Tv(vtxA);
      glVertex3Tv(vtxB);
      glVertex3Tv(vtxC);

      anyDrawn = true;
      hLast = h;
   }

   glEnd();

   if (anyDrawn)
   {
      f = m_D->m_MeshBreaker->getHistoryFace(hLast);

      mesh->getFace(f, vtxA, vtxB, vtxC);

      glLineWidth(3.f);
      glDisable(GL_LIGHTING);
      glBegin(GL_LINE_STRIP);
      glColor3f(1.f, 1.f, 1.f);

      glVertex3Tv(vtxA);
      glVertex3Tv(vtxB);
      glVertex3Tv(vtxC);

      glEnd();
   }

   for (MeshIndex i = 0; i < m_D->m_MeshBreaker->getNumberOfDebugFaces(); ++i)
   {
      f = m_D->m_MeshBreaker->getDebugFace(i);
      PRINTVBL(f);

      mesh->getFace(f, vtxA, vtxB, vtxC);

      glLineWidth(3.f);
      glDisable(GL_LIGHTING);
      glBegin(GL_LINE_STRIP);
      glColor3f(1.f, 0.f, 0.f);

      glVertex3Tv(vtxA);
      glVertex3Tv(vtxB);
      glVertex3Tv(vtxC);

      glEnd();
   }
}

template class AWT::DrawMeshBreakerHistory<double>;
template class AWT::DrawMeshBreakerHistory<float>;
