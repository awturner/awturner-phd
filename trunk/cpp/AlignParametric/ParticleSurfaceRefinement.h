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
#ifndef __ParticleSurfaceRefinement_H__
#define __ParticleSurfaceRefinement_H__

#include "RefinementStrategy.h"
#include "CoulombicParticleSurface.h"

namespace AWT
{
   namespace AlignParametric
   {
      class ParticleSurfaceRefinement : public RefinementStrategy
      {
      public:
         typedef ManagedAutoPointer<ParticleSurfaceRefinement> P;

      protected:
         ParticleSurfaceRefinement( );
         virtual ~ParticleSurfaceRefinement( );

      public:
         static P getInstance( );
         virtual std::string getClassName( ) const;

         const Idx getMaxParticles( ) const;
         void setMaxParticles( const Idx v );

         const Idx getSpreadBlockTime( ) const;
         void setSpreadBlockTime( const Idx v );

         const Idx getParticleMaturityAge( ) const;
         void setParticleMaturityAge( const Idx v );

         const T getMinSplitSeparation( ) const;
         void setMinSplitSeparation( const T v );

         virtual void addParticleSurface( ParticleSurface::P surf );

         virtual bool refine( AWT::Container::Iterator<Idx>::P iter );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __ParticleSurfaceRefinement_H__