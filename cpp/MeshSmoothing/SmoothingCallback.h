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
#ifndef __SMOOTHINGCALLBACK_H__
#define __SMOOTHINGCALLBACK_H__

#include "MeshSearching/FacesInRadiusCallback.h"

#include "ObjectEKDTree/OEKDTypeDefs.h"
#include "ObjectManagement/ManagedObject.h"

#include "Mesh/Mesh.h"
#include "Mesh/Tuples.h"

namespace AWT
{
   namespace OEKDTree
   {
      template <class T, unsigned char K>
      class OEKDTreeData;
   }

   template <class T>
   class SmoothingCallback : public FacesInRadiusCallback<T>
   {
   protected:
      SmoothingCallback( );
      ~SmoothingCallback( );
   public:
      typedef ManagedAutoPointer< SmoothingCallback<T> > P;
      static P getInstance( );

      virtual void faceFound( const typename OEKDTree::OEKDTreeData<T,3>::P data, const OEKDTree::ObjectIndex index );

      virtual void setTestPoint( const T* in_TestPoint );
      virtual void getTestPoint( T* out_TestPoint ) const;

      virtual void setNormals( typename Tuples<T>::P tuples );
      virtual typename Tuples<T>::P getNormals( );

      virtual void setMesh( typename Mesh<T>::P mesh );
      virtual typename Mesh<T>::P getMesh(  );

      virtual void reset( );

      virtual void getPredictedPoint( T* out_PredPoint ) const;

      virtual bool isMollifying( ) const;
      virtual void setMollifying( const bool m );

      virtual void setSpatialSigma( const T sigma );
      virtual T    getSpatialSigma( ) const;

      virtual void setInfluenceSigma( const T sigma );
      virtual T    getInfluenceSigma( ) const;

      virtual std::string getClassName( ) const;
   protected:
      struct D;
      D* m_D;
   };
}

#endif // __SMOOTHINGCALLBACK_H__