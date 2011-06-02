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
//#ifndef __ICPFRAGMENT_H__
//#define __ICPFRAGMENT_H__
//
//#include "ObjectManagement/ManagedObject.h"
//
//#include "Mesh/Mesh.h"
//#incldue "DeformableMesh/DeformableMesh.H
//
//   struct Fragment
//   {
//      Fragment(typename Mesh<T>::P mesh)
//      {
//         this->mesh = mesh;
//
//         // Identity Matrix
//         transformation.set_identity();
//      }
//
//      void resetCorrespondences()
//      {
//         deformablePoints  = AddTuples<T>::getInstance(3);
//         deformableNormals = TuplesImpl<T>::getInstance(3, selectedVertices->getNumberOfPoints());
//      }
//
//      void transform(vnl_matrix_fixed<T,4,4> t)
//      {
//         TuplesFunctions<T>::transformPoints(points, t);
//         TuplesFunctions<T>::transformPoints(normals, t, true);
//         
//         transformation = t * transformation;
//      }
//
//      typename Mesh<T>::P      mesh;
//      Tuples<MeshIndex>::P     selectedVertices;
//
//      typename Tuples<T>::P    points;
//      typename Tuples<T>::P    normals;
//
//      typename AddTuples<T>::P deformablePoints;
//      typename Tuples<T>::P    deformableNormals;
//
//      vnl_matrix_fixed<T,4,4>  transformation;
//   };
//
//
//namespace AWT
//{
//   class ICPFragment : public ManagedObject
//   {
//   public:
//      typedef ManagedAutoPointer<ICPFragment> P;
//
//   protected:
//      ICPFragment();
//      virtual ~ICPFragment();
//
//   public:
//      static P getInstance();
//      virtual std::string getClassName() const;
//
//      void resetCorrespondences();
//      void transform(vnl_matrix_fixed<T,4,4> t);
//
//   protected:
//      struct D;
//      D* m_D;
//
//   };
//}
//
//#endif // __ICPFRAGMENT_H__