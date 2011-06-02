#ifndef __CORRESPONDENCEFINDER_H__
#define __CORRESPONDENCEFINDER_H__

#include "ObjectManagement/ManagedObject.h"
#include "Mesh/Pose.h"

namespace AWT
{
   template <class T>
   class Mesh;

   template <class T>
   class CorrespondenceFinder : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<CorrespondenceFinder<T>> P;
   protected:
      CorrespondenceFinder( typename Mesh<T>::P source, typename Mesh<T>::P target, const bool bothWays = false );
      virtual ~CorrespondenceFinder( );

   public:
      static P getInstance( typename Mesh<T>::P source, typename Mesh<T>::P target, const bool bothWays = false );

      virtual void reset( );

      virtual unsigned int getNumberOfCorrespondences( ) const;

      virtual void setCorrespondence( const unsigned int v, const T* sourceVtx, const T* targetVtx, const unsigned int idx );

      virtual bool getCorrespondence( const unsigned int v, T* sourceVtx, T* targetVtx, unsigned int& idx ) const;

      virtual void update( );

      virtual std::string getClassName( ) const;

      virtual T getSumOfSquaredErrors( ) const;

      virtual T getMaxError( ) const;

      virtual T getRmsError( ) const;

      virtual ModifiedTime getModifiedTime( ) const;

      virtual void getPose( const bool scaled, Pose<T>* pose ) const;

      virtual void getRotation( typename Pose<T>::PoseMatrix& R ) const;

      virtual T getScale( ) const;

      virtual void getTranslation( typename Pose<T>::PoseVector& t, bool isScaled ) const;

      virtual bool isIgnoreEdges( ) const;

      virtual void setIgnoreEdges( const bool v );

      virtual void setCompatibilityCosine( const T v );

      virtual T getCompatibilityCosine( ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __CORRESPONDENCEFINDER_H__