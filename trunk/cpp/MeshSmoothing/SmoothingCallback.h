#ifndef __SMOOTHINGCALLBACK_H__
#define __SMOOTHINGCALLBACK_H__

#include "MeshSearching/FacesInRadiusCallback.h"

#include "ObjectEKDTree/OEKDTypeDefs.h"
#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"

namespace AWT
{
   namespace OEKDTree
   {
      template <class T, unsigned char K>
      class OEKDTreeData;
   }

   template <class T>
   class SmoothingCallback : public FacesInRadiusCallback<T>
   {
   protected:
      SmoothingCallback( );
      ~SmoothingCallback( );
   public:
      typedef ManagedAutoPointer< SmoothingCallback<T> > P;
      static P getInstance( );

      virtual void faceFound( const typename OEKDTree::OEKDTreeData<T,3>::P data, const OEKDTree::ObjectIndex index );

      virtual void setTestPoint( const T* in_TestPoint );
      virtual void getTestPoint( T* out_TestPoint ) const;

      virtual void setNormals( typename Tuples<T>::P tuples );
      virtual typename Tuples<T>::P getNormals( );

      virtual void setMesh( typename Mesh<T>::P mesh );
      virtual typename Mesh<T>::P getMesh(  );

      virtual void reset( );

      virtual void getPredictedPoint( T* out_PredPoint ) const;

      virtual bool isMollifying( ) const;
      virtual void setMollifying( const bool m );

      virtual void setSpatialSigma( const T sigma );
      virtual T    getSpatialSigma( ) const;

      virtual void setInfluenceSigma( const T sigma );
      virtual T    getInfluenceSigma( ) const;

      virtual std::string getClassName( ) const;
   protected:
      struct D;
      D* m_D;
   };
}

#endif // __SMOOTHINGCALLBACK_H__