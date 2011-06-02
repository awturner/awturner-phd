#ifndef __MOGHARIMEASURE_H__
#define __MOGHARIMEASURE_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh/MeshTypeDefs.h"
#include "Mesh/Tuples.h"
#include "Mesh/Mesh.h"

#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"

namespace AWT
{
   template <class T>
   class MoghariMeasure : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MoghariMeasure> P;

   protected:
      MoghariMeasure( );
      virtual ~MoghariMeasure( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      struct LocalStatistics
      {
         LocalStatistics( )
         {
            covariance.fill( 0 );
            mean.fill( 0 );
            scale = 1;
            npts = 0;
         }

         vnl_matrix_fixed<T,3,3> eigenvectors;
         vnl_matrix_fixed<T,3,3> covariance;
         vnl_vector_fixed<T,3> mean;

         T         scale;

         MeshIndex npts;
      };

      vnl_matrix_fixed<T,3,3> calculateEigenvectors( const vnl_matrix<T>& m );

      void calculateLocalStatistics( typename Mesh<T>::P mesh, const MeshIndex v, const T radius, LocalStatistics* lstats );

      void calculateForVertex( typename Mesh<T>::P mesh, const MeshIndex v, const T radius, vnl_vector<T>& out_Matrix );
      
      typename Tuples<T>::P calculate( typename Mesh<T>::P mesh, const T radius );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __MOGHARIMEASURE_H__