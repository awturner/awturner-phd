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
#ifndef __TUPLES_H__
#define __TUPLES_H__

#include "ObjectManagement/ManagedObject.h"
#include "Mesh/MeshTypeDefs.h"

#include <ostream>

namespace AWT
{
   typedef unsigned char TupleIndex;

   template <class T>
   class Tuples : public virtual ManagedObject
   {
   public:
      typedef ManagedAutoPointer<Tuples<T>> P;

      virtual TupleIndex getTupleSize( ) const = 0;

      virtual MeshIndex getNumberOfPoints( ) const = 0;

      virtual T    getPointElement( const MeshIndex in_Index, const TupleIndex i ) const = 0;
      virtual void getPoint( const MeshIndex in_Index, T* out_Point ) const = 0;

      virtual void setPointElement( const MeshIndex in_Index, const TupleIndex i, const T in_Value ) = 0;
      virtual void setPoint( const MeshIndex in_Index, const T* in_Point ) = 0;

      virtual void addPoint( const T* in_Point ) = 0;

      virtual void ensureSize( MeshIndex size ) = 0;

      virtual void clear( ) = 0;

      virtual void lock( void* obj ) = 0;
      virtual void unlock( void* obj ) = 0;

      virtual bool isLocked( ) const = 0;

      virtual const T* getDefault( ) const = 0;

      virtual P plus( const P other, const T mult = 1 )
      {
         T* vtxSelf  = new T[this->getTupleSize()];
         T* vtxOther = new T[this->getTupleSize()];

         for ( MeshIndex i = 0, imax = this->getNumberOfPoints( ); i < imax; ++i )
         {
            this->getPoint( i, vtxSelf );
            other->getPoint( i, vtxOther );

            for ( MeshIndex ax = 0; ax < this->getTupleSize(); ++ax )
               vtxSelf[ax] += mult*vtxOther[ax];

            this->setPoint( i, vtxSelf );
         }

         delete [] vtxSelf;
         delete [] vtxOther;

         return P( this );
      }

      virtual P plus( const T* vtxOther, const T mult = 1 )
      {
         T* vtxSelf = new T[this->getTupleSize()];

         for ( MeshIndex i = 0, imax = this->getNumberOfPoints( ); i < imax; ++i )
         {
            this->getPoint( i, vtxSelf );

            for ( MeshIndex ax = 0; ax < this->getTupleSize(); ++ax )
               vtxSelf[ax] += mult*vtxOther[ax];

            this->setPoint( i, vtxSelf );
         }

         delete [] vtxSelf;

         return P( this );
      }

      virtual P scale( const T scalar )
      {
         T* vtxSelf  = new T[this->getTupleSize()];

         for ( MeshIndex i = 0, imax = this->getNumberOfPoints( ); i < imax; ++i )
         {
            this->getPoint( i, vtxSelf );

            for ( MeshIndex ax = 0; ax < this->getTupleSize(); ++ax )
               vtxSelf[ax] *= scalar;

            this->setPoint( i, vtxSelf );
         }

         delete [] vtxSelf;

         return P( this );
      }

      P operator+=( const P other )
      {
         return plus( other, 1 );
      }

      P operator-=( const P other )
      {
         return plus( other, -1 );
      }

      P operator+=( const T* vtxOther )
      {
         return plus( vtxOther, 1 );
      }

      P operator-=( const T* vtxOther )
      {
         return plus( vtxOther, -1 );
      }

      P operator*=( const T scalar )
      {
         return scale( scalar );
      }

      P operator/=( const T scalar )
      {
         return scale( 1/scalar );
      }
   };

   template <class T>
   std::ostream& operator<<( std::ostream& os, const Tuples<T>& tuples )
   {
      os << "[" << std::endl;

      T* data = new T[ tuples.getTupleSize( ) ];

      for ( MeshIndex v = 0, vmax = tuples.getNumberOfPoints( ); v < vmax; ++v )
      {
         tuples.getPoint( v, data );

         for ( MeshIndex a = 0; a < tuples.getTupleSize( ); ++a )
            os << "  " << data[a];

         os << std::endl;
      }

      os << "]" << std::endl;

      delete data;
      return os;
   }
}

#endif // __TUPLES_H__