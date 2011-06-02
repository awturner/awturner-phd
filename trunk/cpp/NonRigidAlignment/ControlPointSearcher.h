#ifndef __CONTROLPOINTSEARCHER_H__
#define __CONTROLPOINTSEARCHER_H__

#include "ObjectEKDTree/OEKDTypeDefs.h"
#include "Mesh/MeshTypeDefs.h"

#include "ObjectEKDTree/OEKDNearestSearcher.h"

#include "ControlPointSetTreeData.h"

namespace AWT
{
   namespace OEKDTree
   {
      template <class T, unsigned char K>
      class OEKDTreeBranch;

      template <class T, unsigned char K>
      class OEKDTreeData;
   }

   template <class T>
   class ControlPointSearcherCallback;

   template <class T>
   class ControlPointSearcher : public OEKDTree::OEKDNearestSearcher<T,3>
   {
   public:
      typedef ManagedAutoPointer<ControlPointSearcher<T>> P;

   protected:
      ControlPointSearcher( );

      virtual ~ControlPointSearcher( );

   public:
      static P getInstance( );

      virtual void setCallback( typename ControlPointSearcherCallback<T>::P callback );

      virtual void calculateBoxDistanceBounds2( typename OEKDTree::OEKDTreeBranch<T,3>* in_Branch, OEKDTree::SqDistBounds<T>& bounds  ) const;

      virtual void checkObject( const int in_Index );

      virtual bool shouldCheck( OEKDTree::OEKDTreeBranch<T,3> *, const OEKDTree::SqDistBounds<T> &) const;

      virtual void setTestPoint( const MeshIndex in_Index, const T* in_TestPoint  );

      virtual void getTestPoint( T* out_TestPoint ) const;

      virtual std::string getClassName( ) const;

      typename ControlPointSetTreeData<T>::P getData( );
      void setData( typename OEKDTree::OEKDTreeData<T,3>::P data );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __CONTROLPOINTSEARCHER_H__