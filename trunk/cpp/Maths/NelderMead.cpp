#include "NelderMead.h"

#include "Useful/Noise.h"

#include <limits>

template <class T>
struct AWT::NelderMead<T>::D
{
   D( typename OptimizationFunction<T>::P func )
      : m_Dims( func->getNumberOfVariables( ) ), m_Population( func->getNumberOfVariables( ), func->getNumberOfVariables( )+1 ), 
      m_Values( 1, func->getNumberOfVariables( )+1 ),
      alpha( 1 ), gamma( 2 ), rho( 0.5 ), sigma( 0.5 )
   {
      m_Func = func;

      m_Population.fill( std::numeric_limits<T>::quiet_NaN() );
      m_Values.fill( std::numeric_limits<T>::quiet_NaN() );
   }

   ~D( )
   {
   }

   void getMaxMinIndices( unsigned int& maxx, unsigned int& minn )
   {
      maxx = minn = 0;

      T maxv = m_Values[0][maxx];
      T minv = m_Values[0][minn];

      for ( unsigned int m = 1; m <= m_Dims; ++m )
      {
         if ( m_Values[0][m] > maxv )
         {
            maxv = m_Values[0][m];
            maxx = m;
         }

         if ( m_Values[0][m] < minv )
         {
            minv = m_Values[0][m];
            minn = m;
         }
      }
   }

   typename OptimizationFunction<T>::P m_Func;
   const unsigned int m_Dims;

   vnl_matrix<T> m_Population;
   vnl_matrix<T> m_Values;

   T alpha;
   T gamma;
   T rho;
   T sigma;
};

template <class T>
AWT::NelderMead<T>::NelderMead( typename AWT::OptimizationFunction<T>::P func )
{
   m_D = new D( func );
}

template <class T>
AWT::NelderMead<T>::~NelderMead( )
{
   delete m_D;
}

template <class T>
typename AWT::NelderMead<T>::P AWT::NelderMead<T>::getInstance( typename AWT::OptimizationFunction<T>::P func )
{
   AUTOGETINSTANCE( AWT::NelderMead<T>, ( func ) );
}

template <class T>
GETNAMEMACRO( AWT::NelderMead<T> );

template <class T>
void AWT::NelderMead<T>::setAlpha( const T v )
{
   m_D->alpha = v;
}

template <class T>
T AWT::NelderMead<T>::getAlpha( ) const
{
   return m_D->alpha;
}

template <class T>
void AWT::NelderMead<T>::setGamma( const T v )
{
   m_D->gamma = v;
}

template <class T>
T AWT::NelderMead<T>::getGamma( ) const
{
   return m_D->gamma;
}

template <class T>
void AWT::NelderMead<T>::setRho( const T v )
{
   m_D->rho = v;
}

template <class T>
T AWT::NelderMead<T>::getRho( ) const
{
   return m_D->rho;
}

template <class T>
void AWT::NelderMead<T>::setSigma( const T v )
{
   m_D->sigma = v;
}

template <class T>
T AWT::NelderMead<T>::getSigma( ) const
{
   return m_D->sigma;
}

template <class T>
void AWT::NelderMead<T>::setInitialGuess( const vnl_matrix<T>& vertex, const vnl_matrix<T>& stdev )
{
   // Create a random simplex around the mean with the given standard deviation for each point
   for ( unsigned int m = 0; m < m_D->m_Dims; ++m )
      for ( unsigned int n = 0; n <= m_D->m_Dims; ++n )
      {
         m_D->m_Population[m][n] = Noise<T>::randn( vertex[m][0], stdev[m][0] );
      }
}

template <class T>
T AWT::NelderMead<T>::getRange( ) const
{
   unsigned int maxi, mini;
   m_D->getMaxMinIndices( maxi, mini );

   return m_D->m_Values[0][maxi] - m_D->m_Values[0][mini];
}

template <class T>
vnl_matrix<T> AWT::NelderMead<T>::getCentroid( ) const
{
   vnl_matrix<T> ret( m_D->m_Dims, 1 );
   ret.fill( 0 );

   for ( unsigned int m = 0; m < m_D->m_Dims; ++m )
      for ( unsigned int n = 0; n <= m_D->m_Dims; ++n )
         ret[m][0] += m_D->m_Population[m][n];

   for ( unsigned int m = 0; m < m_D->m_Dims; ++m )
      ret[m][0] /= m_D->m_Dims + 1;

   return ret;
}

template <class T>
vnl_matrix<T> AWT::NelderMead<T>::getSimplex( ) const
{
   return vnl_matrix<T>( m_D->m_Population );
}


template <class T>
void AWT::NelderMead<T>::initialize( )
{
   const unsigned int rs = m_D->m_Func->getNumberOfVariables( );

   vnl_matrix<T> mat( m_D->m_Dims, 1 );
   for ( unsigned int n = 0; n <= m_D->m_Dims; ++n )
   {
      mat = m_D->m_Population.extract( rs, 1, 0, n );
      m_D->m_Values( 0, n ) = m_D->m_Func->evaluate( mat );
   }
}

template <class T>
void AWT::NelderMead<T>::step( )
{
   unsigned int worstIdx, bestIdx;
   m_D->getMaxMinIndices( worstIdx, bestIdx );

   unsigned int secondWorst;
   {
      T secondMax = -std::numeric_limits<T>::infinity( );

      for ( unsigned int n = 0; n <= m_D->m_Dims; ++n )
      {
         if ( n == worstIdx || n == bestIdx )
            continue;

         if ( m_D->m_Values[0][n] > secondMax )
         {
            secondMax = m_D->m_Values[0][n];
            secondWorst = n;
         }
      }
   }

   // Calculate the mean excluding the worst point
   vnl_matrix<T> excMean = getCentroid() * static_cast<T>( m_D->m_Dims + 1 );
   for ( unsigned int m = 0; m < m_D->m_Dims; ++m )
      excMean[m][0] -= m_D->m_Population[m][worstIdx];
   excMean /= static_cast<T>( m_D->m_Dims );

   const unsigned int rs = m_D->m_Func->getNumberOfVariables( );

   vnl_matrix<T> worstPoint = m_D->m_Population.extract( rs, 1, 0, worstIdx );

   // Compute the reflection through the worst-exclusive mean
   vnl_matrix<T> xr = excMean + m_D->alpha * ( excMean - worstPoint );
   T fxr = m_D->m_Func->evaluate( xr );

   if ( fxr < m_D->m_Values[0][bestIdx] )
   {
      // If the reflected point is the current best, then perhaps see
      // if we can get better by going even further
      vnl_matrix<T> xe = excMean + m_D->gamma * ( excMean - worstPoint );
      T fxe = m_D->m_Func->evaluate( xe );

      if ( fxe < fxr )
      {
         m_D->m_Population.set_columns( worstIdx, xe );
         m_D->m_Values( 0, worstIdx ) = fxe;
      }
      else
      {
         m_D->m_Population.set_columns( worstIdx, xr );
         m_D->m_Values( 0, worstIdx ) = fxr;
      }
   }
   else if ( fxr < m_D->m_Values[0][secondWorst] )
   {
      // The reflected point's value lies in between the best and the second worst
      // so let's use this again next time.
      m_D->m_Population.set_columns( worstIdx, xr );
      m_D->m_Values( 0, worstIdx ) = fxr;
   }
   else
   {
      vnl_matrix<T> xc = worstPoint + m_D->rho * ( excMean - worstPoint );
      T fxc = m_D->m_Func->evaluate( xc );

      if ( fxc < m_D->m_Values[0][worstIdx] )
      {
         m_D->m_Population.set_columns( worstIdx, xc );
         m_D->m_Values( 0, worstIdx ) = fxc;
      }
      else
      {
         for ( unsigned int m = 0; m < m_D->m_Dims; ++m )
            for ( unsigned int n = 0; n <= m_D->m_Dims; ++n )
            {
               m_D->m_Population[m][n] = m_D->m_Population[m][bestIdx] + m_D->sigma * ( m_D->m_Population[m][n] - m_D->m_Population[m][bestIdx] );
            }

         // All the points have moved, so reevaluate them all
         initialize( );
      }
   }
}

template class AWT::NelderMead<double>;
template class AWT::NelderMead<float>;