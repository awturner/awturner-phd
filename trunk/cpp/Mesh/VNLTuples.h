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
#ifndef __VNLTUPLES_H__
#define __VNLTUPLES_H__

#include "Tuples.h"

#include "vnl/vnl_matrix.h"

// Wraps a VNL Matrix as Tuples
namespace AWT
{
   template <class T>
   class VNLTuples : public Tuples<T>
   {
   public:
      typedef ManagedAutoPointer<VNLTuples<T>> P;

   protected:
      VNLTuples( const vnl_matrix<T>& _matx, const bool _homog )
         : matx( _matx ), homogeneous( _homog )
      {
      }

      VNLTuples( const MeshIndex r, const MeshIndex c, const bool _homog )
         : matx( _homog?(r+1):r, c ), homogeneous( _homog )
      {
      }

      virtual ~VNLTuples( )
      {
      }

   public:
      static P getInstance( const vnl_matrix<T>& matx, const bool homog )
      {
         AUTOGETINSTANCE( AWT::VNLTuples<T>, ( matx, homog ) );
      }

      static P getInstance( const MeshIndex r, const MeshIndex c, const bool homog )
      {
         AUTOGETINSTANCE( AWT::VNLTuples<T>, ( r, c, homog ) );
      }

      virtual std::string getClassName( ) const
      {
         return "AWT::VNLTuples<T>";
      }

      virtual TupleIndex getTupleSize( ) const
      {
         return (homogeneous)?(matx.rows()-1):matx.rows();
      }

      virtual MeshIndex getNumberOfPoints( ) const
      {
         return matx.cols();
      }

      virtual T    getPointElement( const MeshIndex in_Index, const TupleIndex i ) const
      {
         return matx( i, in_Index );
      }

      virtual void getPoint( const MeshIndex in_Index, T* out_Point ) const
      {
         for ( MeshIndex i = 0, imax = getTupleSize(); i < imax; ++i )
            out_Point[i] = matx(i,in_Index);
      }

      virtual void setPointElement( const MeshIndex in_Index, const TupleIndex i, const T in_Value )
      {
         AWTEXCEPTIONTHROW( "Not supported" );
      }

      virtual void setPoint( const MeshIndex in_Index, const T* in_Point )
      {
         AWTEXCEPTIONTHROW( "Not supported" );
      }

      virtual void addPoint( const T* in_Point )
      {
         AWTEXCEPTIONTHROW( "Not supported" );
      }

      virtual void ensureSize( MeshIndex size )
      {
         AWTEXCEPTIONTHROW( "Not supported" );
      }

      virtual void clear( )
      {
         AWTEXCEPTIONTHROW( "Not supported" );
      }

      virtual void lock( void* obj )
      {
      }

      virtual void unlock( void* obj )
      {
      }

      virtual bool isLocked( ) const
      {
         return false;
      }

      virtual const T* getDefault( ) const
      {
         AWTEXCEPTIONTHROW( "Not supported" );
      }

   protected:
      const vnl_matrix<T>& matx;
      const bool homogeneous;
   };
}

#endif // __VNLTUPLES_H__