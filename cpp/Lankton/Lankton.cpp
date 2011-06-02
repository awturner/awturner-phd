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
#include "CImg.h"
#include "DoubleLinkedList.h"

#include "Useful/PrintMacros.h"
#include "Useful/ProfTimer.h"

#include <map>

using namespace cimg_library;
using namespace AWT;

// Let's define some types
typedef char LabelType;
typedef float PhiType;
typedef unsigned int Counter;

typedef CImg<unsigned char> SegImage;
typedef CImg<LabelType>     LabelImage;
typedef CImg<PhiType>       LevelSetImage;

typedef int Coord;
struct IndexType { Coord x; Coord y; Coord z; };
typedef DLListNode<IndexType>          IndexNode;
typedef DLList<IndexType>              IndexList;
typedef std::map<LabelType,IndexList*> IndexListMap;

typedef DLList<PhiType>                ForceList;
typedef DLListNode<PhiType>            ForceNode;

#include "CImgGradient.h"

#include "SparseFieldSolver.h"

namespace AWT
{
   class ForceFunctionTemplate : public SparseFieldSolver::ForceFunction
   {
   public:
      virtual SparseFieldSolver::TimeType getMaximumStep( ) const  { return maxstep; }
      virtual int getNumberOfLayers( ) const    { return nlayers; }
      virtual bool useSubPixelAccuracy( ) const { return subpixel; }

      virtual void setSolver( SparseFieldSolver* solv ) { this->solv = solv; }

   protected:
      ForceFunctionTemplate( const SparseFieldSolver::TimeType _maxstep, const int _nlayers, const bool _subpixel )
         : maxstep( _maxstep ), nlayers( _nlayers ), subpixel( _subpixel ) {}

      SparseFieldSolver* solv;

      const SparseFieldSolver::TimeType maxstep;
      const int nlayers;
      const bool subpixel;
   };

   class CurvatureForceFunction : public ForceFunctionTemplate
   {
   public:
      CurvatureForceFunction( ) : ForceFunctionTemplate( 1.f/6.f,2,false ) {}

      virtual SparseFieldSolver::PhiType  calculateForce( const SparseFieldSolver::SegLabel label, const SparseFieldSolver::IndexType& pos ) const
      {
         return meancurvature<PhiType>( solv->getLevelSetImage( label ), pos );
      }
   };

   class DilateForceFunction : public ForceFunctionTemplate
   {
   public:
      DilateForceFunction( ) : ForceFunctionTemplate( 1.f/2.f,2,false ) {}

      virtual SparseFieldSolver::PhiType  calculateForce( const SparseFieldSolver::SegLabel label, const SparseFieldSolver::IndexType& pos ) const
      {
         return 1;
      }
   };

   class ErodeForceFunction : public ForceFunctionTemplate
   {
   public:
      ErodeForceFunction( ) : ForceFunctionTemplate( 1.f/2.f,2,false ) {}

      virtual SparseFieldSolver::PhiType  calculateForce( const SparseFieldSolver::SegLabel label, const SparseFieldSolver::IndexType& pos ) const
      {
         return -1;
      }
   };

   template <class T>
   class ThresholdForceFunction : public ForceFunctionTemplate
   {
   public:
      ThresholdForceFunction( const CImg<T>& edges ) : ForceFunctionTemplate( 1.f, 2, false ), im( edges )
      {
      }

      virtual SparseFieldSolver::PhiType  calculateForce( const SparseFieldSolver::SegLabel label, const SparseFieldSolver::IndexType& pos ) const
      {
         return im.linear_pix3d( pos.x, pos.y, pos.z );
      }

      virtual SparseFieldSolver::PhiType  calculateForce( const SparseFieldSolver::SegLabel label, const SparseFieldSolver::SubIndexType& pos ) const
      {
         return im.linear_pix3d( pos.x, pos.y, pos.z );
      }

   protected:
      const CImg<T>& im;
   };
}

int main( int argc, char** argv )
{
   CImg<float> edges;
   edges = edges.channel( 0 );
   edges.load( argv[1] );

   const float minn = edges.min(), maxx = edges.max();
   edges = ( edges - minn ) / ( maxx - minn ) - 0.5f;
   SparseFieldSolver::ForceFunction* func = new ThresholdForceFunction<float>( edges );

   SegImage initSeg;
   initSeg.load( argv[2] );

   initSeg = initSeg.get_channel( 0 );

   cimg_forXYZ( initSeg, x, y, z )
      initSeg( x, y, z ) = initSeg( x, y, z ) ? 1 : 0;

   SparseFieldSolver::P sfs = SparseFieldSolver::getInstance( func, initSeg );

   const SparseFieldSolver::LevelSetImage& lsim = sfs->getLevelSetImage(0);
   CImgDisplay disp( sfs->getLevelSetImage(0), "label image", 1 );

   CImg<unsigned char> outImage( lsim.dimx(), lsim.dimy(), 1, 3 );
   
   ProfilingTimer lastUpdateTimer;
   lastUpdateTimer.start();
   double lastUpdateTime = 1000;

   unsigned int iter = 0;
   while ( !disp.is_closed )
   {
      ++iter;
      const bool debug = iter >= 9;

      if ( true || lastUpdateTime > 1.0/5.0 )
      {
         lastUpdateTime = 0;

         const SparseFieldSolver::LevelSetImage label = sfs->getLevelSetImage(0);

         const LabelType minn = -(func->getNumberOfLayers()+1);
         const LabelType maxx = -minn;

         if ( maxx != minn )
         {
            cimg_forXY( lsim, x, y )
            {
               if ( lsim(x,y) >= -0.5f && lsim( x, y ) <= 0.5f )
               {
                  outImage(x,y,0,0) = 255;
                  outImage(x,y,0,1) = outImage(x,y,0,2) = 0;
               }
               else
               {
                  unsigned char vv = 255 * ( lsim(x,y) - minn ) / ( maxx - minn );
                  cimg_forV( outImage, v )
                     outImage( x, y, 0, v ) = vv;
               }
            }
         }
         else
         {
            outImage.fill( 0 );
         }

         disp.display( outImage );

         //if ( debug )
         //   PAUSE;
      }
      else
      {
         lastUpdateTimer.stop();
         lastUpdateTime += lastUpdateTimer.getDurationInSecs();
         lastUpdateTimer.start();
      }

      try
      {
         sfs->update( debug );
      }
      catch ( std::exception ex )
      {
         DEBUGMACRO( ex.what() );
         DEBUGLINEANDEXIT( 1 );
      }
   }
}