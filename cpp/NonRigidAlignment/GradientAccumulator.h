#ifndef __GRADIENTACCUMULATOR_H__
#define __GRADIENTACCUMULATOR_H__

#include "ControlPointSearcherCallback.h"

namespace AWT
{
   template <class T>
   class NonRigidAlignment;

   template <class T>
   class Tuples;

   template <class T>
   class GradientAccumulator : public ControlPointSearcherCallback<T>
   {
   public:
      typedef ManagedAutoPointer<GradientAccumulator<T>> P;

   protected:
      GradientAccumulator( typename NonRigidAlignment<T>::P data );

      virtual ~GradientAccumulator( );

   public:
      static P getInstance( typename NonRigidAlignment<T>::P nra );

      virtual void controlPointFound( const MeshIndex controlIndex );

      Tuples<T>* getGradient( ) const;

      void getMovingPoint( T* out_Point ) const;

      void getFixedPoint( T* out_Point ) const;

      void setMovingPoint( const T* in_Point );

      void setFixedPoint( const T* in_Point );

      MeshIndex getFaceIndex( ) const;

      void setFaceIndex( const MeshIndex idx );

      virtual std::string getClassName( ) const;

   protected:
      struct D;
      D* m_D;
   };
}  

#endif // __GRADIENTACCUMULATOR_H__