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
//#ifndef __EKDSEARCHER_H__
//#define __EKDSEARCHER_H__
//
//namespace AWT
//{
//   namespace KDTree
//   {
//      template <class T, unsigned char K>
//      class EKDTreeBranch;
//
//      template <class T, unsigned char K>
//      class KDTreeDataElement;
//
//      template <class T, unsigned char K>
//      class EKDSearcher
//      {
//      public:
//         virtual T    calculateBoxDistanceBounds2(EKDTreeBranch<T,K>* in_Branch ) = 0;
//
//         virtual bool checkPointOfInterest(const KDTreeDataElement<T,K>* in_Element, const int in_Index) = 0;
//
//         virtual bool shouldCheck(T in_LowerBound = 0) = 0;
//
//         virtual T    getMaxSearchDistanceSquared() const = 0;
//
//         virtual int  getNumberOfChecks() const = 0;
//
//         virtual int  getNumberOfBoxChecks() const = 0;
//
//         virtual void reset() = 0;
//      };
//   }
//}
//
//#endif // __EKDSEARCHER_H__