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
#include "ParticleSurfaceRefinement.h"

#include "Useful/ColouredConsole.h"
#include "Useful/Noise.h"

#include <queue>

using namespace AWT;
using namespace AWT::AlignParametric;

struct AWT::AlignParametric::ParticleSurfaceRefinement::D
{
   void checkSameNumber( )
   {
      const Idx N = getN( );

      if ( N >= maxParticles )
         return;

      // Make sure that all surfaces have the same number of samples
      for ( Idx s = 1; s < surfaces.size(); ++s )
         if ( surfaces[s]->getNumberOfSamples( ) != N )
            throw "Not all surfaces have the same number of samples (before)";
   }

   const Idx getN( ) const
   {
      return surfaces[0]->getNumberOfSamples( );
   }

   std::vector<Idx> convergedCount;
   std::vector<ParticleSurface::P> surfaces;

   std::vector< MatrixType > previousPositions;

   Idx maxParticles;
   Idx spreadBlockTime;
   Idx particleMaturityAge;

   T   minSplitSeparation;

   Idx lastSplit;
   Idx iter;
};

AWT::AlignParametric::ParticleSurfaceRefinement::ParticleSurfaceRefinement( )
{
   m_D = new D;

   m_D->maxParticles = 0;
   m_D->spreadBlockTime = 0;
   m_D->particleMaturityAge = 10;

   m_D->minSplitSeparation  = 0;

   m_D->lastSplit = m_D->iter = 0;
}

AWT::AlignParametric::ParticleSurfaceRefinement::~ParticleSurfaceRefinement( )
{
   delete m_D;
}

AWT::AlignParametric::ParticleSurfaceRefinement::P AWT::AlignParametric::ParticleSurfaceRefinement::getInstance( )
{
   AUTOGETINSTANCE( AWT::AlignParametric::ParticleSurfaceRefinement, ( ) );
}

GETNAMEMACRO( AWT::AlignParametric::ParticleSurfaceRefinement );

void AWT::AlignParametric::ParticleSurfaceRefinement::addParticleSurface( ParticleSurface::P surf )
{
   m_D->surfaces.push_back( surf );
}

bool AWT::AlignParametric::ParticleSurfaceRefinement::refine( AWT::Container::Iterator<Idx>::P iter )
{
   // Give particles time to spread out
   if ( ++m_D->iter < m_D->lastSplit + m_D->spreadBlockTime || m_D->surfaces[0]->getNumberOfSamples() >= m_D->maxParticles )
      return false;

   const Idx N = m_D->getN( );

   // Split ALL particles

   bool didSplit = false;

   typedef std::vector<Idx> IdxList;
   typedef std::vector<ParticleSurface::P> SurfaceList;
   for ( Idx i = 0; i < N && m_D->surfaces[0]->getNumberOfSamples() < m_D->maxParticles; ++i )
   {
      // Define a consistent split direction
      T theShift[3];
      FOREACHAXIS( ax )
         theShift[ax] = Noise<T>::randu( -1, 1 ) * 1e-2;

      for ( SurfaceList::iterator sit = m_D->surfaces.begin(); sit != m_D->surfaces.end(); ++sit )
      {
         (*sit)->splitParticle( i, theShift );
      }

      didSplit = true;
   }

   if ( didSplit )
      m_D->lastSplit = m_D->iter;

   PRINTBOOL( didSplit );
   return didSplit;
}
#if 0
   {
      ColouredConsole cons( 12 );
      DEBUGLINE;
   }
   if ( !iter->hasNext() )
      return false;

   m_D->checkSameNumber( );

   const Idx N = m_D->getN( );

   if ( N >= m_D->maxParticles )
      return false;

   {
      ColouredConsole cons( 12 );
      DEBUGLINE;
   }
   // Make sure that there is a counter for all particles
   while ( m_D->convergedCount.size() > N )
      m_D->convergedCount.pop_back();

   while ( m_D->convergedCount.size() < N )
      m_D->convergedCount.push_back( 0 );

   // Give particles time to spread out
   if ( ++m_D->iter < m_D->lastSplit + m_D->spreadBlockTime )
      return false;

   // Split ALL particles

   bool didSplit = false;
   {
      typedef std::vector<Idx> IdxList;
      typedef std::vector<ParticleSurface::P> SurfaceList;
      for ( Idx i = 0; i < N && m_D->surfaces[0]->getNumberOfSamples() < m_D->maxParticles; ++i )
      {
         // Define a consistent split direction
         T theShift[3];
         FOREACHAXIS( ax )
            theShift[ax] = Noise<T>::randu( -1, 1 ) * 1e-2;

         for ( SurfaceList::iterator sit = m_D->surfaces.begin(); sit != m_D->surfaces.end(); ++sit )
         {
            (*sit)->splitParticle( i, theShift );
         }

         didSplit = true;
         // Reset the convergence counter
         m_D->convergedCount[i] = 0;
      }
   }

   if ( didSplit )
      m_D->lastSplit = m_D->iter;

   return didSplit;

   {
      ColouredConsole cons( 12 );
      DEBUGLINE;
   }

   m_D->lastSplit = m_D->iter;

   std::vector<Idx> particlesToSplit;

   typedef std::pair<T,Idx> QueuePair;
   typedef std::priority_queue<QueuePair> Queue;

   Queue queue;

   typedef std::vector<Idx> IdxList;
   typedef std::vector<ParticleSurface::P> SurfaceList;

   while ( iter->hasNext() )
   {
      const Idx p = iter->next();

      bool shouldSplit = false;

      if ( true || ++m_D->convergedCount[p] >= m_D->particleMaturityAge )
      {
         particlesToSplit.push_back( p );
         shouldSplit = true;
      }

      /*
      PRINTVBL( m_D->convergedCount[p] );
      PRINTVBL( m_D->minSplitSeparation );
      PRINTVBL( m_D->particleMaturityAge );
      PRINTBOOL( shouldSplit );
      */

      T sumMin = 0;
      for ( SurfaceList::iterator sit = m_D->surfaces.begin(); shouldSplit && sit != m_D->surfaces.end(); ++sit )
      {
         const T thisMin = (*sit)->getMinDistance( p );

         shouldSplit = thisMin > m_D->minSplitSeparation;

         sumMin += thisMin*thisMin;
      }

      if ( shouldSplit )
      {
         ColouredConsole cons( ColouredConsole::COL_GREEN | ColouredConsole::COL_BRIGHT );
         PRINTVBL( p );

         queue.push( QueuePair( sumMin, p ) );
      }
   }

   {
      ColouredConsole cons( 12 );
      PRINTVBL( particlesToSplit.size() );
   }

   if ( particlesToSplit.empty() )
      return false;

   while ( !queue.empty() && m_D->surfaces[0]->getNumberOfSamples() < m_D->maxParticles )
   {
      const QueuePair qp = queue.top();
      queue.pop();

      // Define a consistent split direction
      T theShift[3];
      FOREACHAXIS( ax )
         theShift[ax] = Noise<T>::randu( -1, 1 ) * 1e-2;

      for ( SurfaceList::iterator sit = m_D->surfaces.begin(); sit != m_D->surfaces.end(); ++sit )
      {
         (*sit)->splitParticle( qp.second, theShift );
      }

      // Reset the convergence counter
      m_D->convergedCount[qp.second] = 0;
   }

   DEBUGLINE;

   const Idx NN = m_D->surfaces[0]->getNumberOfSamples( );

   // Make sure that all surfaces have the same number of samples
   for ( Idx s = 1; s < m_D->surfaces.size(); ++s )
      if ( m_D->surfaces[s]->getNumberOfSamples( ) != NN )
         throw "Not all surfaces have the same number of samples (after)";

   PRINTVBL(NN);
   return true;
}
#endif 

const Idx AWT::AlignParametric::ParticleSurfaceRefinement::getMaxParticles( ) const
{
   return m_D->maxParticles;
}

void AWT::AlignParametric::ParticleSurfaceRefinement::setMaxParticles( const Idx v )
{
   if ( v != m_D->maxParticles )
   {
      m_D->maxParticles = v;
      modified();
   }
}

const Idx AWT::AlignParametric::ParticleSurfaceRefinement::getSpreadBlockTime( ) const
{
   return m_D->spreadBlockTime;
}

void AWT::AlignParametric::ParticleSurfaceRefinement::setSpreadBlockTime( const Idx v )
{
   if ( v != m_D->spreadBlockTime )
   {
      m_D->spreadBlockTime = v;
      modified();
   }
}

const Idx AWT::AlignParametric::ParticleSurfaceRefinement::getParticleMaturityAge( ) const
{
   return m_D->particleMaturityAge;
}

void AWT::AlignParametric::ParticleSurfaceRefinement::setParticleMaturityAge( const Idx v )
{
   if ( v != m_D->particleMaturityAge )
   {
      m_D->particleMaturityAge = v;
      modified();
   }
}

const T AWT::AlignParametric::ParticleSurfaceRefinement::getMinSplitSeparation( ) const
{
   return m_D->minSplitSeparation;
}

void AWT::AlignParametric::ParticleSurfaceRefinement::setMinSplitSeparation( const T v )
{
   if ( v != m_D->minSplitSeparation )
   {
      m_D->minSplitSeparation = v;
      modified();
   }
}