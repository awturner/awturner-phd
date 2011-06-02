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
//#ifndef __NEARESTPOINTSEARCH_H__
//#define __NEARESTPOINTSEARCH_H__
//
//#include "EKDTree/EKDSearcher.h"
//#include "EKDTree/PointDataElement.h"
//#include "EKDTree/TriangleDataElement.h"
//#include "EKDTree/EKDTreeBranch.h"
//
//#include <climits>
//
//namespace AWT
//{
//   namespace KDTree
//   {
//      template <class T, unsigned char K>
//      class NearestPointSearch : public EKDSearcher<T,K>
//      {
//      public:
//         NearestPointSearch(T* in_TestPoint = 0, T in_MaxDistance = std::numeric_limits<T>::max());
//
//         virtual T calculateBoxDistanceBounds2(EKDTreeBranch<T,K>* in_Branch);
//         virtual bool shouldCheck(T in_LowerBound);
//
//         virtual bool checkPointOfInterest(const KDTreeDataElement<T,K>* in_Element, const int in_Index);
//
//         virtual void setTestPoint(T* in_TestPoint );
//         virtual void getTestPoint(T* out_TestPoint) const;
//
//         virtual int  getNearestPoint(T* out_NearestPoint = 0) const;
//
//         virtual T    getMaxSearchDistanceSquared() const;
//
//         virtual void setMaximumDistance(T in_MaxDistance);
//         virtual T    getMaximumDistance();
//
//         virtual int  getNumberOfChecks() const;
//         virtual int  getNumberOfBoxChecks() const;
//         virtual void reset();
//
//      protected:
//         virtual void handlePointOfInterest(const T* in_PointOfInterest, const int in_Index);
//
//         virtual bool checkPointOfInterest(const AWT::KDTree::PointDataElement<T,K>* in_Element, T* out_Poi);
//         virtual bool checkPointOfInterest(const AWT::KDTree::TriangleDataElement<T>* in_Element, T* out_Poi);
//
//         int m_NearestIndex;
//         T   m_NearestPoint[K];
//
//         T m_InitMaxDistance2;
//         T m_MaxDistance2;
//         T m_TestPoint[K];
//
//         int m_NumberOfChecks;
//
//         int m_NumberOfBoxChecks;
//      };
//   }
//}
//
//namespace AWT { namespace KDTree {
//
//template <class T, unsigned char K>
//NearestPointSearch<T,K>::NearestPointSearch(T* in_TestPoint, T in_MaxDist)
//{
//   m_NumberOfChecks = 0;
//   m_NearestIndex = -1;
//   m_MaxDistance2 = std::numeric_limits<T>::max();
//
//   if (in_TestPoint != 0)
//      setTestPoint(in_TestPoint);
//
//   setMaximumDistance(in_MaxDist);
//   m_InitMaxDistance2 = m_MaxDistance2;
//}
//
//template <class T, unsigned char K>
//void NearestPointSearch<T,K>::setTestPoint(T* in_TestPoint)
//{
//   for (int i = 0; i < K; i++)
//      m_TestPoint[i] = in_TestPoint[i];
//}
//
//template <class T, unsigned char K>
//void NearestPointSearch<T,K>::getTestPoint(T* out_TestPoint) const
//{
//   for (int i = 0; i < K; i++)
//      out_TestPoint[i] = m_TestPoint[i];
//}
//
//template <class T, unsigned char K>
//int NearestPointSearch<T,K>::getNearestPoint(T* out_Point) const
//{
//   if (m_NearestIndex != -1 && out_Point != 0)
//   {
//      for (int i = 0; i < K; i++)
//         out_Point[i] = m_NearestPoint[i];
//   }
//
//   return m_NearestIndex;
//}
//
//template <class T, unsigned char K>
//void NearestPointSearch<T,K>::setMaximumDistance(T in_MaxDistance)
//{
//   if (in_MaxDistance > sqrt(std::numeric_limits<T>::max()))
//      m_MaxDistance2 = std::numeric_limits<T>::max();
//   else
//      m_MaxDistance2 = in_MaxDistance * in_MaxDistance;
//}
//
//template <class T, unsigned char K>
//T NearestPointSearch<T,K>::getMaximumDistance()
//{
//   return sqrt(m_MaxDistance2);
//}
//
//template <class T, unsigned char K>
//T NearestPointSearch<T,K>::getMaxSearchDistanceSquared() const
//{
//   return m_MaxDistance2;
//}
//
//template <class T, unsigned char K>
//T NearestPointSearch<T,K>::calculateBoxDistanceBounds2(EKDTreeBranch<T,K>* in_Branch)
//{
//   ++m_NumberOfBoxChecks;
//   T ret = 0;
//   T tmp;
//
//   for (int i = 0; i < K; i++)
//   {
//      //std::cerr << "Testing " << m_TestPoint[i] << " against [" << in_Branch->getMinimumBound(i) << "..." << in_Branch->getMaximumBound(i) << "]" << std::endl;
//
//      tmp = std::min(std::max(m_TestPoint[i], in_Branch->getMinimumBound(i)), in_Branch->getMaximumBound(i));
//
//      //std::cerr << in_Branch->getMinimumBound(i) << "->" << in_Branch->getMaximumBound(i) << std::endl;
//      //std::cerr << "Clamping " << m_TestPoint[i] << " to " << tmp << std::endl;
//
//      tmp -= m_TestPoint[i];
//
//      //std::cerr << "Delta = " << tmp << std::endl;
//
//      ret += tmp*tmp;
//
//      if (ret > m_MaxDistance2)
//         break;
//   }
//
//   return ret;
//}
//
//template <class T, unsigned char K>
//bool NearestPointSearch<T,K>::shouldCheck(T in_LowerBound)
//{
//   return in_LowerBound < m_MaxDistance2;
//}
//
//template <class T, unsigned char K>
//void NearestPointSearch<T,K>::handlePointOfInterest(const T *in_PointOfInterest, const int in_Index)
//{
//   T dist2 = 0;
//   T tmp;
//
//   for (int i = 0; i < K; i++)
//   {
//      tmp = in_PointOfInterest[i] - m_TestPoint[i];
//      dist2 += tmp*tmp;
//
//      if (dist2 >= m_MaxDistance2)
//      {
//         return;
//      }
//   }
//
//   //std::cerr << dist2 << std::endl;
//
//   // If we have got this far, we don't need to
//   // compare to m_MaxDistance2
//
//   m_NearestIndex = in_Index;
//
//   for (int i = 0; i < K; i++)
//      m_NearestPoint[i] = in_PointOfInterest[i];
//
//   m_MaxDistance2 = dist2;
//}
//
//#include "EKDTree/SimpleVectorOps.h"
//
//template <class T, unsigned char K>
//bool NearestPointSearch<T,K>::checkPointOfInterest(const PointDataElement<T,K>* in_Element, T* out_Poi)
//{
//   for (int i = 0; i < K; i++)
//      out_Poi[i] = in_Element->GetValue(i);
//
//   return true;
//}
//
//template <class T, unsigned char K>
//bool NearestPointSearch<T,K>::checkPointOfInterest(const TriangleDataElement<T>* in_Element, T* out_NearestPoint)
//{
//   if (K != 3)
//      AWTEXCEPTIONTHROW("This is only defined for 3-dimensional triangles!");
//
//   T point0[3];
//   T point1[3];
//   T point2[3];
//
//   for (int i = 0; i < 3; i++)
//   {
//      point0[i] = in_Element->GetCoordinate(0, i);
//
//      point1[i] = in_Element->GetCoordinate(1, i) - point0[i];
//      point2[i] = in_Element->GetCoordinate(2, i) - point0[i];
//   }
//
//   T p[3] = {
//      m_TestPoint[0]-point0[0], 
//      m_TestPoint[1]-point0[1],
//      m_TestPoint[2]-point0[2],
//   };
//
//   T nml[3];
//   cross(&point1[0], &point2[0], &nml[0]);
//   normalize<double,3>(nml);
//
//   // Check if this cell is degenerate!
//   if (nml[0] != nml[0]  ||  nml[1] != nml[1]  ||  nml[2] != nml[2])
//   {
//      // This is a bit of a cop-out - we can find the closest point on
//      // the line/point
//      return false;
//   }
//
//   T nml_dot_p = dot<double,3>(&nml[0], &p[0]);
//   T np[3] = {
//      p[0] - nml_dot_p * nml[0],
//      p[1] - nml_dot_p * nml[1],
//      p[2] - nml_dot_p * nml[2]
//   };
//
//   T vec0[3] = { point1[0], point1[1], point1[2] };
//   T vec1[3] = { point2[0], point2[1], point2[2] };
//   T vec2[3] = { point2[0]-point1[0], point2[1]-point1[1], point2[2]-point1[2] };
//
//   T nsqvec[3] = {
//      dot<double,3>(&vec0[0], &vec0[0]),
//      dot<double,3>(&vec1[0], &vec1[0]),
//      dot<double,3>(&vec2[0], &vec2[0])
//   };
//
//   T lams[3] = {
//      dot<double,3>(&vec0[0], &np[0]),
//      dot<double,3>(&vec1[0], &np[0]),
//      dot<double,3>(&vec2[0], &np[0]) - dot<double,3>(&vec2[0], &vec0[0])
//   };
//
//   //int resultCase;
//
//   if (lams[0] < 0 && lams[1] < 0)
//   {
//      out_NearestPoint[0] = 0;
//      out_NearestPoint[1] = 0;
//      out_NearestPoint[2] = 0;
//      //resultCase = 1;
//   }
//   else if (lams[0] > nsqvec[0] && lams[2] < 0)
//   {
//      out_NearestPoint[0] = point1[0];
//      out_NearestPoint[1] = point1[1];
//      out_NearestPoint[2] = point1[2];
//      //resultCase = 2;
//   }
//   else if (lams[1] > nsqvec[1] && lams[2] > nsqvec[2])
//   {
//      out_NearestPoint[0] = point2[0];
//      out_NearestPoint[1] = point2[1];
//      out_NearestPoint[2] = point2[2];
//      //resultCase = 3;
//   }
//   else
//   {
//      // This is the determinant of [ vec0 vec1 nml ]
//      T det =   vec0[0] * (vec1[1]*nml[2] - nml[1]*vec1[2])
//              - vec0[1] * (vec1[0]*nml[2] - nml[0]*vec1[2])
//              + vec0[2] * (vec1[0]*nml[1] - nml[0]*vec1[1]);
//
//      // This is the adjoint of [ vec0 vec1 nml ]
//      T mat[3][3] = {
//         {  (vec1[1]*nml[2]-nml[1]*vec1[2]),   -(vec1[0]*nml[2]-nml[0]*vec1[2]),    (vec1[0]*nml[1]-nml[0]*vec1[1])   },
//         { -(vec0[1]*nml[2]-nml[1]*vec0[2]),    (vec0[0]*nml[2]-nml[0]*vec0[2]),   -(vec0[0]*nml[1]-nml[0]*vec0[1])   },
//         {  (vec0[1]*vec1[2]-vec1[1]*vec0[2]), -(vec0[0]*vec1[2]-vec1[0]*vec0[2]),  (vec0[0]*vec1[1]-vec1[0]*vec0[1]) }
//      };
//
//      T proj[3] = {
//         (mat[0][0]*np[0] + mat[0][1]*np[1] + mat[0][2]*np[2]) / det,
//         (mat[1][0]*np[0] + mat[1][1]*np[1] + mat[1][2]*np[2]) / det,
//         (mat[2][0]*np[0] + mat[2][1]*np[1] + mat[2][2]*np[2]) / det,
//      };
//
//      if (lams[0] >= 0 && lams[0] <= nsqvec[0] && proj[1] < 0)
//      {
//         T mult = lams[0] / nsqvec[0];
//
//         out_NearestPoint[0] = 0 + mult * vec0[0];
//         out_NearestPoint[1] = 0 + mult * vec0[1];
//         out_NearestPoint[2] = 0 + mult * vec0[2];
//         //resultCase = 4;
//      }
//      else if (lams[1] >= 0 && lams[1] <= nsqvec[1] && proj[0] < 0)
//      {
//         T mult = lams[1] / nsqvec[1];
//
//         out_NearestPoint[0] = 0 + mult * vec1[0];
//         out_NearestPoint[1] = 0 + mult * vec1[1];
//         out_NearestPoint[2] = 0 + mult * vec1[2];
//         //resultCase = 5;
//      }
//      else if (proj[0]+proj[1] > 1)
//      {
//         T mult = lams[2] / nsqvec[2];
//
//         out_NearestPoint[0] = point1[0] + mult * vec2[0];
//         out_NearestPoint[1] = point1[1] + mult * vec2[1];
//         out_NearestPoint[2] = point1[2] + mult * vec2[2];
//         //resultCase = 6;
//      }
//      else
//      {
//         out_NearestPoint[0] = np[0];
//         out_NearestPoint[1] = np[1];
//         out_NearestPoint[2] = np[2];
//         //resultCase = 0;
//      }
//   }
//
//   p[0] -= out_NearestPoint[0];
//   p[1] -= out_NearestPoint[1];
//   p[2] -= out_NearestPoint[2];
//
//   out_NearestPoint[0] += point0[0];
//   out_NearestPoint[1] += point0[1];
//   out_NearestPoint[2] += point0[2];
//
//   return true;
//}
//
//template <class T, unsigned char K>
//bool NearestPointSearch<T,K>::checkPointOfInterest(const KDTreeDataElement<T,K>* in_Element, const int in_Index)
//{
//   ++m_NumberOfChecks;
//
//   bool ret;
//   T    out_Poi[K];
//
//   const TriangleDataElement<T>* tri = dynamic_cast<const TriangleDataElement<T>*>(in_Element);
//   if (tri != 0)
//   {
//      if (ret = checkPointOfInterest(tri, out_Poi))
//      {
//         handlePointOfInterest(out_Poi, in_Index);
//      }
//
//      return ret;
//   }
//
//   const PointDataElement<T,K>* pde = dynamic_cast<const PointDataElement<T,K>*>(in_Element);
//   if (pde != 0)
//   {
//      if (ret = checkPointOfInterest(pde, out_Poi))
//      {
//         handlePointOfInterest(out_Poi, in_Index);
//      }
//
//      return ret;
//   }
//
//   std::cerr << "Warning: this geometry type is not recognised." << std::endl;
//   
//   return false;
//}
//
//template <class T, unsigned char K>
//int NearestPointSearch<T,K>::getNumberOfBoxChecks() const
//{
//   return m_NumberOfBoxChecks;
//}
//
//template <class T, unsigned char K>
//int NearestPointSearch<T,K>::getNumberOfChecks() const
//{
//   return m_NumberOfChecks;
//}
//
//template <class T, unsigned char K>
//void NearestPointSearch<T,K>::reset()
//{
//   m_NumberOfBoxChecks = 0;
//   m_NumberOfChecks = 0;
//
//   setMaximumDistance(m_InitMaxDistance2);
//}
//
//} }
//
//#endif // __NEARESTPOINTSEARCH_H__