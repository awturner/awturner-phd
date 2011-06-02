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
#include "PelvicGeneratorDirection.h"

#include "Mesh/MeshImpl.h"
#include "HalfEdge.h"
#include "HalfEdgeVertex.h"

#include "Mesh/Mesh.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/AreaAveragedNormalCalculator.h"

#include "Useful/ArrayVectorFunctions.h"

#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"

#include <algorithm>

template <class T>
struct AWT::PelvicGeneratorDirection<T>::D
{
   typedef std::vector< HalfEdge* > HalfEdgeVec;

   void calculateGeneratorCentroid(HalfEdgeVec& generator, T* centroid)
   {
      centroid[0] = centroid[1] = centroid[2] = 0;

      HalfEdgeVec::iterator it = generator.begin();
      HalfEdgeVec::iterator en = generator.end();

      T vtx[3];
      while (it != en)
      {
         m_Mesh->getVertex((*it)->getVertex()->getIndex(), vtx);

         for (int i = 0; i < 3; ++i)
            centroid[i] += vtx[i];

         ++it;
      }

      for (int i = 0; i < 3; ++i)
         centroid[i] /= generator.size();
   }

   int pickObturatorGenerator(HalfEdgeVec* generators)
   {
      if (!m_Mesh->hasVertexNormals())
      {
         typename NormalCalculator<T>::P nc = AreaAveragedNormalCalculator<T>::getInstance();
         nc->calculateNormalsAndSet(m_Mesh);
      }

      // Find the common point
      MeshIndex v[2];
      v[1] = 0;

      for (v[0] = 0; v[0] < generators[0].size() && generators[0][ v[0] ]->getVertex() != generators[1][ v[1] ]->getVertex(); ++v[0])
         for (v[1] = 0; v[1] < generators[1].size() && generators[0][ v[0] ]->getVertex() != generators[1][ v[1] ]->getVertex(); ++v[1]);
      
      if (v[0] >= generators[0].size() || v[1] >= generators[1].size())
         AWTEXCEPTIONTHROW("Couldn't find common vertex");

      if (generators[0][ v[0] ]->getVertex()->getIndex() != generators[1][ v[1] ]->getVertex()->getIndex())
         AWTEXCEPTIONTHROW("Couldn't find common vertex");

      MeshIndex commonVertex = generators[0][ v[0] ]->getVertex()->getIndex();

      // Get the normal at the common point
      T norm[3], vtx[3];
      m_Mesh->getVertex(commonVertex, vtx);
      m_Mesh->getVertexNormals()->getPoint(commonVertex, norm);

      // Shift the centroids to be relative to the common point
      T centroid[2][3];
      for (int i = 0; i < 3; ++i)
      {
         centroid[0][i] = m_Centroids[0][i] - vtx[i];
         centroid[1][i] = m_Centroids[1][i] - vtx[i];
      }

      T dotCentroid[2];
      dotCentroid[0] = dot(centroid[0], norm, 3);
      dotCentroid[1] = dot(centroid[1], norm, 3);

      // We need it to be that only one of the centroids is in the +ve normal direction
      if ((dotCentroid[0] >= 0) ^ (dotCentroid[1] >= 0))
      {
         int ret = (dotCentroid[0] >= 0) ? 0 : 1;

         /*
         HalfEdgeVec tmp = generators[0];
         generators[0] = generators[1];
         generators[1] = tmp;
         */

         return ret;
      }

      AWTEXCEPTIONTHROW("Centroids are on same side of surface!");
   }

   void fitBasis(HalfEdgeVec& generator, const T* centroid, T* normal)
   {
      vnl_matrix<T> mat(3,3);
      mat.fill(0);

      HalfEdgeVec::iterator it = generator.begin();
      HalfEdgeVec::iterator en = generator.end();

      T vtx[3];
      while (it != en)
      {
         m_Mesh->getVertex((*it)->getVertex()->getIndex(), vtx);

         for (int i = 0; i < 3; ++i)
            vtx[i] -= centroid[i];

         for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
               mat[i][j] += vtx[i]*vtx[j];

         ++it;
      }

      vnl_symmetric_eigensystem<T> eig(mat);
      //DEBUGMACRO(eig.get_eigenvalue(0) << ":\t\t" << eig.get_eigenvector(0));
      //DEBUGMACRO(eig.get_eigenvalue(1) << ":\t\t" << eig.get_eigenvector(1));
      //DEBUGMACRO(eig.get_eigenvalue(2) << ":\t\t" << eig.get_eigenvector(2));

      // We want the smallest eigenvalue as the z-direction
      T z[3];
      vnl_vector<T> vec = eig.get_eigenvector(0);

      FOREACHAXIS(ax)
         z[ax] = vec[ax];

      MeshFunctions<T>::calculatePrincipalAxesGivenZ(m_Mesh, z, m_Basis);

      //FOREACHAXIS(ax)
      //   PRINTVEC(m_Basis[ax], 3);
   }

   int calculateMeshSideOfPlane(const T* centroid, const T* normal)
   {
      T meshCentroid[3];
      T totalArea = 0;

      MeshFunctions<T>::calculateFaceCentroid(m_Mesh, meshCentroid);
      for (int i = 0; i < 3; ++i)
         meshCentroid[i] -= centroid[i];

      T dotMesh = dot(meshCentroid, normal, 3);
      
      if (dotMesh > 0)
         return 1;
      else if (dotMesh < 0)
         return -1;
      else
         return 0;
   }

   void calculateBasis(HalfEdgeVec* generators)
   {
      // So... we need to work out which of the generators is the obturator one
      m_Ob = pickObturatorGenerator(generators);
      //PRINTVBL(m_Ob);

      // Righty ho. Now, let's fit a m_Basis through these points
      fitBasis(generators[m_Ob], m_Centroids[m_Ob], m_Basis[2]);

      // If the mesh's centroid is in front of this m_Basis, we want to flip the m_Basis normal
      if (calculateMeshSideOfPlane(m_Centroids[m_Ob], m_Basis[2]) > 0)
      {
         scale<T>(m_Basis[2], -1, 3);
         scale<T>(m_Basis[1], -1, 3);
      }

      // Want to make sure that the x-axis points towards the mesh centroid
      T meshCentroid[3], projCentroid[3];
      MeshFunctions<T>::calculateFaceCentroid(m_Mesh, meshCentroid);

      unproject(meshCentroid, m_Basis, m_Centroids[m_Ob], projCentroid);

      if (projCentroid[0] < 0)
      {
         scale<T>(m_Basis[0], -1, 3);
         scale<T>(m_Basis[1], -1, 3);
      }

      //PRINTVEC(projCentroid, 3);
   }

   void calculatePelvicSide()
   {
      m_PelvicSide = calculateMeshSideOfPlane(m_Centroids[m_Ob], m_Basis[1]);

      PRINTVBL(m_PelvicSide);
   }

   
   void calculateGeneratorDirection(HalfEdgeVec* generators)
   {
      T vtx[3];
      T vtxLast[3];
      T vtxTmp[3];
      T vtxCross[3];

      T ibasis[3][3];
      T imu[3];

      T sumTheta[] = { 0, 0 };

      for (int i = 0; i < 2; ++i)
      {
         HalfEdgeVec::iterator it = generators[i].begin();
         HalfEdgeVec::iterator en = generators[i].end();

         // Want to project relative to the centroid of this generators
         unprojectParameters<T>(m_Basis, m_Centroids[i], ibasis, imu);

         m_Mesh->getVertex(generators[i].back()->getVertex()->getIndex(), vtxTmp);

         // Take the vertex, project onto the plane, and normalize
         project(vtxTmp, ibasis, imu, vtxLast);
         vtxLast[(i==m_Ob)?2:1] = 0;
         normalize<T>(vtxLast, 3);

         for (; it != en; ++it)
         {
            m_Mesh->getVertex((*it)->getVertex()->getIndex(), vtxTmp);
            
            // Take the vertex, project onto the x-y plane, and normalize
            project(vtxTmp, ibasis, imu, vtx);
            vtx[(i==m_Ob)?2:1] = 0;
            normalize<T>(vtx, 3);
            
            // Calculate the angle between these vectors
            cross(vtx, vtxLast, vtxCross);

            T sinTheta = vtxCross[ (i==m_Ob) ? 2 : 1 ];
            T cosTheta = dot<T>(vtx, vtxLast, 3);

            T theta = atan2(sinTheta, cosTheta);
            sumTheta[i] += theta;

            // Store this for next time
            FOREACHAXIS(ax)
               vtxLast[ax] = vtx[ax];
         }

         m_GeneratorDirections[i] = (sumTheta[i] >= 0) ? 1 : -1;
      }

      m_GeneratorDirections[1] = -m_GeneratorDirections[1];

      PRINTVBL(m_Ob);
      PRINTVEC(sumTheta, 2);
   }

   void normalizeMesh(int desiredSide, int desiredObGenDir, int desiredPubGenDir)
   {
      // I want to calculate a transformation which maps the
      // shape into a principal axis-aligned, zero surface mean
      // position

      int mult = 1;
      
      if (desiredSide != 0)
         mult = (m_PelvicSide != desiredSide) ? -1 : 1;
      
      T fbasis[3][3], fmu[3];

      // I want to construct a matrix which does the rotation and reflection in one go
      unprojectParameters(m_Basis, m_Centroids[m_Ob], fbasis, fmu);

      // ... but first I will use the unreflected mesh to transform the basis
      // Project each of the basis axes
      // Don't reflect these!
      {
         //unprojectParameters(m_Basis, m_Centroids[m_Ob], ibasis, imu);
         T newBasis[3][3];
         T zero[] = { 0, 0, 0 };
         FOREACHAXIS(ax)
            project(m_Basis[ax], fbasis, zero, newBasis[ax]);

         assign(newBasis[0], m_Basis[0], 3);
         assign(newBasis[1], m_Basis[1], 3);
         assign(newBasis[2], m_Basis[2], 3);
      }

      // Now! Reflect in the z-axis
      fmu[1] *= mult;
      FOREACHAXIS(ax)
         fbasis[ax][1] *= mult;

      // Transform the mesh
      MeshFunctions<T>::transformMesh(m_Mesh, fbasis, fmu);

      // Project the generator centroids
      for (unsigned int i = 0; i < 2; ++i)
         project(m_Centroids[i], fbasis, fmu);

      m_GeneratorDirections[0] *= mult;
      m_GeneratorDirections[1] *= mult;
      m_PelvicSide             *= mult;

      // Now: the question on everybody's lips is... do we need to correct the
      // generator directions?

      int desiredGenDir[2];
      desiredGenDir[0] = (m_Ob == 0) ? desiredObGenDir : desiredPubGenDir;
      desiredGenDir[1] = (m_Ob == 1) ? desiredObGenDir : desiredPubGenDir;

      for (unsigned int g = 0; g < 2; ++g)
      {
         if (desiredGenDir[g] != 0 && m_GeneratorDirections[g] != desiredGenDir[g])
         {
            flipGenerator(m_Generators[g]);

            m_GeneratorDirections[g] = desiredGenDir[g];
         }
      }
   }

   void flipGenerator(HalfEdgeVec& generator)
   {
      // Twizzle the list around
      generator.push_back(generator[0]);
      generator.push_back(generator[1]);
      generator.erase(generator.begin(), generator.begin()+2);

      // Reverse the list and flip all the edges
      std::reverse(generator.begin(), generator.end());
      for (unsigned int i = 0; i < generator.size(); ++i)
         generator[i] = generator[i]->getPair();
   }

   void calculate(const bool dest)
   {
      // Firstly, let's check to see if we've got a torus!
      if (m_Fg->getNumberOfGeneratorPairs() != 1)
         AWTEXCEPTIONTHROW("This object is not Genus-1");

      // Get the generator pair
      m_Generators[0] = m_Fg->getGenerator(0, 0);
      m_Generators[1] = m_Fg->getGenerator(0, 1);
      
      if (dest)
         m_Mesh = m_Fg->getMesh();
      else
         m_Mesh = MeshImpl<T>::getInstance(m_Fg->getMesh());

      // Calculate the centroids of the generators
      calculateGeneratorCentroid(m_Generators[0], m_Centroids[0]);
      calculateGeneratorCentroid(m_Generators[1], m_Centroids[1]);

      calculateBasis(m_Generators);

      // Now that we have the basis, we can work out which way around the
      // mesh and generator are
      calculatePelvicSide();

      calculateGeneratorDirection(m_Generators);

      normalizeMesh(1, 1, 1);
   }

   typename AWT::GeneratorProvider<T>::P m_Fg;

   T                                  m_Centroids[2][3];
   T                                  m_Basis[3][3];
   unsigned int                       m_Ob;

   int                                m_PelvicSide;
   int                                m_GeneratorDirections[2];

   HalfEdgeVec                        m_Generators[2];

   typename Mesh<T>::P                m_Mesh;
   HalfEdgeMesh::P                    m_HalfEdgeMesh;
};

template <class T>
AWT::PelvicGeneratorDirection<T>::PelvicGeneratorDirection(typename AWT::GeneratorProvider<T>::P fg, const bool dest)
{
   m_D = new D;

   m_D->m_Fg = fg;
   m_D->calculate(dest);

   m_D->m_HalfEdgeMesh = fg->getHalfEdgeMesh();
}  

template <class T>
AWT::PelvicGeneratorDirection<T>::~PelvicGeneratorDirection()
{
   delete m_D;
}

template <class T>
typename AWT::PelvicGeneratorDirection<T>::P AWT::PelvicGeneratorDirection<T>::getInstance(typename GeneratorProvider<T>::P fg, const bool dest)
{
   AUTOGETINSTANCE(AWT::PelvicGeneratorDirection<T>, (fg, dest));
}

template <class T>
GETNAMEMACRO(AWT::PelvicGeneratorDirection<T>);

template <class T>
void AWT::PelvicGeneratorDirection<T>::getBasis(T* centroid, T* x, T* y, T* z) const
{
   for (int i = 0; i < 3; ++i)
   {
      centroid[i] = m_D->m_Centroids[m_D->m_Ob][i];
      x[i] = m_D->m_Basis[0][i];
      y[i] = m_D->m_Basis[1][i];
      z[i] = m_D->m_Basis[2][i];
   }
}

template <class T>
void AWT::PelvicGeneratorDirection<T>::normalizeMesh(int desiredSide, int desiredObGenDirection, int desiredPubGenDirection)
{
   m_D->normalizeMesh(desiredSide, desiredObGenDirection, desiredPubGenDirection);
}

template <class T>
typename AWT::Mesh<T>::P AWT::PelvicGeneratorDirection<T>::getMesh()
{
   return m_D->m_Mesh;
}

template <class T>
const std::vector< AWT::HalfEdge* >&  AWT::PelvicGeneratorDirection<T>::getObturatorGenerator()
{
   return m_D->m_Generators[m_D->m_Ob];
}

template <class T>
const std::vector< AWT::HalfEdge* >& AWT::PelvicGeneratorDirection<T>::getPubicGenerator()
{
   return m_D->m_Generators[1 - m_D->m_Ob];
}

template <class T>
unsigned int AWT::PelvicGeneratorDirection<T>::getNumberOfGeneratorPairs() const
{
   return 1;
}

template <class T>
const std::vector< AWT::HalfEdge* >&  AWT::PelvicGeneratorDirection<T>::getGenerator(unsigned int pair, unsigned int i)
{
   if (pair > 0)
      AWTEXCEPTIONTHROW("Pair index out of bounds");

   switch (i)
   {
   case 0:
      return getObturatorGenerator();
   case 1:
      return getPubicGenerator();
   default:
      AWTEXCEPTIONTHROW("Index out of bounds");
   }
}

template <class T>
AWT::HalfEdgeMesh::P AWT::PelvicGeneratorDirection<T>::getHalfEdgeMesh()
{
   return m_D->m_HalfEdgeMesh;
}

template class AWT::PelvicGeneratorDirection<double>;
template class AWT::PelvicGeneratorDirection<float>;