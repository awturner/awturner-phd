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