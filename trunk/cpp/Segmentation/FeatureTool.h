#ifndef __FEATURETOOL_H__
#define __FEATURETOOL_H__

#include "DrawingTool.h"

#include <ostream>
#include <vector>

namespace AWT
{
   template <class T>
   class Point
   {
   public:
      Point( const T _x, const T _y )
         : x( _x ), y( _y )
      {}

      T x;
      T y;
   };

   template <class T, class ScanT, unsigned int V>
   class FeatureTool : public DrawingTool<T,ScanT,V>
   {
   public:
      FeatureTool( CommandManager* m );
      ~FeatureTool( );

      virtual char* getName( ) const;

      virtual void setCurrentImage( cimg_library::CImg<T>* im );

      virtual bool onMouseButton( const int x, const int y, const int z, const int button );
      virtual bool onMouseMove( const int x, const int y, const int z, const int button );

      virtual bool onActivate( );

      void findPoints( const T cc, const int mz, std::vector<Point<int>>& outPoints );

   protected:
      ScanT calculateGradientSquared( const int mx, const int my, const int mz );

      bool                     m_FirstTime;
      std::vector<Point<int>>  m_Points;
   };

}

#include "ConnectedRegions.h"

using namespace AWT;
using namespace cimg_library;

template <class T, class ScanT, unsigned int V>
AWT::FeatureTool<T,ScanT,V>::FeatureTool( AWT::CommandManager* m )
: AWT::DrawingTool<T,ScanT,V>::DrawingTool( m )
{
   //m_RegionImage = 0;
   m_FirstTime = true;
}

template <class T, class ScanT, unsigned int V>
AWT::FeatureTool<T,ScanT,V>::~FeatureTool( )
{
   //if ( m_RegionImage != 0 )
   //   delete m_RegionImage;
}

template <class T, class ScanT, unsigned int V>
char* AWT::FeatureTool<T,ScanT,V>::getName( ) const
{
   return "Feature Highlighter";
}

template <class T, class ScanT, unsigned int V>
void AWT::FeatureTool<T,ScanT,V>::setCurrentImage( CImg<T>* im )
{
   // Do the usual stuff
   AWT::DrawingTool<T,ScanT,V>::setCurrentImage( im );

   //if ( m_RegionImage != 0 )
   //{
   //   if ( m_RegionImage->dimx( ) != im->dimx( ) || m_RegionImage->dimy( ) != im->dimy( ) )
   //   {
   //      delete m_RegionImage;
   //      m_RegionImage = 0;      
   //   }
   //}

   //if ( m_RegionImage == 0 )
   //   m_RegionImage = new CImg<T>( im->dimx( ), im->dimy( ), 1, 1 );
   
   //m_CurrentRegion = 0;
   onActivate( );
}

template <class T, class ScanT, unsigned int V>
bool AWT::FeatureTool<T,ScanT,V>::onActivate( )
{
   AWT::DrawingTool<T,ScanT,V>::onActivate( );

   m_FirstTime = true;

   return true;
}

template <class T, class ScanT, unsigned int V>
bool AWT::FeatureTool<T,ScanT,V>::onMouseButton( const int mx, const int my, const int mz, const int button )
{
   return onMouseMove( mx, my, mz, button );
}

template <class T, class ScanT, unsigned int V>
ScanT AWT::FeatureTool<T,ScanT,V>::calculateGradientSquared( const int mx, const int my, const int mz )
{
   ScanT gradMag = 0;
   ScanT tmp;

   int coords[] = { mx, my, mz };
   int limits[] = { (*m_ScanImage).dimx( )-1, (*m_ScanImage).dimy( )-1, (*m_ScanImage).dimz( )-1 };

   int deltaPos[3];
   int deltaNeg[3];

   for ( int d = 0; d < 3; ++d )
   {
      for ( int dd = 0; dd < 3; ++dd )
      {
         deltaPos[dd] = ( d == dd && coords[d] < limits[d] ) ? 1 : 0;
         deltaNeg[dd] = ( d == dd && coords[d] > 0         ) ? -1 : 0;
      }

      tmp = (*m_ScanImage)( mx+deltaPos[0], my+deltaPos[1], mz+deltaPos[2] ) - (*m_ScanImage)( mx+deltaNeg[0], my+deltaNeg[1], mz+deltaNeg[2] );
      tmp /= ( deltaPos[d] - deltaNeg[d] );

      gradMag += tmp*tmp;
   }

   return gradMag;
}

template <class T, class ScanT, unsigned int V>
void AWT::FeatureTool<T,ScanT,V>::findPoints( const T cc, const int mz, std::vector<Point<int>>& out_Points )
{
   int minCoord, maxCoord;
   const int gradThresh = 32*32;

   // First of all, sweep across in the x-direction
   cimg_for_insideX( (*m_ScanImage), x, 1 )
   {
      minCoord = maxCoord = -1;

      cimg_for_insideY( (*m_ScanImage), y, 1 )
      {
         if ( (*m_CurrentImage)(x,y,mz) == cc && (*m_ScanImage)(x,y,mz) > 100 )
         {
            if ( calculateGradientSquared( x, y, mz ) > gradThresh )
            {
               if ( minCoord == -1 )
                  minCoord = maxCoord = y;
               else
                  maxCoord = y;
            }
         }
      }

      if ( minCoord != -1 )
      {
         out_Points.push_back( Point<int>( x, minCoord ) );
      }

      if ( maxCoord != minCoord )
      {
         out_Points.push_back( Point<int>( x, maxCoord ) );
      }
   }

   //Next, sweep across in the y-direction
   cimg_for_insideY( (*m_ScanImage), y, 1 )
   {
      maxCoord = minCoord = -1;

      cimg_for_insideX( (*m_ScanImage), x, 1 )
      {
         if ( (*m_CurrentImage)(x,y,mz) == cc && (*m_ScanImage)(x,y,mz) > 100 )
         {
            if ( calculateGradientSquared( x, y, mz ) > gradThresh )
            {
               if ( minCoord == -1 )
                  minCoord = maxCoord = x;
               else
                  maxCoord = x;
            }
         }
      }

      if ( minCoord != -1 )
      {
         out_Points.push_back( Point<int>( minCoord, y ) );
      }

      if ( maxCoord != minCoord )
      {
         out_Points.push_back( Point<int>( maxCoord, y ) );
      }
   }
}

template <class T, class ScanT, unsigned int V>
bool AWT::FeatureTool<T,ScanT,V>::onMouseMove( const int mx, const int my, const int mz, const int button )
{
   if ( m_ScanImage && m_CurrentImage && m_OverlayImage )
   {
      if ( button )
      {
         if ( m_FirstTime )
         {
           // m_RegionImage->fill( 0 );

            bool coloursUsed[255];
            for ( int i = 0; i < 255; ++i )
               coloursUsed[i] = false;

            cimg_forXY( (*m_CurrentImage), x, y )
               coloursUsed[ (*m_CurrentImage)(x, y, mz) ] = true;

            const int gradThresh = 32*32;

            int minCoord = -1, maxCoord = -1;

            m_Points.clear( );

            for ( int cc = 1; cc < 255; ++cc )
            {
               if ( !coloursUsed[cc] )
                  continue;

               findPoints( cc, mz, m_Points );
            }

            m_FirstTime = false;
         }

         std::vector<Point<int>>::iterator begin = m_Points.begin( );
         std::vector<Point<int>>::iterator end   = m_Points.end( );

         for ( ; begin != end; ++begin )
            (*m_OverlayImage)( (*begin).x, (*begin).y ) = 255;
      }
      else
      {
         //m_CurrentRegion = 0;
         m_OverlayImage->fill( 0 );
      }
      
      return true;
   }
   
   return false;
}

#endif // __FEATURETOOL_H__