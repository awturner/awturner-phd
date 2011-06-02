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
//         virtual T    calculateBoxDistanceBounds2( EKDTreeBranch<T,K>* in_Branch  ) = 0;
//
//         virtual bool checkPointOfInterest( const KDTreeDataElement<T,K>* in_Element, const int in_Index ) = 0;
//
//         virtual bool shouldCheck( T in_LowerBound = 0 ) = 0;
//
//         virtual T    getMaxSearchDistanceSquared( ) const = 0;
//
//         virtual int  getNumberOfChecks( ) const = 0;
//
//         virtual int  getNumberOfBoxChecks( ) const = 0;
//
//         virtual void reset( ) = 0;
//      };
//   }
//}
//
//#endif // __EKDSEARCHER_H__