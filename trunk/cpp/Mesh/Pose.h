#ifndef __MESHPOSE_H__
#define __MESHPOSE_H__

#include "ObjectManagement/ManagedObject.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_matrix.h"

namespace AWT
{
   template <class T>
   class Pose : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<Pose<T>> P;

      typedef vnl_matrix_fixed<T,4,4> PoseMatrix;
      typedef vnl_vector_fixed<T,4>   PoseVector;

   protected:
      Pose( );
      virtual ~Pose( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      virtual PoseMatrix getMatrix( ) const;

      virtual void setMatrix( const PoseMatrix& matrix );

      virtual void project( const T* in_Point, T* out_Point, const T implicit = 1 ) const;

      virtual P getRelativePose( const P pose ) const;

      virtual P getInversePose( ) const;

      virtual void identity( );

      virtual void translate( const T* trans );

      virtual void rotateDeg( const T theta, const T* axis );

      virtual void rotateDeg( const T theta, const T* axis, const T* centre );

      virtual void rotate( const T theta, const T* axis );

      virtual void rotate( const T theta, const T* axis, const T* centre );

      virtual void scale( const T sc );

   protected:
      struct D;
      D* m_D;

   };
}

template <class T>
vnl_matrix<T> operator*( const AWT::Pose<T>& pose, const vnl_matrix<T>& points );

#endif // __MESHPOSE_H__