#ifndef __AXISALIGNEDBOUNDINGBOX_H__
#define __AXISALIGNEDBOUNDINGBOX_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   typedef unsigned char Axis;

   template <class T, unsigned char K>
   class AxisAlignedBoundingBox : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer< AxisAlignedBoundingBox<T,K> > P;

   public:
      /*! Get the minimum extent along the specified axis
      * @param in_Axis The desired axis
      * @return Upper bound on x : p[in_Axis] >= x
      */
      virtual T getMinimumBound( const Axis in_Axis ) const = 0;
      
      /*! Get the maximum extent along the specified axis
      * @param in_Axis The desired axis
      * @return Lower bound on x : p[in_Axis] <= x
      */
      virtual T getMaximumBound( const Axis in_Axis ) const = 0;
   };

   template <class T, unsigned char K>
   class AxisAlignedBoundingBoxImpl : public AxisAlignedBoundingBox<T,K>
   {
   public:
      typedef ManagedAutoPointer<AxisAlignedBoundingBoxImpl<T,K>> P;

   protected:
      AxisAlignedBoundingBoxImpl( )
      {
         m_Bounds = new T[2*K];

         for ( int ax = 0; ax < K; ++ax )
         {
            m_Bounds[2*ax+0] = -100;
            m_Bounds[2*ax+1] =  100;
         }
      }

      virtual ~AxisAlignedBoundingBoxImpl( )
      {
         delete m_Bounds;
      }

   public:
      static P getInstance( )
      {
         AxisAlignedBoundingBoxImpl<T,K>* inst = new AxisAlignedBoundingBoxImpl<T,K>( );
         inst->countMe( );
         AxisAlignedBoundingBoxImpl<T,K>::P ret( inst );
         inst->release( );
         return ret;
      }

      std::string getClassName( ) const
      {
         return "AWT::AxisAlignedBoundingBoxImpl<T,K>";
      }

      virtual void setMinimumBound( const Axis in_Axis, const T v ) 
      {
         m_Bounds[2*in_Axis+0] = v;
      }

      virtual void setMaximumBound( const Axis in_Axis, const T v ) 
      {
         m_Bounds[2*in_Axis+1] = v;
      }

      virtual T getMinimumBound( const Axis in_Axis ) const
      {
         return m_Bounds[2*in_Axis+0];
      }

      virtual T getMaximumBound( const Axis in_Axis ) const
      {
         return m_Bounds[2*in_Axis+1];
      }
      

   protected:
      T* m_Bounds;
   };
}

#endif // __AXISALIGNEDBOUNDINGBOX_H__