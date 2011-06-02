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
#ifndef __FACESNEARESTPOINTSSEARCHER_H__
#define __FACESNEARESTPOINTSSEARCHER_H__

#include "FacesSearch.h"

namespace AWT
{
   template <class T>
   class FacesInRadiusCallback;

   template <class T>
   class FacesInRadiusSearch : public FacesSearch<T>
   {
   protected:
      FacesInRadiusSearch( );

      virtual ~FacesInRadiusSearch( );

   public:
      typedef ManagedAutoPointer< FacesInRadiusSearch<T> > P;

   public:
      static P getInstance( );

      virtual void setCallback( typename FacesInRadiusCallback<T>::P callback );

      virtual void calculateBoxDistanceBounds2( typename OEKDTreeBranch<T,3>* in_Branch, SqDistBounds<T>& bounds  ) const;

      virtual bool shouldCheck( typename AWT::OEKDTree::OEKDTreeBranch<T,3>* in_Branch, const SqDistBounds<T>& bounds ) const;

      virtual void checkObject( const int in_Index );

      typename OEKDTreeData<T,3>::P getTreeData( );
      void setTreeData( typename OEKDTreeData<T,3>::P data );

      virtual void setRadius( const T radius );

      virtual T    getRadius( ) const;

      virtual void setTestPoint( const T* in_TestPoint  );

      virtual void getTestPoint( T* out_TestPoint ) const;

      virtual std::string getClassName( ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __FACESNEARESTPOINTSSEARCHER_H__