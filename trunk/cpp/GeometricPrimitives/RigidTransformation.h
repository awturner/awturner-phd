#ifndef __PRIMITIVE_RIGIDTRANSFORMATION_H__
#define __PRIMITIVE_RIGIDTRANSFORMATION_H__

#include "ObjectManagement/ReferenceCountedObject.h"

namespace AWT
{
   namespace GeometricPrimitives
   {
      class RigidTransformation : public AWT::ReferenceCountedObject
      {
      protected:
         RigidTransformation( );
         virtual ~RigidTransformation( );
      public:
         RigidTransformation* getInstance( );
      };
   }
}

#endif __PRIMITIVE_RIGIDTRANSFORMATION_H__