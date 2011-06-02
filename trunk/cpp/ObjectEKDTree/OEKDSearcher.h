#ifndef __OEKDSEARCHER_H__
#define __OEKDSEARCHER_H__

#include "ObjectManagement/ManagedObject.h"

#include "ObjectEKDTree/OEKDTree.h"

namespace AWT
{
   namespace OEKDTree
   {
      template <class T, unsigned char K>
      class OEKDTree;

      template <class T, unsigned char K>
      class OEKDTreeBranch;

      template <class T, unsigned char K>
      class OEKDSearcher : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer< OEKDSearcher<T,K> > P;

      public:
         virtual void searchTree( typename OEKDTree<T,K>* in_Tree ) = 0;

         virtual void setData( typename OEKDTreeData<T,K>::P in_Data ) = 0;

         int  getNumberOfChecks( ) const    { return m_NumberOfChecks; }
         int  getNumberOfBoxChecks( ) const { return m_NumberOfBoxChecks; }
         virtual void reset( ) { m_NumberOfChecks = m_NumberOfBoxChecks = 0; }

      protected:
         void objectChecked() const { ++m_NumberOfChecks; }
         void boxChecked()    const { ++m_NumberOfBoxChecks; }

      private:
         mutable int m_NumberOfChecks;
         mutable int m_NumberOfBoxChecks;
      };

      template <class InputType, class ResultType>
      typename ResultType::P checkType( typename InputType::P in )
      {
         ResultType* casted = dynamic_cast<ResultType*>( in.getData() );
         if (casted == 0)
            throw "Invalid input";

         return ResultType::P(casted);
      }
   }
}

#endif // __OEKDSEARCHER_H__