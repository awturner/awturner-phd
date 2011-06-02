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
//      Fragment( typename Mesh<T>::P mesh )
//      {
//         this->mesh = mesh;
//
//         // Identity Matrix
//         transformation.set_identity( );
//      }
//
//      void resetCorrespondences( )
//      {
//         deformablePoints  = AddTuples<T>::getInstance( 3 );
//         deformableNormals = TuplesImpl<T>::getInstance( 3, selectedVertices->getNumberOfPoints( ) );
//      }
//
//      void transform( vnl_matrix_fixed<T,4,4> t )
//      {
//         TuplesFunctions<T>::transformPoints( points, t );
//         TuplesFunctions<T>::transformPoints( normals, t, true );
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
//      ICPFragment( );
//      virtual ~ICPFragment( );
//
//   public:
//      static P getInstance( );
//      virtual std::string getClassName( ) const;
//
//      void resetCorrespondences( );
//      void transform( vnl_matrix_fixed<T,4,4> t );
//
//   protected:
//      struct D;
//      D* m_D;
//
//   };
//}
//
//#endif // __ICPFRAGMENT_H__