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
#ifndef __CORRESPONDENCEFINDER_H__
#define __CORRESPONDENCEFINDER_H__

#include "ObjectManagement/ManagedObject.h"
#include "Mesh/Pose.h"

namespace AWT
{
   template <class T>
   class Mesh;

   template <class T>
   class CorrespondenceFinder : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<CorrespondenceFinder<T>> P;
   protected:
      CorrespondenceFinder( typename Mesh<T>::P source, typename Mesh<T>::P target, const bool bothWays = false );
      virtual ~CorrespondenceFinder( );

   public:
      static P getInstance( typename Mesh<T>::P source, typename Mesh<T>::P target, const bool bothWays = false );

      virtual void reset( );

      virtual unsigned int getNumberOfCorrespondences( ) const;

      virtual void setCorrespondence( const unsigned int v, const T* sourceVtx, const T* targetVtx, const unsigned int idx );

      virtual bool getCorrespondence( const unsigned int v, T* sourceVtx, T* targetVtx, unsigned int& idx ) const;

      virtual void update( );

      virtual std::string getClassName( ) const;

      virtual T getSumOfSquaredErrors( ) const;

      virtual T getMaxError( ) const;

      virtual T getRmsError( ) const;

      virtual ModifiedTime getModifiedTime( ) const;

      virtual void getPose( const bool scaled, Pose<T>* pose ) const;

      virtual void getRotation( typename Pose<T>::PoseMatrix& R ) const;

      virtual T getScale( ) const;

      virtual void getTranslation( typename Pose<T>::PoseVector& t, bool isScaled ) const;

      virtual bool isIgnoreEdges( ) const;

      virtual void setIgnoreEdges( const bool v );

      virtual void setCompatibilityCosine( const T v );

      virtual T getCompatibilityCosine( ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __CORRESPONDENCEFINDER_H__