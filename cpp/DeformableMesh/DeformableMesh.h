#ifndef __DEFORMABLEMESH_H__
#define __DEFORMABLEMESH_H__

#include "Mesh/Mesh.h"
#include "vnl/vnl_matrix.h"

namespace AWT
{
   /*! \class DeformableMesh
   \brief Deformable Mesh, i.e. a shape model

   Abstract representation of a shape model.
   */

   template <class T>
   class DeformableMesh : public Mesh<T>
   {
   public:
      typedef ManagedAutoPointer< DeformableMesh<T> > P;

   public:
      virtual MeshIndex getNumberOfActiveModes( ) const = 0;

      virtual void setActiveModes( const MeshIndex m ) = 0;
      virtual void activateAllModes( ) = 0;

      virtual void setModeWeight( const MeshIndex m, const T weight ) = 0;

      virtual T getModeWeight( const MeshIndex m ) const = 0;

      virtual void setModeWeights( const T* weights ) = 0;

      virtual void getModeWeights( T* weights ) const = 0;

      virtual typename Tuples<T>::P getMode( const MeshIndex m ) = 0;

      virtual void getJacobian( const MeshIndex v, T* pnt, vnl_matrix<T>& jacobian ) const = 0;
   };
}

#endif // __DEFORMABLEMESH_H__