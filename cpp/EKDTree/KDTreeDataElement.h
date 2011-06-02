/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
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
