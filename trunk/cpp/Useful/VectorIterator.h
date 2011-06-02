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
#ifndef __VECTORITERATOR_H__
#define __VECTORITERATOR_H__

#include "Iterator.h"

#include <vector>

namespace AWT
{
   template <class T>
   class VectorIterator : public Container::Iterator<T>
   {
   protected:
      VectorIterator( const std::vector<T>& vec )
      {
         this->iter    = vec.begin( );
         this->iterEnd = vec.end( );
      }

      virtual ~VectorIterator( )
      {
      }

   public:
      static VectorIterator<T>* getInstance( const std::vector<T>& vec )
      {
         return new VectorIterator<T>( vec );
      }

      virtual std::string getClassName( ) const
      {
         return "AWT::VectorIterator<T>";
      }

      virtual bool hasNext( )
      {
         return iter != iterEnd;
      }

      virtual T next( )
      {
         if ( !hasNext( ) )
            AWTEXCEPTIONTHROW("No such element");

         return *iter++;
      }

   protected:
      typedef typename std::vector<T>::const_iterator it_type;

      it_type iter;
      it_type iterEnd;
   };
}

#endif // __VECTORITERATOR_H__