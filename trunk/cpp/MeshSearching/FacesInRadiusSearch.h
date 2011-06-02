#ifndef __FACESNEARESTPOINTSSEARCHER_H__
#define __FACESNEARESTPOINTSSEARCHER_H__

#include "FacesSearch.h"

namespace AWT
{
   template <class T>
   class FacesInRadiusCallback;

   template <class T>
   class FacesInRadiusSearch : public FacesSearch<T>
   {
   protected:
      FacesInRadiusSearch( );

      virtual ~FacesInRadiusSearch( );

   public:
      typedef ManagedAutoPointer< FacesInRadiusSearch<T> > P;

   public:
      static P getInstance( );

      virtual void setCallback( typename FacesInRadiusCallback<T>::P callback );

      virtual void calculateBoxDistanceBounds2( typename OEKDTreeBranch<T,3>* in_Branch, SqDistBounds<T>& bounds  ) const;

      virtual bool shouldCheck( typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const SqDistBounds<T>& bounds ) const;

      virtual void checkObject( const int in_Index );

      typename OEKDTreeData<T,3>::P getTreeData( );
      void setTreeData( typename OEKDTreeData<T,3>::P data );

      virtual void setRadius( const T radius );

      virtual T    getRadius( ) const;

      virtual void setTestPoint( const T* in_TestPoint  );

      virtual void getTestPoint( T* out_TestPoint ) const;

      virtual std::string getClassName( ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __FACESNEARESTPOINTSSEARCHER_H__