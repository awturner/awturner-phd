#ifndef __SHAPEMODEL_H__
#define __SHAPEMODEL_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class ShapeModel : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<ShapeModel> P;

      protected:
         ShapeModel( Mesh::P meanMesh );
         virtual ~ShapeModel( );

      public:
         static P getInstance( Mesh::P meanMesh );
         virtual std::string getClassName( ) const;

         Mesh::P getMesh( const Vector& weights );

         void addMode( const Points& mode );

         Index getNumberOfModes( ) const;

         void activateAllModes( );
         void setNumberOfModes( const Index n );

         void jacobian( const Index v, Matrix& jac );
         void jacobian( const Index f, const ConvexWeights& ws, Matrix& jac );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __SHAPEMODEL_H__