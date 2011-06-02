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
#include "DeformableMeshFitting.h"

#include "DeformableMesh/DeformableMesh.h"

#include "DeformableMeshFittingListener.h"

#include "DrawableFramework/Drawable.h"

#include "Maths/Constants.h"

#include "Mesh/AreaAveragedNormalCalculator.h"
#include "Mesh/AreaAveragedNormalCalculator.h"
#include "Mesh/Mesh.h"
#include "Mesh/Pose.h"
#include "Mesh/SamplePoints.h"
#include "Mesh/SampledMesh.h"
#include "Mesh/Tuples.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix_fixed.h"

#include "vnl/algo/vnl_svd.h"

#include "MoghariMeasure.h"

#include "NonRigidAlignment/CorrespondenceFinder.h"

#include "Useful/ArrayVectorFunctions.h"
#include "Useful/Exception.h"
#include "Useful/PrintMacros.h"

#include "vld.h"
#include <fstream>
#include <limits>
#include <queue>

#include "Fragment.h"

template <class T>
struct AWT::DeformableMeshFitting<T>::D
{
   D(DeformableMeshFitting<T>* ptr)
   {
      m_Parent = ptr;
   }

   ~D()
   {
   }

   void addFragment(typename Mesh<T>::P mesh, SamplePoints::P sampPts)
   {
      m_Fragments.push_back(Fragment<T>::getInstance(mesh, sampPts));
   }

   void rigidUpdate()
   {
      std::vector<typename Fragment<T>::P>::iterator iter    = m_Fragments.begin();
      std::vector<typename Fragment<T>::P>::iterator endIter = m_Fragments.end();

      for (; iter != endIter; ++iter)
      {
         (*iter)->updateRigid();
      }
   }

   void updateCorrespondences()
   {
      std::vector<typename Fragment<T>::P>::iterator iter    = m_Fragments.begin();
      std::vector<typename Fragment<T>::P>::iterator endIter = m_Fragments.end();

      for (; iter != endIter; ++iter)
      {
         CorrespondenceFinder<T>* corrFinder = (*iter)->getCorrespondenceFinder();

         if (corrFinder == 0 || corrFinder->getModifiedTime() < (*iter)->getMesh()->getModifiedTime()
            || corrFinder->getModifiedTime() < m_DeformableMesh->getModifiedTime())
         {
            (*iter)->createCorrespondenceFinder(*m_DeformableMesh);
         }
      }

      {
         for (std::vector< typename DeformableMeshFittingListener<T>::P >::iterator it = m_Listeners.begin();
               it != m_Listeners.end();
               ++it)
         {
            (*it).getData()->correspondencesFound(m_Parent);
         }
      }
      //m_Listeners.execute(m_Parent, &DeformableMeshFittingListener<T>::correspondencesFound);
   }

   DeformableMeshFitting<T>*                 m_Parent;

   typename DeformableMesh<T>::P             m_DeformableMesh;

   std::vector< typename Fragment<T>::P >       m_Fragments;

   bool                                      m_Loading;

   //CompositeDeformableMeshFittingListener<T> m_Listeners;
   std::vector< typename DeformableMeshFittingListener<T>::P > m_Listeners;
};

template <class T>
AWT::DeformableMeshFitting<T>::DeformableMeshFitting()
{
   m_D = new D(this);

   m_D->m_Loading        = false;
}

template <class T>
AWT::DeformableMeshFitting<T>::~DeformableMeshFitting()
{
   delete m_D;
}

template <class T>
typename AWT::DeformableMeshFitting<T>::P AWT::DeformableMeshFitting<T>::getInstance()
{
   AUTOGETINSTANCE(AWT::DeformableMeshFitting<T>, ());
}

template <class T>
GETNAMEMACRO(AWT::DeformableMeshFitting<T>);

template <class T>
void AWT::DeformableMeshFitting<T>::addTargetMesh(typename AWT::Mesh<T>::P mesh, AWT::SamplePoints::P sampPts)
{
   m_D->addFragment(mesh, sampPts);
}

template <class T>
void AWT::DeformableMeshFitting<T>::setDeformableMesh(typename AWT::DeformableMesh<T>::P defMesh)
{
   m_D->m_DeformableMesh = defMesh;
}

template <class T>
AWT::TargetIndex AWT::DeformableMeshFitting<T>::getNumberOfTargetMeshes() const
{
   return static_cast<TargetIndex>(m_D->m_Fragments.size());
}

template <class T>
typename AWT::Fragment<T>::P AWT::DeformableMeshFitting<T>::getFragment(TargetIndex i)
{
   return m_D->m_Fragments[i];
}

template <class T>
typename AWT::DeformableMesh<T>::P AWT::DeformableMeshFitting<T>::getDeformableMesh()
{
   return *m_D->m_DeformableMesh;
}

template <class T>
void AWT::DeformableMeshFitting<T>::updateCorrespondences()
{
   m_D->updateCorrespondences();
}

template <class T>
void AWT::DeformableMeshFitting<T>::stepRigidUpdate()
{
   VLDEnable();
   m_D->rigidUpdate();
   VLDDisable();

//#ifdef _DEBUG
//   DEBUGLINEANDEXIT(1);
//#endif
}

template <class T>
void AWT::DeformableMeshFitting<T>::stepParameterUpdate()
{
   VLDEnable();

   unsigned int nmodes = m_D->m_DeformableMesh->getNumberOfActiveModes();

   T* modeWeights  = new T[ nmodes ];

   m_D->m_DeformableMesh->getModeWeights(modeWeights);
   
   vnl_matrix<T> A(nmodes, nmodes);
   vnl_vector<T> b(nmodes);
   
   T pntTarget[3];
   T pntDeform[3];

   MeshIndex trigIdx;

   A.fill(0);
   b.fill(0);

   vnl_matrix<T> jac(3, nmodes);
   vnl_vector<T> delta(3);
   vnl_vector<T> deltad(3);

   std::vector<typename Fragment<T>::P>::iterator iter    = m_D->m_Fragments.begin();
   std::vector<typename Fragment<T>::P>::iterator iterEnd = m_D->m_Fragments.end();

   double jacTime = 0, corrTime = 0, solveTime = 0, transTime = 0, incTime = 0;

   for (; iter != iterEnd; ++iter)
   {
      CorrespondenceFinder<T>* corrFinder = (*iter)->getCorrespondenceFinder();

      MeshIndex ncorrs = corrFinder->getNumberOfCorrespondences();

      if (true)
      {
         // This is the point-to-point cost function
         for (MeshIndex v = 0; v < ncorrs; ++v)
         {
            bool isValid = corrFinder->getCorrespondence(v, pntTarget, pntDeform, trigIdx);
            if (!isValid) continue;

            delta.set(pntTarget);

            deltad.set(pntDeform);
            delta -= deltad;

            m_D->m_DeformableMesh->getJacobian(trigIdx, pntDeform, jac);

            vnl_matrix<T> jact = jac.transpose();
            
            A += jact * jac;
            b += jact * delta;
         }
      }
      else
      {
         // This is the point-to-plane cost function
         T pntNormal[3];
         vnl_vector<T> normal(3, 1);

         bool nanNormal = false;

         for (MeshIndex v = 0; v < ncorrs; ++v)
         {
            if (v % 10000 == 0)
               DEBUGMACRO(v << "/" << ncorrs);

            bool isValid = corrFinder->getCorrespondence(v, pntTarget, pntDeform, trigIdx);
            if (!isValid) continue;

            // Calculate the delta between the points
            delta.set(pntTarget);

            deltad.set(pntDeform);
            delta -= deltad;

            // Retrieve the normal at this vertex
            (*iter)->getMesh()->getVertexNormals()->getPoint(v, pntNormal);
            normalize(pntNormal, 3);
            normal.set(pntNormal);

            // Get the jacobian 
            m_D->m_DeformableMesh->getJacobian(trigIdx, pntDeform, jac);

            vnl_matrix<T> jact = jac.transpose();

            vnl_vector<T> jactn = jact * normal;

            A += outer_product(jactn, jactn);
            b += outer_product(jactn, normal) * delta;
         }

         if (nanNormal)
         {
            DEBUGMACRO("At least one of the normals was NaN... sort this out!");
         }
         
         //PRINTVBL(A);
         //PRINTVBL(b);
      }
   }

   vnl_svd<T> svd(A);
   vnl_vector<T> x = svd.solve(b);

   for (unsigned int i = 0; i < nmodes; ++i)
      modeWeights[i] += x(i);

   m_D->m_DeformableMesh->setModeWeights(modeWeights);

   //PRINTVEC(modeWeights, nmodes);

   delete[] modeWeights;

   AreaAveragedNormalCalculator<T>::getInstance()->calculateNormalsAndSet(*m_D->m_DeformableMesh);
   
   //AWT::Drawable::releaseMutex(this);

   //DEBUGMACRO("Updating correspondences...");
   updateCorrespondences();
   //DEBUGMACRO("Done.");
   
   VLDDisable();
}

template <class T>
T AWT::DeformableMeshFitting<T>::getRmsError() const
{
   std::vector<typename Fragment<T>::P>::iterator iter    = m_D->m_Fragments.begin();
   std::vector<typename Fragment<T>::P>::iterator iterEnd = m_D->m_Fragments.end();

   T sumOfSquaredErrors = 0;
   unsigned int numberOfCorrespondences = 0;

   for (; iter != iterEnd; ++iter)
   {
      CorrespondenceFinder<T>* corrFinder = (*iter)->getCorrespondenceFinder();

      if (corrFinder != 0)
      {
         sumOfSquaredErrors      += corrFinder->getSumOfSquaredErrors();
         numberOfCorrespondences += corrFinder->getNumberOfCorrespondences();
      }
      else
      {
         return std::numeric_limits<T>::infinity();
      }
   }

   return sqrt(sumOfSquaredErrors / numberOfCorrespondences);
}

template <class T>
T AWT::DeformableMeshFitting<T>::getMaxError() const
{
   std::vector<typename Fragment<T>::P>::iterator iter    = m_D->m_Fragments.begin();
   std::vector<typename Fragment<T>::P>::iterator iterEnd = m_D->m_Fragments.end();

   T maxError = 0;

   for (; iter != iterEnd; ++iter)
   {
      CorrespondenceFinder<T>* corrFinder = (*iter)->getCorrespondenceFinder();

      if (corrFinder != 0)
      {
         maxError = std::max<T>(maxError, corrFinder->getMaxError());
      }
      else
      {
         return std::numeric_limits<T>::infinity();
      }
   }

   return maxError;
}

template <class T>
void AWT::DeformableMeshFitting<T>::setInitialPose(const TargetIndex i, typename Pose<T>::P pose)
{
   (m_D->m_Fragments[i])->setInitialPose(pose);
}

//template <class T>
//void AWT::DeformableMeshFitting<T>::setInitialRotation(const TargetIndex i, Matrix<T>& R)
//{
//   (m_D->m_Fragments[i])->setInitialRotation(R);
//}
//
//template <class T>
//void AWT::DeformableMeshFitting<T>::setInitialTranslation(const TargetIndex i, Matrix<T>& t)
//{
//   (m_D->m_Fragments[i])->setInitialTranslation(t);
//}

template <class T>
void AWT::DeformableMeshFitting<T>::getInitialPose(const TargetIndex i, typename Pose<T>::P pose)
{
   (m_D->m_Fragments[i])->getInitialPose(pose);
}

template <class T>
void AWT::DeformableMeshFitting<T>::getPose(const AWT::TargetIndex i, typename AWT::Pose<T>::P pose) const
{
   (m_D->m_Fragments[i])->getPose(pose);
}

template <class T>
void AWT::DeformableMeshFitting<T>::setInitialParameters(const T* params)
{
   m_D->m_DeformableMesh->setModeWeights(params);
}

template <class T>
typename AWT::CorrespondenceFinder<T>::P AWT::DeformableMeshFitting<T>::getCorrespondences(const AWT::TargetIndex i)
{
   return m_D->m_Fragments[i]->getCorrespondenceFinder();
}

template <class T>
void AWT::DeformableMeshFitting<T>::addListener(typename AWT::DeformableMeshFittingListener<T>::P listener)
{
   m_D->m_Listeners.push_back(listener);
}

template <class T>
void AWT::DeformableMeshFitting<T>::removeListener(typename AWT::DeformableMeshFittingListener<T>::P list)
{
   std::vector< typename DeformableMeshFittingListener<T>::P >::iterator iter = m_D->m_Listeners.begin();
   
   while (iter != m_D->m_Listeners.end())
   {
      if ((*iter).getDataConst() == *list)
         m_D->m_Listeners.erase(iter);
      else
         ++iter;
   }
}

template <class T>
void AWT::DeformableMeshFitting<T>::setLoading(const bool v)
{
   m_D->m_Loading = v;
}

template <class T>
typename AWT::Fragment<T>::P AWT::Fragment<T>::getInstance(typename AWT::Mesh<T>::P mesh, AWT::SamplePoints::P sampPts)
{
   AUTOGETINSTANCE(AWT::Fragment<T>, (mesh, sampPts));
}

template <class T>
void AWT::DeformableMeshFitting<T>::calculateRegistrationError(const AWT::TargetIndex a, const AWT::TargetIndex b, T& angularError, T& translationalError)
{
   Fragment<T>::calculateRegistrationError(getFragment(a), getFragment(b), angularError, translationalError);
}

template <class T>
GETNAMEMACRO(AWT::Fragment<T>);

template class AWT::DeformableMeshFitting<double>;
template class AWT::DeformableMeshFitting<float>;
