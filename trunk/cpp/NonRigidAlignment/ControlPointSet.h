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
#ifndef __CONTROLPOINTOEKDDATA_H__
#define __CONTROLPOINTOEKDDATA_H__

#include "ObjectEKDTree/OEKDTreeData.h"

#include "ControlPoint.h"
#include "ControlPointSetTreeData.h"

namespace AWT
{
   namespace OEKDTree
   {
      template <class T, unsigned char K>
      class OEKDSearcher;
   }

   template <class T>
   class ControlPoint;

   template <class T>
   class ControlPointSet : public ManagedObject /* : public OEKDTree::OEKDTreeData<T,3>*/
   {
   public:
      typedef ManagedAutoPointer<ControlPointSet<T>> P;

   protected:
      ControlPointSet( );

      virtual ~ControlPointSet( );

   public:
      static typename ControlPointSet<T>::P getInstance( );
      static typename ControlPointSet<T>::P getInstance( T* bounds, T spacing );

      virtual void add( typename ControlPoint<T>::P cp );

      virtual void add( typename ControlPointSet<T>::P cps );

      virtual void fit( typename ControlPointSet<T>::P cps );

      virtual OEKDTree::ObjectIndex getNumberOfObjects( ) const;

      virtual const typename ControlPoint<T>::P getControlPointConst( OEKDTree::ObjectIndex oi ) const;
         
      virtual typename ControlPoint<T>::P getControlPoint( OEKDTree::ObjectIndex oi );

      virtual T getValue( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;

      virtual T getMinimumBound( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;

      virtual T getMaximumBound( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;

      virtual void search( typename OEKDTree::OEKDSearcher<T,3>::P searcher );

      virtual std::string getClassName( ) const;

      virtual typename ControlPointSetTreeData<T>::P getTreeData( );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __CONTROLPOINTOEKDDATA_H__