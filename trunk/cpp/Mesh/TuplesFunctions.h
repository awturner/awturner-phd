#ifndef __TUPLESFUNCTIONS_H__
#define __TUPLESFUNCTIONS_H__

#include "ObjectManagement/ManagedObject.h"

#include "Tuples.h"

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_matrix.h>

#include <ostream>

namespace AWT
{
   template <class T>
   class TuplesFunctions : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<TuplesFunctions<T>> P;

   protected:
      TuplesFunctions( );
      virtual ~TuplesFunctions( );

   public:
      static void calculateBounds( const typename Tuples<T>::P tuples, double* out_Bounds );
      static void expandBounds( const typename Tuples<T>::P mesh, double* out_Bounds );
      static void expandBounds( const double* bounds, double* out_Bounds );

      static void transformPoints( typename Tuples<T>::P pts, const vnl_matrix_fixed<T,4,4> r, const bool asDirections = false );
      static void transformPoints( vnl_vector<T>& pts, const vnl_matrix_fixed<T,4,4> r, const bool asDirections = false );

      static typename Tuples<T>::P transform( typename Tuples<T>::P pts, const vnl_matrix<T> R );

      static T calculateRss( const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY );
      static T calculateRms( const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY );

      static T calculateRmsOrtho( const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY, const typename Tuples<T>::P normals, 
                                  const T sigmaParallel, const T sigmaPerp,
                                  T& maxParallel, T& maxPerp );

      static T calculateRssOrtho( const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY, const typename Tuples<T>::P normals, 
                                  const T sigmaParallel, const T sigmaPerp,
                                  T& maxParallel, T& maxPerp );

      static vnl_matrix_fixed<T,4,4> absoluteOrientation( const vnl_vector<T> ptsX, const typename vnl_vector<T> ptsY, const bool scale = false );
      static vnl_matrix_fixed<T,4,4> absoluteOrientation( const typename Tuples<T>::P ptsX, const typename Tuples<T>::P ptsY, const bool scale = false );
      
      static vnl_matrix_fixed<T,4,4> rotationTranslationAboutPoint( const T axis[3], const T theta, const T centre[3], const T translate[3] );

      static void fill( typename Tuples<T>::P pts, const T scalar );

      static void copyToVNLVector( const typename Tuples<T>::P pts, vnl_vector<T>& mat, const MeshIndex element = 0 );
      static void copyToVNLMatrix( const typename Tuples<T>::P pts, vnl_matrix<T>& mat, const bool transpose = false );

   protected:
      static vnl_matrix_fixed<T,4,4> doAbsoluteOrientation( const vnl_matrix_fixed<T,3,3>& sum_xy, const vnl_vector<T>& sum_x, const vnl_vector<T>& sum_y, 
         const T xtx, const T yty, const MeshIndex N, const T doScale );
   };
}


#endif // __TUPLESFUNCTIONS_H__