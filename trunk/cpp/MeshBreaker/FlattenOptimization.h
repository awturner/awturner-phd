#ifndef __FLATTENOPTIMIZATION_H__
#define __FLATTENOPTIMIZATION_H__

#include "ObjectManagement/ManagedObject.h"

#include "FlattenMeshPair.h"
#include "NonRigidAlignment/ControlPoint.h"

#include "vnl/vnl_matrix.h"

namespace AWT
{
   template <class T>
   class FlattenOptimization : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<FlattenOptimization> P;

   protected:
      FlattenOptimization( typename AWT::FlattenMeshPair<T>::P flatten, 
                                                  typename AWT::Tuples<T>::P referencePoints,
                                                  std::vector< typename ControlPoint<T>::P > controlPoints );
      virtual ~FlattenOptimization( );

   public:
      static P getInstance( typename AWT::FlattenMeshPair<T>::P flatten, 
                                                  typename AWT::Tuples<T>::P referencePoints,
                                                  std::vector< typename ControlPoint<T>::P > controlPoints );
      virtual std::string getClassName( ) const;

      virtual void getControlParameters( vnl_matrix<T>& v );

      virtual void incrementControlParameters( vnl_matrix<T>& v );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __FLATTENOPTIMIZATION_H__