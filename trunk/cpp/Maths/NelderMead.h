#ifndef __NELDERMEAD_H__
#define __NELDERMEAD_H__

#include "ObjectManagement/ManagedObject.h"

#include "vnl/vnl_matrix.h"
#include "OptimizationFunction.h"

namespace AWT
{
   template <class T>
   class NelderMead : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<NelderMead> P;

   protected:
      NelderMead( typename OptimizationFunction<T>::P func );
      virtual ~NelderMead( );

   public:
      static P getInstance( typename OptimizationFunction<T>::P func );
      virtual std::string getClassName( ) const;

      void setInitialGuess( const vnl_matrix<T>& vertex, const vnl_matrix<T>& stdev );

      T getRange( ) const;

      vnl_matrix<T> getCentroid( ) const;

      vnl_matrix<T> getSimplex( ) const;

      void initialize( );

      void step( );

      T getAlpha( ) const;
      void setAlpha( const T v );

      T getGamma( ) const;
      void setGamma( const T v );

      T getRho( ) const;
      void setRho( const T v );

      T getSigma( ) const;
      void setSigma( const T v );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __NELDERMEAD_H__