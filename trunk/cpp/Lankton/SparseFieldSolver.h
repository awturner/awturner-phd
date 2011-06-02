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
#ifndef __SPARSEFIELDSOLVER_H__
#define __SPARSEFIELDSOLVER_H__

#include "ObjectManagement/ManagedObject.h"

#include "CImg.h"
#include "DoubleLinkedList.h"

using namespace cimg_library;

namespace AWT
{
   class SparseFieldSolver : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<SparseFieldSolver> P;

      typedef float TimeType;

      typedef float PhiType;
      typedef CImg<PhiType> LevelSetImage;

      typedef char LabelType;
      typedef CImg<LabelType> LabelImage;

      // Used to define the, erm, initial segmentation

      typedef unsigned char SegLabel;
      typedef CImg<SegLabel> SegmentedImage;

      // Used to define positions within the image
      // It's signed purely to make some of the checking a bit easier...
      // images are never going to be > 2^15-1 = 32767 in side, so
      // things are peachy
      typedef short Coord;
      struct IndexType { 
         Coord x; 
         Coord y; 
         Coord z; 
         
         IndexType( ) { }
         IndexType( Coord _x, Coord _y, Coord _z )
            : x( _x ), y( _y ), z( _z )
         {
         }
      };

      typedef float SubCoord;
      struct SubIndexType { 
         SubCoord x; SubCoord y; SubCoord z; 

         static IndexType round( const SubIndexType& pos )
         {
            IndexType rpos;
            rpos.x = static_cast<Coord>( floor( static_cast<SubCoord>(1)/2 + pos.x ) );
            rpos.y = static_cast<Coord>( floor( static_cast<SubCoord>(1)/2 + pos.y ) );
            rpos.z = static_cast<Coord>( floor( static_cast<SubCoord>(1)/2 + pos.z ) );
            return rpos;
         }
      };

      // This is a function which will calculate the update force
      class ForceFunction
      {
      public:
         virtual void setSolver( SparseFieldSolver* solv ) = 0;

         virtual TimeType getMaximumStep( ) const = 0;
         virtual int      getNumberOfLayers( ) const = 0;
         virtual bool     useSubPixelAccuracy( ) const = 0;

         virtual PhiType  calculateForce( const SegLabel label, const SubIndexType& pos ) const { return 0; }
         virtual PhiType  calculateForce( const SegLabel label, const IndexType& pos ) const { return 0; }
      };

   protected:
      SparseFieldSolver( ForceFunction* func, const SegmentedImage& initSeg );
      virtual ~SparseFieldSolver( );

   public:
      static P getInstance( ForceFunction* func, const SegmentedImage& initSeg );

      virtual std::string getClassName( ) const;

      virtual TimeType update( const bool debug = false );

      virtual const LevelSetImage& getLevelSetImage( const SegLabel label ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __SPARSEFIELDSOLVER_H__