#ifndef __SURFACEMESH_POINTSSEARCHER_H__
#define __SURFACEMESH_POINTSSEARCHER_H__

#include "MeshSearch.h"
#include "VerticesTreeData.h"

namespace AWT
{
   template <class T>
   class VerticesSearch : public MeshSearch<T>
   {
   public:
      void setData( typename OEKDTreeData<T,3>::P in_Data );
      typename typename VerticesTreeData<T>::P getData( );

   protected:
      typename VerticesTreeData<T>::P m_Data;

   };

   template <class T>
   void VerticesSearch<T>::setData( typename OEKDTree::OEKDTreeData<T,3>::P data )
   {
      m_Data = checkType<OEKDTree::OEKDTreeData<T,3>,VerticesTreeData<T>>(data);
   }

   template <class T>
   typename VerticesTreeData<T>::P VerticesSearch<T>::getData( )
   {
      return m_Data;
   }
}

#endif // __SURFACEMESH_FACESSEARCHER_H__