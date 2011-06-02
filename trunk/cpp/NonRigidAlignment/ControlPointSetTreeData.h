#ifndef __CONTROLPOINTSETTREEDATA_H__
#define __CONTROLPOINTSETTREEDATA_H__

#include "ObjectEKDTree/OEKDTreeData.h"
#include "ControlPoint.h"
#include <vector>

namespace AWT
{
   template <class T>
   class ControlPoint;

   template <class T>
   class ControlPointSetTreeData : public OEKDTree::OEKDTreeData<T,3>
   {
   public:
      typedef ManagedAutoPointer< ControlPointSetTreeData<T> > P;

   protected:
      ControlPointSetTreeData( std::vector<typename ControlPoint<T>::P>* vec );
      virtual ~ControlPointSetTreeData( );

   public:
      static P getInstance( std::vector<typename ControlPoint<T>::P>* vec );

      virtual OEKDTree::ObjectIndex getNumberOfObjects( ) const;

      virtual T getValue( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;

      virtual T getCoordinate( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;
      virtual void getPosition( const OEKDTree::ObjectIndex objectIndex, T* pos ) const;

      virtual T getMinimumBound( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;
      virtual T getMaximumBound( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;

      virtual std::string getClassName( ) const;
   protected:
      struct D;
      D* m_D;
   };
}

#endif // __CONTROLPOINTSETTREEDATA_H__