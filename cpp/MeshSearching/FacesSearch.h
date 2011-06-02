#ifndef __SURFACEMESH_FACESSEARCHER_H__
#define __SURFACEMESH_FACESSEARCHER_H__

#include "MeshSearch.h"
#include "FacesTreeData.h"

namespace AWT
{
   template <class T>
   class FacesSearch : public MeshSearch<T>
   {
   public:
      void setData( typename OEKDTreeData<T,3>::P in_Data );
      typename typename FacesTreeData<T>::P getData( );

   protected:
      typename FacesTreeData<T>::P m_Data;

   };
   
   template <class T>
   void FacesSearch<T>::setData( typename OEKDTree::OEKDTreeData<T,3>::P data )
   {
      m_Data = checkType<OEKDTree::OEKDTreeData<T,3>,FacesTreeData<T>>(data);
   }

   template <class T>
   typename FacesTreeData<T>::P FacesSearch<T>::getData( )
   {
      return m_Data;
   }

}

#endif // __SURFACEMESH_FACESSEARCHER_H__