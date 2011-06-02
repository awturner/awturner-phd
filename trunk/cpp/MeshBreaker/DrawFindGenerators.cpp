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
#define NOMINMAX

#include "DrawFindGenerators.h"

#include "OpenGLDrawables/OpenGLHeaders.h"
#include "Useful/HueWheelSequence.h"

template <class T>
struct AWT::DrawFindGenerators<T>::D
{
   typename FindGenerators<T>::P m_FindGenerators;

   double m_Bounds[6];

   ModifiedTime m_MeshLastModified;
};

template <class T>
AWT::DrawFindGenerators<T>::DrawFindGenerators(typename FindGenerators<T>::P fg)
{
   m_D = new D();

   m_D->m_FindGenerators = fg;
   getBounds(m_D->m_Bounds);
}

template <class T>
AWT::DrawFindGenerators<T>::~DrawFindGenerators()
{
   delete m_D;
}

template <class T>
typename AWT::DrawFindGenerators<T>::P AWT::DrawFindGenerators<T>::getInstance(typename FindGenerators<T>::P fg )
{
   AUTOGETINSTANCE(AWT::DrawFindGenerators<T>, (fg));
}

template <class T>
GETNAMEMACRO(AWT::DrawFindGenerators<T>);

template <class T>
AWT::DrawMaterial::P AWT::DrawFindGenerators<T>::getMaterial()
{
   return DrawMaterial::P();
}

template <class T>
void AWT::DrawFindGenerators<T>::setMaterial(AWT::DrawMaterial::P /*mat*/)
{
}

template <class T>
void AWT::DrawFindGenerators<T>::getBounds(double *bounds)
{
   if (getModifiedTime() > m_D->m_FindGenerators->getModifiedTime() || m_D->m_MeshLastModified < m_D->m_FindGenerators->getModifiedTime())
   {
      Mesh<T>::P mesh = m_D->m_FindGenerators->getMesh();

      for (int ax = 0; ax < 3; ++ax)
      {
         m_D->m_Bounds[2*ax+0] =  std::numeric_limits<double>::infinity();
         m_D->m_Bounds[2*ax+1] = -std::numeric_limits<double>::infinity();
      }

      T vtxA[3];

      //for (MeshIndex h = 0; h < mesh->getNumberOfVertices(); ++h)
      MESH_EACHVERTEX(mesh, h)
      {
         mesh->getVertex(h, vtxA);

         for (int ax = 0; ax < 3; ++ax)
         {
            if (abs(vtxA[ax]) < std::numeric_limits<T>::infinity())
            {
               m_D->m_Bounds[2*ax+0] = std::min<double>(m_D->m_Bounds[2*ax+0], vtxA[ax]);
               m_D->m_Bounds[2*ax+1] = std::max<double>(m_D->m_Bounds[2*ax+1], vtxA[ax]);
            }
         }
      }

      m_D->m_MeshLastModified = m_D->m_FindGenerators->getModifiedTime();
      modified();
   }

   for (int i = 0; i < 6; ++i)
      bounds[i] = m_D->m_Bounds[i];
}

#include "Useful/ArrayVectorFunctions.h"

template <class T>
void AWT::DrawFindGenerators<T>::drawImpl(AWT::DrawContext::P context)
{
   Mesh<T>::P mesh = m_D->m_FindGenerators->getMesh();

   T vtx[3];

   glEnable(GL_COLOR_MATERIAL);

   glEnable(GL_COLOR_MATERIAL);
   
   glDisable(GL_LIGHTING);
   
   Tuples<MeshIndex>::P cont[2];

   glLineWidth(3.f);

   HueWheelSequence::P hws = HueWheelSequence::getInstance(m_D->m_FindGenerators->getNumberOfGeneratorPairs());
   float rgba[4];
   
   //glEnable(GL_LINE_STIPPLE);
   for (MeshIndex g = 0; g < m_D->m_FindGenerators->getNumberOfGeneratorPairs(); ++g)
   {
      hws->nextColour(rgba);

      m_D->m_FindGenerators->getGeneratorPair(g, cont[0], cont[1]);

      for (int i = 0; i < 2; ++i)
      {
         glColor3fv(rgba);

         glBegin(GL_LINE_LOOP);

         for (MeshIndex v = 0; v < cont[i]->getNumberOfPoints(); ++v)
         {
            mesh->getVertex(cont[i]->getPointElement(v, 0), vtx);

            glVertex3Tv(vtx);
         }

         glEnd();

         for (int i = 0; i < 3; ++i)
            rgba[i] = 1 - rgba[i];
      }
   }
   //glDisable(GL_LINE_STIPPLE);

   glPointSize(10.f);
   glColor3f(1.f, 1.f, 0.f);

   mesh->getVertex(m_D->m_FindGenerators->getLastMergePoint(), vtx);
   glBegin(GL_POINTS);
   glVertex3Tv(vtx);
   glEnd();

   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glEnable(GL_LIGHTING);
   glPolygonOffset(-10.f, -10.f);
   glBegin(GL_TRIANGLES);

   glColor3f(1.f, 1.f, 1.f);

   //T vtxA[3], vtxB[3], vtxC[3];
   //T nmlA[3], nmlB[3], nmlC[3];
   //MESH_EACHFACE(mesh, f)
   //{
   //   if (m_D->m_FindGenerators->isFaceVisited(f))
   //   {
   //      mesh->getFace(f, vtxA, vtxB, vtxC);

   //      for (int ax = 0; ax < 3; ++ax)
   //      {
   //         nmlA[ax] = vtxB[ax] - vtxA[ax];
   //         nmlB[ax] = vtxC[ax] - vtxA[ax];
   //      }

   //      cross(nmlA, nmlB, nmlC);

   //      if (dot(nmlC, nmlC, 3) != 0)
   //         normalize(nmlC, 3);

   //      glNormal3Tv(nmlC);

   //      glVertex3Tv(vtxA);
   //      glVertex3Tv(vtxB);
   //      glVertex3Tv(vtxC);
   //   }
   //}

   glEnd();
   glPolygonOffset(0.f, 0.f);
}

template class AWT::DrawFindGenerators<double>;
template class AWT::DrawFindGenerators<float>;
