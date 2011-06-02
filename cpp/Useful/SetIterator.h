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
#ifndef __SETITERATOR_H__
#define __SETITERATOR_H__

#include "Iterator.h"
#include "Useful/Exception.h"

#include <set>

namespace AWT
{
   namespace Container
   {
      template <class T, class Cont >
      class STLIterator : public Iterator<T>
      {
      public:
         typedef ManagedAutoPointer<STLIterator<T,Cont>> P;

      protected:
         STLIterator( const Cont& s )
         {
            this->iter    = s.begin( );
            this->iterEnd = s.end( );
         }

         virtual ~STLIterator( )
         {
         }

      public:
         static P getInstance( const Cont& s )
         {
            typedef AWT::Container::STLIterator<T,Cont> STLIterator;
            AUTOGETINSTANCE( STLIterator, ( s ) );
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

         virtual std::string getClassName( ) const
         {
            return "AWT::Container::STLIterator<T,Cont>";
         }

      protected:
         typedef typename Cont::const_iterator it_type;

         it_type iter;
         it_type iterEnd;
      };

      template <class T>
      class SetIterator : public Iterator<T>
      {
      public:
         typedef ManagedAutoPointer<SetIterator<T>> P;

      protected:
         SetIterator( const std::set<T>& s )
         {
            this->iter    = s.begin( );
            this->iterEnd = s.end( );
         }

         virtual ~SetIterator( )
         {
         }

      public:
         static P getInstance( const std::set<T>& s )
         {
            AUTOGETINSTANCE( AWT::Container::SetIterator<T>, ( s ) );
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

         virtual std::string getClassName( ) const
         {
            return "AWT::Container::SetIterator<T>";
         }

      protected:
         typedef typename std::set<T>::const_iterator it_type;

         it_type iter;
         it_type iterEnd;
      };
   }
}

#endif // __SETITERATOR_H__