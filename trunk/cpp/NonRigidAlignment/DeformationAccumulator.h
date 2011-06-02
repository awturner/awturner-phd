#ifndef __DEFORMATIONACCUMULATOR_H__
#define __DEFORMATIONACCUMULATOR_H__

#include "NonRigidAlignment.h"
#include "ControlPointSearcher.h"

#include "ControlPointSearcherCallback.h"

namespace AWT
{
   template <class T>
   class DeformationAccumulator : public ControlPointSearcherCallback<T>
   {
   public:
      typedef ManagedAutoPointer<DeformationAccumulator<T>> P;

   protected:
      DeformationAccumulator( typename ControlPointSet<T>::P cps );

      virtual ~DeformationAccumulator( );

   public:
      static P getInstance( typename ControlPointSet<T>::P cps );

      virtual void reset( );

      virtual void controlPointFound( const MeshIndex controlIndex );

      void getDeformedPoint( T* out_Point ) const;

      void getDeformation( T* out_Point ) const;

      void getTestPoint( T* out_Point ) const;

      void setTestPoint( const T* in_Point );

      unsigned int getNumberOfControlPoints( ) const;

      virtual std::string getClassName( ) const;

   protected:
      struct D;
      D* m_D;
   };
}  

#endif // __DEFORMATIONACCUMULATOR_H__