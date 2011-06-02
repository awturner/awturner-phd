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