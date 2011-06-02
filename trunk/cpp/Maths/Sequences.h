#ifndef __SEQUENCES_H__
#define __SEQUENCES_H__

namespace AWT
{
   template <class T>
   const T linspace( const T lower, const T upper, const T alpha )
   {
      return (1-alpha)*lower + alpha*upper;
   }

   template <class T>
   const T linspace( const T lower, const T upper, const unsigned int steps, const unsigned int i )
   {
      return linspace( lower, upper, static_cast<T>(i)/steps );
   }

   template <class T>
   const T logspace( const T lower, const T upper, const T alpha )
   {
      return exp( linspace( log(lower), log(upper), alpha ) );
   }

   template <class T>
   const T logspace( const T lower, const T upper, const unsigned int steps, const unsigned int i )
   {
      return logspace( lower, upper, static_cast<T>(i)/steps );
   }
}

#endif // __SEQUENCES_H__