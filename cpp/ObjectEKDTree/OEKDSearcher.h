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
         virtual void searchTree(typename OEKDTree<T,K>* in_Tree) = 0;

         virtual void setData(typename OEKDTreeData<T,K>::P in_Data) = 0;

         int  getNumberOfChecks() const    { return m_NumberOfChecks; }
         int  getNumberOfBoxChecks() const { return m_NumberOfBoxChecks; }
         virtual void reset() { m_NumberOfChecks = m_NumberOfBoxChecks = 0; }

      protected:
         void objectChecked() const { ++m_NumberOfChecks; }
         void boxChecked()    const { ++m_NumberOfBoxChecks; }

      private:
         mutable int m_NumberOfChecks;
         mutable int m_NumberOfBoxChecks;
      };

      template <class InputType, class ResultType>
      typename ResultType::P checkType(typename InputType::P in)
      {
         ResultType* casted = dynamic_cast<ResultType*>(in.getData());
         if (casted == 0)
            throw "Invalid input";

         return ResultType::P(casted);
      }
   }
}

#endif // __OEKDSEARCHER_H__