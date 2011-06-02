#ifndef __WEIGHTFUNCTION_H__
#define __WEIGHTFUNCTION_H__

#include <cmath>

namespace AWT
{
   template <class T>
   class WeightFunction
   {
   public:
      virtual T calculate( T lambda ) = 0;
      virtual void finishedWith( )
      {
         if ( --usageCount == 0 )
            delete this;
      }

   protected:
      WeightFunction( ) { usageCount = 0; }

      int usageCount;
   };

   template <class T>
   class SingletonWeightFunction : public WeightFunction<T>
   {
   public:
      virtual void finishedWith( )
      {
         --usageCount;
      }
   protected:
      SingletonWeightFunction( ) {}
      virtual ~SingletonWeightFunction( ) {}
   };

   template <class T>
   class UniformWeightFunction : public SingletonWeightFunction<T>
   {
   public:
      virtual T calculate( T lambda ) { return 1; }

      static UniformWeightFunction* getInstance( )
      {
         if ( instance == 0 )
            instance = new UniformWeightFunction( );

         return instance;
      }

   protected:
      static UniformWeightFunction* instance;
   };

   template <class T>
   UniformWeightFunction<T>* UniformWeightFunction<T>::instance = 0;

   template <class T>
   class ForwardOnlyWeightFunction : public SingletonWeightFunction<T>
   {
   public:
      virtual T calculate( T lambda ) { return ( lambda >= 0 ) ? 1 : 0; }

      static ForwardOnlyWeightFunction* getInstance( )
      {
         if ( instance == 0 )
            instance = new ForwardOnlyWeightFunction( );

         return instance;
      }

   protected:
      static ForwardOnlyWeightFunction* instance;
   };

   template <class T>
   ForwardOnlyWeightFunction<T>* ForwardOnlyWeightFunction<T>::instance = 0;

   template <class T>
   class NormalWeightFunction : public WeightFunction<T>
   {
   public:
      NormalWeightFunction( const T sigma ) : sigma2( sigma*sigma ) {}

      virtual T calculate( T lambda ) { return exp( -lambda*lambda/sigma2 ); }

   protected:
      const T sigma2;
   };

   template <class T>
   class AnisotropicNormalWeightFunction : public WeightFunction<T>
   {
   public:
      AnisotropicNormalWeightFunction( const T sigmaF, const T sigmaB ) : sigmaF2( sigmaF*sigmaF ), sigmaB2( sigmaB*sigmaB ) {}

      virtual T calculate( T lambda ) {
         if ( lambda > 0 )
            return exp( -lambda*lambda/sigmaF2 );
         else
            return exp( -lambda*lambda/sigmaB2 );
      }

   protected:
      const T sigmaF2;
      const T sigmaB2;
   };

   template <class T>
   class AnisotropicTriangleWeightFunction : public WeightFunction<T>
   {
   public:
      AnisotropicTriangleWeightFunction( const T _sigmaF, const T _sigmaB ) : sigmaF( _sigmaF ), sigmaB( _sigmaB ) {}

      virtual T calculate( T lambda ) {
         T ret;

         if ( lambda > 0 )
            if ( lambda > sigmaF )
               return 0;
            else
               return 1 - lambda / sigmaF;
         else
            if ( -lambda > sigmaB )
               return 0;
            else
               return 1 + lambda / sigmaB;
      }

   protected:
      const T sigmaF;
      const T sigmaB;
   };
}

#endif // __WEIGHTFUNCTION_H__