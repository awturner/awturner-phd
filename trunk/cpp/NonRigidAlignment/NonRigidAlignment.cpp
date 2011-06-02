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
#include "NonRigidAlignment.h"

#include "Mesh/SampledMesh.h"

#include "ControlPoint.h"
#include "ControlPointSearcher.h"
#include "ControlPointSet.h"
#include "CorrespondenceFinder.h"
#include "DeformationAccumulator.h"
#include "GradientAccumulator.h"

#include "Maths/Constants.h"

#include "Mesh/AreaAveragedNormalCalculator.h"
#include "MeshSearching/FacesNearestPointSearch.h"
#include "Mesh/TuplesImpl.h"
#include "Mesh/MeshImpl.h"
#include "MeshSearching/VerticesNearestVertexSearch.h"

#include "MeshAlignmentData.h"

#include "NonRigidAlignmentListener.h"

#include "vnl/algo/vnl_determinant.h"
#include "vnl/algo/vnl_svd.h"

#include "vnl/vnl_matlab_filewrite.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

template <class T>
struct AWT::NonRigidAlignment<T>::D
{
   D() 
   {
   }

   std::vector< typename NonRigidAlignmentListener<T>::P > m_Listeners;
   //CompositeNonRigidAlignmentListener<T> m_Listeners;

   typename MeshAlignmentData<T>::P      m_MeshAlign;
   typename ControlPointSet<T>::P        m_ControlPoints;

   typename CorrespondenceFinder<T>::P   m_CorrFinder;
   
   T                       m_RmsMove;
   T                       m_MaxMove;

   typename Pose<T>::PoseMatrix          m_Rotation;
   typename Pose<T>::PoseVector          m_Translation;
   T                       m_Scale;
};

template <class T>
GETNAMEMACRO(AWT::NonRigidAlignment<T>);

template <class T>
AWT::NonRigidAlignment<T>::NonRigidAlignment()
{
   m_D = new D;

   m_D->m_MeshAlign = MeshAlignmentData<T>::getInstance();

   // TODO: Fix the cyclic reference count which this causes
   m_D->m_MeshAlign->addListener(this);

   m_D->m_Scale = 1;
   m_D->m_Rotation.set_identity();
   m_D->m_Translation.fill(0);
}

template <class T>
AWT::NonRigidAlignment<T>::~NonRigidAlignment()
{
   delete m_D;
}

template <class T>
void AWT::NonRigidAlignment<T>::addListener(typename AWT::NonRigidAlignmentListener<T>::P list)
{
   m_D->m_Listeners.push_back(list);
}

template <class T>
void AWT::NonRigidAlignment<T>::removeListener(typename AWT::NonRigidAlignmentListener<T>::P list)
{
   std::vector< typename NonRigidAlignmentListener<T>::P >::iterator iter = m_D->m_Listeners.begin();
   
   while (iter != m_D->m_Listeners.end())
   {
      if ((*iter).getDataConst() == *list)
         m_D->m_Listeners.erase(iter);
      else
         ++iter;
   }
   
}

template <class T>
bool AWT::NonRigidAlignment<T>::badNumber(T val)
{
   return (val != val) || abs(val) == std::numeric_limits<T>::infinity();
}

template <class T>
typename AWT::NonRigidAlignment<T>::P AWT::NonRigidAlignment<T>::getInstance()
{
   AUTOGETINSTANCE(NonRigidAlignment, ());
}

template <class T>
void AWT::NonRigidAlignment<T>::nonRigidAlignmentStep()
{
   if (*m_D->m_ControlPoints == 0) AWTEXCEPTIONTHROW("Control points not set!");

   // Make sure that we've got points to move!
   if (*m_D->m_MeshAlign->getFixedPoints() == 0)  AWTEXCEPTIONTHROW("Source points not set!");
   if (*m_D->m_MeshAlign->getMovingPoints() == 0) AWTEXCEPTIONTHROW("Target points not set!");

   Mesh<T>::P movingPoints = m_D->m_MeshAlign->getMovingPoints();
   Mesh<T>::P fixedPoints  = m_D->m_MeshAlign->getFixedPoints();

   Mesh<T>::P fixedPointsOriginal = m_D->m_MeshAlign->getFixedPointsOriginal();

   T defPoint[] = { 0, 0, 0 };

   T fixedPoint[3];
   T movingPoint[3];

   T fixedPointOriginal[3];
   T weights[3];

   T pointA[3], pointAOriginal[3];
   T pointB[3], pointBOriginal[3];
   T pointC[3], pointCOriginal[3];

   DeformationAccumulator<T>::P defAccum  = DeformationAccumulator<T>::getInstance(this->getControlPoints());

   ControlPointSearcher<T>::P controlPointSearcher = ControlPointSearcher<T>::getInstance();
   controlPointSearcher->setData(m_D->m_ControlPoints->getTreeData());

   // Find the m_D->m_Correspondences between the two point clouds
   getCorrespondences();

   // Keep the m_D->m_Correspondences' original positions
   Tuples<T>::P corrOriginal = TuplesImpl<T>::getInstance(3, defPoint, m_D->m_CorrFinder->getNumberOfCorrespondences());
   // Keep the control points' current values
   Tuples<T>::P controlValuesNow = TuplesImpl<T>::getInstance(3, defPoint, m_D->m_ControlPoints->getNumberOfObjects());

   GradientAccumulator<T>::P gradAccum = GradientAccumulator<T>::getInstance(this);

   controlPointSearcher->setCallback(gradAccum);

   //for (MeshIndex v = 0; v < movingPoints->getNumberOfVertices(); ++v)
   MESH_EACHVERTEX(movingPoints, v)
   {
      MeshIndex trig;

      bool isValid = m_D->m_CorrFinder->getCorrespondence(v, movingPoint, fixedPoint, trig);

      if (!isValid)
         continue;

      fixedPoints->getFace(trig, pointA, pointB, pointC);

      // This is an inversion of [ pointA pointB pointC ] weights = fixedPoint
      T det = pointA[0]*pointB[1]*pointC[2]-pointA[0]*pointC[1]*pointB[2]-pointA[1]*pointB[0]*pointC[2]+pointA[1]*pointC[0]*pointB[2]+pointA[2]*pointB[0]*pointC[1]-pointA[2]*pointC[0]*pointB[1];
      weights[0] = ((pointB[1]*pointC[2]-pointC[1]*pointB[2])*fixedPoint[0] + (-pointB[0]*pointC[2]+pointC[0]*pointB[2])*fixedPoint[1] + (pointB[0]*pointC[1]-pointC[0]*pointB[1])*fixedPoint[2]) / det;
      weights[1] = ((-pointA[1]*pointC[2]+pointC[1]*pointA[2])*fixedPoint[0] + (pointA[0]*pointC[2]-pointC[0]*pointA[2])*fixedPoint[1] + (-pointA[0]*pointC[1]+pointC[0]*pointA[1])*fixedPoint[2]) / det;
      weights[2] = ((pointA[1]*pointB[2]-pointB[1]*pointA[2])*fixedPoint[0] + (-pointA[0]*pointB[2]+pointB[0]*pointA[2])*fixedPoint[1] + (pointA[0]*pointB[1]-pointB[0]*pointA[1])*fixedPoint[2]) / det;

      // Now we have the weights which give rise to the point in the deformed model...
      // ...so now we need to project the point into the undeformed model
      fixedPointsOriginal->getFace(trig, pointAOriginal, pointBOriginal, pointCOriginal);
      for (int d = 0; d < 3; ++d)
         fixedPointOriginal[d] = weights[0]*pointAOriginal[d] + weights[1]*pointBOriginal[d] + weights[2]*pointCOriginal[d];

      // Record the position of the original point for later
      corrOriginal->setPoint(v, fixedPointOriginal);

      controlPointSearcher->reset();

      // We want to search for control points which influence that the original point location
      controlPointSearcher->setTestPoint(trig, fixedPointOriginal);

      gradAccum->setMovingPoint(movingPoint);
      gradAccum->setFixedPoint(fixedPoint);
      gradAccum->setFaceIndex(trig);

      m_D->m_ControlPoints->search(controlPointSearcher);
   }

   T controlValue[3];
   T grad[3];

   // Record the current control values
   for (MeshIndex v = 0; v < m_D->m_ControlPoints->getNumberOfObjects(); ++v)
   {
      m_D->m_ControlPoints->getControlPoint(v)->getValue(controlValue);
      controlValuesNow->setPoint(v, controlValue);
   }

   controlPointSearcher->setCallback(defAccum);
   T deformedPoint[3];

   const T mus[] = { static_cast<T>(-0.1), static_cast<T>(0), static_cast<T>(0.1) };
   T rsss[3];

   MeshIndex unusedPoints = 0;
   MeshIndex usedPoints   = 0;

   for (MeshIndex v = 0; v < m_D->m_ControlPoints->getNumberOfObjects(); ++v)
   {
      gradAccum->getGradient()->getPoint(v, grad);

      bool used = dot(grad, grad, 3) != 0;

      m_D->m_ControlPoints->getControlPoint(v)->setUsed(used);

      if (used)
         ++usedPoints;
      else
         ++unusedPoints;
   }

   PRINTVBL(unusedPoints);
   PRINTVBL(usedPoints);

   // This does a line search by testing three values of gradient multiplier
   // and then fitting a parabola to the resultant RSS values
   for (int mu_index = 0; mu_index < 3; ++mu_index)
   {
      // Update the control values
      for (MeshIndex v = 0; v < m_D->m_ControlPoints->getNumberOfObjects(); ++v)
      {
         controlValuesNow->getPoint(v, controlValue);

         gradAccum->getGradient()->getPoint(v, grad);

         controlValue[0] += mus[mu_index] * grad[0];
         controlValue[1] += mus[mu_index] * grad[1];
         controlValue[2] += mus[mu_index] * grad[2];

         for (int i = 0; i < 3; ++i)
         {
            if (badNumber(controlValue[i]))
            {
               PRINTVEC(controlValue, 3);
               PRINTVBL(mus[mu_index]);
               PRINTVEC(grad, 3);
               return;
            }  
         }
         
         m_D->m_ControlPoints->getControlPoint(v)->setValue(controlValue);
      }

      // Update the correspondence positions
      rsss[mu_index] = 0;
      for (MeshIndex v = 0, vmax = corrOriginal->getNumberOfPoints(); v < vmax; ++v)
      {
         corrOriginal->getPoint(v, fixedPointOriginal);

         controlPointSearcher->setTestPoint(v, fixedPointOriginal);

         defAccum->reset();
         defAccum->setTestPoint(fixedPointOriginal);

         m_D->m_ControlPoints->search(controlPointSearcher);

         defAccum->getDeformedPoint(deformedPoint);

         movingPoints->getVertex(v, movingPoint);

         for (int i = 0; i < 3; ++i)
         {
            T tmp = deformedPoint[i] - movingPoint[i];
            rsss[mu_index] += tmp*tmp;
         }
      }

      pointA[mu_index] = 1;
      pointB[mu_index] = mus[mu_index];
      pointC[mu_index] = mus[mu_index]*mus[mu_index];

      /*std::cerr << mus[mu_index] << "\t" << rsss[mu_index] << std::endl;*/

      if (badNumber(rsss[mu_index]))
      {
         return;
      }
   }

   // Fit a parabola to the calculated RSS values
   T det = pointA[0]*pointB[1]*pointC[2]-pointA[0]*pointC[1]*pointB[2]-pointA[1]*pointB[0]*pointC[2]+pointA[1]*pointC[0]*pointB[2]+pointA[2]*pointB[0]*pointC[1]-pointA[2]*pointC[0]*pointB[1];

   weights[0] = ((pointB[1]*pointC[2]-pointC[1]*pointB[2])*rsss[0] + (-pointB[0]*pointC[2]+pointC[0]*pointB[2])*rsss[1] + (pointB[0]*pointC[1]-pointC[0]*pointB[1])*rsss[2]) / det;
   weights[1] = ((-pointA[1]*pointC[2]+pointC[1]*pointA[2])*rsss[0] + (pointA[0]*pointC[2]-pointC[0]*pointA[2])*rsss[1] + (-pointA[0]*pointC[1]+pointC[0]*pointA[1])*rsss[2]) / det;
   weights[2] = ((pointA[1]*pointB[2]-pointB[1]*pointA[2])*rsss[0] + (-pointA[0]*pointB[2]+pointB[0]*pointA[2])*rsss[1] + (pointA[0]*pointB[1]-pointB[0]*pointA[1])*rsss[2]) / det;

   // This is the minimum of the parabola
   T muQuad = -weights[1] / (2 * weights[2]);

   if (muQuad != muQuad || abs(muQuad) == std::numeric_limits<T>::infinity())
   {
      DEBUGMACRO("Infinite or NaN muQuad!");
      return;
   }

   for (MeshIndex v = 0; v < m_D->m_ControlPoints->getNumberOfObjects(); ++v)
   {
      //m_D->m_ControlPoints->getControlPoint(v)->getValue(controlValue);
      controlValuesNow->getPoint(v, controlValue);

      gradAccum->getGradient()->getPoint(v, grad);

      controlValue[0] += muQuad*grad[0];
      controlValue[1] += muQuad*grad[1];
      controlValue[2] += muQuad*grad[2];

      m_D->m_ControlPoints->getControlPoint(v)->setValue(controlValue);
   }

   // Now go through and update the points
   updateSourcePoints();

}

template <class T>
void AWT::NonRigidAlignment<T>::updateSourcePoints()
{
   Mesh<T>::P fixedPoints         = m_D->m_MeshAlign->getFixedPoints();
   Mesh<T>::P fixedPointsOriginal = m_D->m_MeshAlign->getFixedPointsOriginal();

   ControlPointSearcher<T>::P   controlPointSearcher = ControlPointSearcher<T>::getInstance();
   controlPointSearcher->setData(m_D->m_ControlPoints->getTreeData());

   DeformationAccumulator<T>::P defAccum             = DeformationAccumulator<T>::getInstance(*m_D->m_ControlPoints);

   T fixedPoint[3];
   T fixedPointOriginal[3];
   T deformedPoint[3];

   T rmsMove = 0;
   T maxMove = 0;

   controlPointSearcher->setCallback(defAccum);

   MESH_EACHVERTEX(fixedPointsOriginal, v)
   {
      fixedPointsOriginal->getVertex(v, fixedPointOriginal);
      controlPointSearcher->setTestPoint(v, fixedPointOriginal);
      fixedPoints->getVertex(v, fixedPoint);

      defAccum->reset();
      defAccum->setTestPoint(fixedPointOriginal);

      m_D->m_ControlPoints->search(controlPointSearcher);

      defAccum->getDeformation(deformedPoint);

      defAccum->getDeformedPoint(deformedPoint);
      fixedPoints->setVertex(v, deformedPoint);

      for (int i = 0; i < 3; ++i)
      {
         T tmp = deformedPoint[i] - fixedPoint[i];
         tmp *= tmp;

         if (tmp > maxMove) maxMove = tmp;
         rmsMove += tmp;
      }
   }

   rmsMove = sqrt(rmsMove / fixedPointsOriginal->getNumberOfVertices());
   maxMove = sqrt(maxMove);

   m_D->m_RmsMove = rmsMove;
   m_D->m_MaxMove = maxMove;

   AreaAveragedNormalCalculator<T>::getInstance()->calculateNormalsAndSet(fixedPoints);

   // Make sure that RMS is up-to-date
   updateCorrespondences();
}

template <class T>
void AWT::NonRigidAlignment<T>::rigidAlignmentStep(const bool allowScale)
{
   // Make sure that we've got points to move!
   if (*m_D->m_MeshAlign->getFixedPoints() == 0)  AWTEXCEPTIONTHROW("Source points not set!");
   if (*m_D->m_MeshAlign->getMovingPoints() == 0) AWTEXCEPTIONTHROW("Target points not set!");

   typename Mesh<T>::P movingPoints = m_D->m_MeshAlign->getMovingPoints();
   typename Mesh<T>::P movingPointsOriginal = m_D->m_MeshAlign->getMovingPointsOriginal();

   // Find the correspondences between the two point clouds
   getCorrespondences();

   typename Pose<T>::PoseMatrix R;
   typename Pose<T>::PoseVector t;

   m_D->m_CorrFinder->getRotation(R);
   m_D->m_CorrFinder->getTranslation(t, allowScale);

   T sc = allowScale ? m_D->m_CorrFinder->getScale() : m_D->m_Scale;

   PRINTVBL(sc);

   T trace = 0;
   for (int row = 0; row < 3; ++row)
      trace += R[row][row];

   // Calculate the angle of rotation from the rotation matrix's trace
   T angle = acos((trace - 1) / 2) * AWT::Constants<T>::RadiansToDegrees;

   PRINTVBL(angle);

   m_D->m_Translation = sc * R * m_D->m_Translation + t;
   m_D->m_Rotation    = R * m_D->m_Rotation;
   m_D->m_Scale       = sc * m_D->m_Scale;

   /*
   PRINTVBLNL(R);
   PRINTVBLNL(t);
   PRINTVBLNL(sc);
   */

   updateTargetPoints();
   updateCorrespondences();
}

template <class T>
void copyTuplesToMatrix(typename AWT::Tuples<T>::P tuples, vnl_matrix<T>& mat)
{
   using namespace AWT;

   const MeshIndex M = tuples->getTupleSize();
   const MeshIndex N = tuples->getNumberOfPoints();

   mat.set_size(M, N);

   T* pnt = new T[M];

   for (MeshIndex c = 0; c < N; ++c)
   {
      tuples->getPoint(c, pnt);

      for (MeshIndex r = 0; r < M; ++r)
         mat(r,c) = pnt[r];
   }

   delete [] pnt;
}

template <class T>
void AWT::NonRigidAlignment<T>::updateTargetPoints()
{
   Mesh<T>::P movingPoints = m_D->m_MeshAlign->getMovingPoints();
   Mesh<T>::P movingPointsOriginal = m_D->m_MeshAlign->getMovingPointsOriginal();

   vnl_vector_fixed<T,4> pntOrig;
   vnl_vector_fixed<T,4> pntT;
   vnl_vector_fixed<T,4> pntCurrent;

   T rmsMove = 0;
   T maxMove = 0;

   
   MESH_EACHVERTEX(movingPoints, v)
   {
      movingPointsOriginal->getVertex(v, pntOrig.data_block());
      pntOrig(3) = 1;

      pntT = m_D->m_Scale * m_D->m_Rotation * pntOrig + m_D->m_Translation;

      movingPoints->getVertex(v, pntCurrent.data_block());
      pntCurrent(3) = 1;
      
      const T moveSquared = (pntOrig - pntCurrent).squared_magnitude();

      rmsMove += moveSquared;
      maxMove = std::max(maxMove, moveSquared);

      movingPoints->setVertex(v, pntT.data_block());
   }

   rmsMove = sqrt(rmsMove / movingPoints->getNumberOfVertices());
   maxMove = sqrt(maxMove);

   m_D->m_RmsMove = rmsMove;
   m_D->m_MaxMove = maxMove;

   // Make sure that RMS is up-to-date
   updateCorrespondences();
}

template <class T>
void AWT::NonRigidAlignment<T>::setSourceMesh(typename AWT::Mesh<T>::P mesh)
{
   m_D->m_MeshAlign->setFixedPoints(mesh);
   modified();
}

template <class T>
typename AWT::Mesh<T>::P AWT::NonRigidAlignment<T>::getSourceMesh() const
{
   return m_D->m_MeshAlign->getFixedPoints();
}

template <class T>
void AWT::NonRigidAlignment<T>::setTargetMesh(typename AWT::Mesh<T>::P mesh)
{
   m_D->m_MeshAlign->setMovingPoints(mesh);
   modified();
}

template <class T>
typename AWT::Mesh<T>::P AWT::NonRigidAlignment<T>::getTargetMesh() const
{
   return m_D->m_MeshAlign->getMovingPoints();
}

template <class T>
typename AWT::Mesh<T>::P AWT::NonRigidAlignment<T>::getProjected() const
{
   Mesh<T>::P referenceMesh = getSourceMesh();
   Mesh<T>::P qqueryMesh    = getTargetMesh();

   Mesh<T>::P outputMesh = MeshImpl<T>::getInstance(qqueryMesh);

   referenceMesh->prepareToSearchVertices();

   FacesNearestPointSearch<T>::P searcher = FacesNearestPointSearch<T>::getInstance();

   T vtx[3], vtxNearest[4];
   for (MeshIndex v = 0; v < qqueryMesh->getNumberOfVertices(); ++v)
   {
      qqueryMesh->getVertex(v, vtx);

      searcher->reset();
      searcher->setTestPoint(vtx);

      referenceMesh->search(searcher);

      searcher->getNearestPoint(vtxNearest);

      vtxNearest[3] = 1;
      outputMesh->setVertex(v, vtxNearest);
   }
   
   return outputMesh;
}


template <class T>
typename AWT::ControlPointSet<T>::P AWT::NonRigidAlignment<T>::getControlPoints()
{
   return m_D->m_ControlPoints;
}

template <class T>
void AWT::NonRigidAlignment<T>::setControlPoints(typename AWT::ControlPointSet<T>::P cps)
{
   m_D->m_ControlPoints = cps;
   modified();

   {
      for (std::vector< typename NonRigidAlignmentListener<T>::P >::iterator it = m_D->m_Listeners.begin();
            it != m_D->m_Listeners.end();
            ++it)
      {
         (*it).getData()->controlPointsChanged(this);
      }
   }
   //m_D->m_Listeners.execute(this, &NonRigidAlignmentListener<T>::controlPointsChanged);
}

template <class T>
typename AWT::MeshAlignmentData<T>::P AWT::NonRigidAlignment<T>::getMeshAlignment()
{
   return m_D->m_MeshAlign;
}

template <class T>
typename AWT::CorrespondenceFinder<T>::P AWT::NonRigidAlignment<T>::getCorrespondences()
{
   Mesh<T>::P movingPoints = m_D->m_MeshAlign->getMovingPoints();
   Mesh<T>::P fixedPoints  = m_D->m_MeshAlign->getFixedPoints();

   ModifiedTime maxMod = std::max<ModifiedTime>(
      movingPoints->getModifiedTime(),
      fixedPoints->getModifiedTime()
  );

   if (
         *m_D->m_CorrFinder == 0 
      || m_D->m_CorrFinder->getModifiedTime() <= maxMod
     )
   {
      updateCorrespondences();
   }
   
   return m_D->m_CorrFinder;
}

template <class T>
void AWT::NonRigidAlignment<T>::updateCorrespondences()
{
   Mesh<T>::P movingPoints = m_D->m_MeshAlign->getMovingPoints();
   Mesh<T>::P fixedPoints  = m_D->m_MeshAlign->getFixedPoints();

   if (*movingPoints == 0 || *fixedPoints == 0)
      return;

   typename NormalCalculator<T>::P nc = AreaAveragedNormalCalculator<T>::getInstance();
   nc->calculateNormalsAndSet(movingPoints);
   
   nc->calculateNormalsAndSet(fixedPoints);

   CorrespondenceFinder<T>::P corrFinder = CorrespondenceFinder<T>::getInstance(movingPoints, fixedPoints);
   
   corrFinder->update();


   m_D->m_CorrFinder = corrFinder;

   m_D->m_CorrFinder->modified();

   {
      for (std::vector< typename NonRigidAlignmentListener<T>::P >::iterator it = m_D->m_Listeners.begin();
            it != m_D->m_Listeners.end();
            ++it)
      {
         (*it).getData()->correspondencesFound(this);
      }
   }
}

template <class T>
T AWT::NonRigidAlignment<T>::getRmsMove() const
{
   return m_D->m_RmsMove;
}

template <class T>
T AWT::NonRigidAlignment<T>::getMaxMove() const
{
   return m_D->m_MaxMove;
}

template <class T>
T AWT::NonRigidAlignment<T>::getRmsError() const
{
   //return m_D->m_RmsError;
   return m_D->m_CorrFinder->getRmsError();
}

template <class T>
T AWT::NonRigidAlignment<T>::getMaxError() const
{
   //return m_D->m_MaxError;
   return m_D->m_CorrFinder->getMaxError();
}

template <class T>
void AWT::NonRigidAlignment<T>::fixedPointsSet(typename MeshAlignmentData<T>::P mad)
{
   if (*mad != *m_D->m_MeshAlign) return;

   {
      for (std::vector< typename NonRigidAlignmentListener<T>::P >::iterator it = m_D->m_Listeners.begin();
            it != m_D->m_Listeners.end();
            ++it)
      {
         (*it).getData()->fixedMeshSet(this);
      }
   }
}

template <class T>
void AWT::NonRigidAlignment<T>::movingPointsSet(typename MeshAlignmentData<T>::P mad)
{
   if (*mad != *m_D->m_MeshAlign) return;

   {
      for (std::vector< typename NonRigidAlignmentListener<T>::P >::iterator it = m_D->m_Listeners.begin();
            it != m_D->m_Listeners.end();
            ++it)
      {
         (*it).getData()->movingMeshSet(this);
      }
   }
}

template <class T>
void AWT::NonRigidAlignment<T>::getRotation(T out_Rotation[3][3]) const
{
   for (int r = 0; r < 3; ++r)
      for (int c = 0; c < 3; ++c)
         out_Rotation[r][c] = m_D->m_Rotation[r][c];
}

template <class T>
T AWT::NonRigidAlignment<T>::getScale() const
{
   return m_D->m_Scale;
}

template <class T>
void AWT::NonRigidAlignment<T>::getTranslation(T out_Translation[3]) const
{
   out_Translation[0] = m_D->m_Translation(0);
   out_Translation[1] = m_D->m_Translation(1);
   out_Translation[2] = m_D->m_Translation(2);
}

template <class T>
void AWT::NonRigidAlignment<T>::setRigidScale(const T in_Scale)
{
   DEBUGMACRO("Setting rigid scale to " << in_Scale);

   // This actually ignores the last line, so only affine transformations can be modelled
   for (int r = 0; r < 3; ++r)
   {
      for (int c = 0; c < 3; ++c)
         m_D->m_Rotation(r, c) = (r==c) ? 1 : 0;

      m_D->m_Translation(r) = 0;
   }

   m_D->m_Scale = in_Scale;

   updateTargetPoints();
}

template <class T>
void AWT::NonRigidAlignment<T>::setRigidTransformation(const T in_Transform[][4])
{
   // This actually ignores the last line, so only affine transformations can be modelled
   for (int r = 0; r < 3; ++r)
   {
      for (int c = 0; c < 3; ++c)
         m_D->m_Rotation(r, c) = in_Transform[r][c];

      m_D->m_Translation(r) = in_Transform[r][3];
   }

   updateTargetPoints();
}

template class AWT::NonRigidAlignment<double>;
template class AWT::NonRigidAlignment<float>;
