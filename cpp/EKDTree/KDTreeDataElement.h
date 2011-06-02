//#ifndef __KDTreeDataElement_H__
//#define __KDTreeDataElement_H__
//
//namespace AWT
//{
//   namespace KDTree
//   {
//      template <class T, unsigned char K>
//      class KDTreeDataElement
//      {
//      public:
//         virtual const unsigned char GetDimensionality( ) { return K; }
//
//         /*! Return the sort value on the given axis
//          * @param in_Axis Axis to return value from
//          */
//         virtual T GetValue( const int in_Axis ) const = 0;
//
//         /*! Return the minimum value of the bounding
//          * box on the specified axis
//          * @param in_Axis Axis to return value from
//          */
//         virtual T GetMinBound( const int in_Axis ) const= 0;
//
//         /*! Return the maximum value of the bounding
//          * box on the specified axis
//          * @param in_Axis Axis to return value from
//          */
//         virtual T GetMaxBound( const int in_Axis ) const = 0;
//
//         /*! Make sure that the data element is up-to-date
//          */
//         virtual void update( ) = 0;
//
//         static int QueryCount;
//
//      protected:
//         /*
//         static T clamp( T val, T min, T max )
//         {
//            if ( val < min )
//               return min;
//
//            if ( val > max )
//               return max;
//
//            return val;
//         }
//         */
//      };
//
//      template <class T, unsigned char K>
//      int KDTreeDataElement<T,K>::QueryCount = 0;
//   }
//}
//
//#endif // __KDTreeDataElement_H__
