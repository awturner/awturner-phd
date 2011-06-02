#include "SparseFieldSolver.h"

#include <map>
#include <iomanip>
#include "DoubleLinkedList.h"

// Do you want to use the full neighbourhood?
#define FULL_NEIGHBOURHOOD

#ifdef FULL_NEIGHBOURHOOD
   #define DEBUGSTOP 49
   #define DEBUGX 73
   #define DEBUGY 105
#else
   #define DEBUGSTOP 190
   #define DEBUGX 172
   #define DEBUGY 73
#endif


#define PRINTLISTSIZES { \
   DEBUGLINE; \
   PRINTVBL( lists[+2]->size( ) ); \
   PRINTVBL( lists[+1]->size( ) ); \
   PRINTVBL( lists[ 0]->size( ) ); \
   PRINTVBL( lists[-1]->size( ) ); \
   PRINTVBL( lists[-2]->size( ) ); \
   BLANKLINE; \
   PRINTVBL( moveLists[+3]->size( ) ); \
   PRINTVBL( moveLists[+2]->size( ) ); \
   PRINTVBL( moveLists[+1]->size( ) ); \
   PRINTVBL( moveLists[ 0]->size( ) ); \
   PRINTVBL( moveLists[-1]->size( ) ); \
   PRINTVBL( moveLists[-2]->size( ) ); \
   PRINTVBL( moveLists[-3]->size( ) ); \
   BLANKLINE; }

struct AWT::SparseFieldSolver::D
{
   typedef unsigned int Counter;

   typedef DLListNode<IndexType>          IndexNode;
   typedef DLList<IndexType>              IndexList;
   typedef std::map<LabelType,IndexList*> IndexListMap;

   typedef PhiType                        ForceType;
   typedef DLList<PhiType>                ForceList;
   typedef DLListNode<PhiType>            ForceNode;

   static void checkLabelInvariant( const LabelImage& label, const LevelSetImage& phi, const int line )
   {
      IndexType pos;
      IndexType qs[26];

      IndexType size;
      size.x = label.dimx();
      size.y = label.dimy();
      size.z = label.dimz();

      try
      {
         cimg_forXY( label, x, y )
         {
            pos.x = x; pos.y = y; pos.z = 0;

            Counter nqs = populateNeighbourhood<1>( size, pos, qs );
            for ( Counter i = 0; i < nqs; ++i )
            {
               if ( abs( label( pos.x, pos.y, pos.z ) - label( qs[i].x, qs[i].y, qs[i].z ) ) > 1 )
               {
                  PRINTVBL( 1*label( pos.x, pos.y, pos.z ) );
                  PRINTVBL( 1*label( qs[i].x, qs[i].y, qs[i].z ) );
                  throw std::exception( "Invariant failed!" );
               }
            }
         }
      }
      catch ( std::exception ex )
      {
         PRINTVBL( line );
         PRINTVBL( ex.what() );
         rainbowLabels( label, phi, __LINE__ );
         throw ex;
      }
   }

   static void rainbowLabels( const LabelImage& label, const LevelSetImage& phi, int line )
   {
      CImg<unsigned char> rainbow( label.dimx(), label.dimy(), 1, 3 );

      cimg_forXY( label, x, y )
      {
         if ( x == DEBUGX && y == DEBUGY )
         {
            rainbow(x,y,0,0) = 0; rainbow(x,y,0,1) = 255; rainbow(x,y,0,2) = 255;
         }
         else
         {
            switch ( label(x,y) )
            {
            case -3:
               rainbow(x,y,0,0) = 0; rainbow(x,y,0,1) = 0; rainbow(x,y,0,2) = 0;
               break;
            case -2:
               rainbow(x,y,0,0) = 255; rainbow(x,y,0,1) = 255; rainbow(x,y,0,2) = 0;
               break;
            case -1:
               rainbow(x,y,0,0) = 255; rainbow(x,y,0,1) =  0; rainbow(x,y,0,2) = 0;
               break;
            case 0:
               rainbow(x,y,0,0) = 255; rainbow(x,y,0,1) = 255; rainbow(x,y,0,2) = 255;
               break;
            case 1:
               rainbow(x,y,0,0) =   0; rainbow(x,y,0,1) = 0; rainbow(x,y,0,2) = 255;
               break;
            case 2:
               rainbow(x,y,0,0) =   0; rainbow(x,y,0,1) = 255; rainbow(x,y,0,2) = 0;
               break;
            case 3:
               rainbow(x,y,0,0) = 0; rainbow(x,y,0,1) = 0; rainbow(x,y,0,2) = 0;
               break;
            }
         }
      }

      char title[256];
      sprintf_s( title, "%d", line );

      CImgDisplay disp( rainbow, title );
      disp.resize( rainbow.dimx() * 3, rainbow.dimy() * 3 );

#define TYPED_DIV( T, x, y ) ( static_cast<T>( x ) / static_cast<T>( y ) )

      int mouseX = disp.mouse_x;
      int mouseY = disp.mouse_y;

      printAtPos( phi, DEBUGX, DEBUGY );
      printAtPos( label, DEBUGX, DEBUGY );

      while ( !disp.is_closed )
      {
         float iaspect = TYPED_DIV( float, rainbow.width,  rainbow.height );
         float waspect = TYPED_DIV( float, disp.window_dimx( ), disp.window_dimy( ) );

         // Keep the aspect ratio
         if ( iaspect > waspect )
         {
            disp.resize( disp.window_dimx( ), (int)( disp.window_dimx( ) / iaspect ) );
         }
         else if ( iaspect < waspect )
         {
            disp.resize( (int)( disp.window_dimy( ) * iaspect ), disp.window_dimy( ) );
         }
         else
         {
            disp.resize( );
         }

         if ( disp.button == 2 )
         {
            int fmouseX = static_cast<int>( ( disp.mouse_x - 0.5f ) / TYPED_DIV( float, (disp.width-1),  rainbow.width-1  ) + 0.5f );
            int fmouseY = static_cast<int>( ( disp.mouse_y - 0.5f ) / TYPED_DIV( float, (disp.height-1), rainbow.height-1 ) + 0.5f );

            if ( fmouseX != mouseX || fmouseY != mouseY )
            {
               mouseX = fmouseX;
               mouseY = fmouseY;

               DEBUGMACRO( fmouseX << "," << fmouseY << " -> " << 1*label( fmouseX,fmouseY )*1 << "\t" << phi( fmouseX, fmouseY ) );
            }
         }

         if ( disp.button == 1 )
         {
            mouseX = static_cast<int>( ( disp.mouse_x - 0.5f ) / TYPED_DIV( float, (disp.width-1),  rainbow.width-1  ) + 0.5f );
            mouseY = static_cast<int>( ( disp.mouse_y - 0.5f ) / TYPED_DIV( float, (disp.height-1), rainbow.height-1 ) + 0.5f );

            printAtPos( phi, mouseX, mouseY );
            printAtPos( label, mouseX, mouseY );

            mouseX = -1;
            mouseY = -1;

            disp.flush();
         }

         cimg::wait( 20 );
      }
   }

   template <class T>
   static void printAtPos( const CImg<T>& phi, const int x, const int y )
   {
      for ( int yy = y-2; yy <= y+2; ++yy )
      {
         for ( int xx = x-2; xx <= x+2; ++xx )
         {
            std::cerr << std::setw( 10 ) << (1*phi(xx,yy)) << " ";
         }
         std::cerr << std::endl;
      }
      std::cerr << std::endl;
   }

   // This expression generates the sequence of layer visiting
   // i    = [  0  1  2  3  4  5  6  7 ... ]
   // self = [  1 -1  2 -2  3 -3  4 -4 ... ]
   static LabelType layerOrder( const LabelType i )
   {
      return ( 1 + ( i >> 1 ) ) * ( (i%2) ? 1 : -1 );
   }

   // Returns the value whose absolute is greater
   template <class T>
   static T maxAbs( const T a, const T b )
   {
      return ( abs(a) > abs(b) ) ? a : b;
   }

   template <int W>
   static Counter populateNeighbourhood( const IndexType& size, const IndexType& pos, IndexType* neighs )
   {
      IndexType idx, mins, maxs;
      
      mins.x = std::max<Coord>( W, pos.x ) - W;
      mins.y = std::max<Coord>( W, pos.y ) - W;
      mins.z = std::max<Coord>( W, pos.z ) - W;

      maxs.x = std::min<Coord>( std::min<Coord>( pos.x, size.x-1-W ) + W, size.x-1 );
      maxs.y = std::min<Coord>( std::min<Coord>( pos.y, size.y-1-W ) + W, size.y-1 );
      maxs.z = std::min<Coord>( std::min<Coord>( pos.z, size.z-1-W ) + W, size.z-1 );

      Counter ret = 0;

#ifdef FULL_NEIGHBOURHOOD
      for ( idx.z = mins.z; idx.z <= maxs.z; ++idx.z )
      {
         for ( idx.y = mins.y; idx.y <= maxs.y; ++idx.y )
         {
            for ( idx.x = mins.x; idx.x <= maxs.x; ++idx.x )
            {
               // Don't reprocess self
               if ( idx.x == pos.x && idx.y == pos.y && idx.z == pos.z )
                  continue;

               neighs[ret++] = idx;
            }
         }
      }
#else
      // Add the + neighbours
      idx = pos; for ( idx.x = pos.x + 1; idx.x <= pos.x + W && idx.x <= maxs.x; ++idx.x ) neighs[ret++] = idx;
      idx = pos; for ( idx.y = pos.y + 1; idx.y <= pos.y + W && idx.y <= maxs.y; ++idx.y ) neighs[ret++] = idx;
      idx = pos; for ( idx.z = pos.z + 1; idx.z <= pos.z + W && idx.z <= maxs.z; ++idx.z ) neighs[ret++] = idx;

      // Add the -neighbours
      idx = pos; for ( idx.x = pos.x - 1; idx.x >= pos.x - W && idx.x >= mins.x; --idx.x ) neighs[ret++] = idx;
      idx = pos; for ( idx.y = pos.y - 1; idx.y >= pos.y - W && idx.y >= mins.y; --idx.y ) neighs[ret++] = idx;
      idx = pos; for ( idx.z = pos.z - 1; idx.z >= pos.z - W && idx.z >= mins.z; --idx.z ) neighs[ret++] = idx;
#endif

      return ret;
   }

   template <class ImType, int W>
   static Counter populateNeighbourhood( const CImg<ImType>& im, const IndexType& pos, IndexType* neighs )
   {
      IndexType size;
      size.x = im.dimx(); size.y = im.dimy(); size.z = im.dimz();
      return populateNeighbourhood<W>( size, pos, neighs );
   }

   static void initialize( const SegmentedImage& seg, const LabelType nlayers, LabelImage& label, LevelSetImage& phi, IndexListMap& layerLists )
   {
      // Assume that seg, label, phi are all the same size

      const LabelType farLayer = nlayers + 1;

      // Pre-condition label layerLists and phi
      cimg_forXYZ( phi, x, y, z )
      {
         if ( seg(x,y,z) == 0 )
         {
            label(x,y,z) =  farLayer;
            phi(x,y,z)   = static_cast<PhiType>(  farLayer );
         }
         else
         {
            label(x,y,z) = -farLayer;
            phi(x,y,z)   = static_cast<PhiType>( -farLayer );
         }
      }

      // Clear the layerLists and create some lists
      layerLists.clear( );
      for ( LabelType el = -nlayers; el <= nlayers; ++el )
         layerLists[el] = new IndexList( );

      IndexType idx;
   #ifdef FULL_NEIGHBOURHOOD
      IndexType qs[26];
   #else
      IndexType qs[6];
   #endif

      Counter cntSeg = 0;

      // Find the zero level set
      cimg_forXYZ( phi, x, y, z )
      {
         idx.x = x; idx.y = y; idx.z = z;

         if ( seg(idx.x,idx.y,idx.z) != 0 )
         {

            ++cntSeg;

            // Check if any points in the neighbourhood has seg = 0
            Counter nqs = populateNeighbourhood<PhiType,1>( phi, idx, qs );

            bool found = false;
            for ( Counter qi = 0; qi < nqs && !found; ++qi )
            {
               found = seg( qs[qi].x, qs[qi].y, qs[qi].z ) == 0;
            }

            if ( found )
            {
               layerLists[0]->insert( idx );
               label( x, y, z ) = 0;
               phi( x, y, z ) = 0;
            }
         }
      }

      for ( LabelType eli = 0; eli < 2*nlayers; ++eli )
      {
         const LabelType el        = layerOrder( eli );
         const LabelType innerSet  = ( el > 0 ) ? ( el - 1 ) : ( el + 1 );
         const PhiType   searchFor = static_cast<PhiType>( ( el > 0 ) ? farLayer : -farLayer );

         for ( IndexNode* it = layerLists[innerSet]->front; it != NULL; it = it->next )
         {
            Counter nqs = populateNeighbourhood<PhiType,1>( phi, it->value, qs );

            for ( Counter qi = 0; qi < nqs; ++qi )
            {
               idx = qs[qi];

               if ( label(idx.x,idx.y,idx.z) == searchFor )
               {
                  layerLists[el]->insert( idx );
                  label(idx.x,idx.y,idx.z) = el;
                  phi(idx.x,idx.y,idx.z)   = el;
               }
            }
         }
      }
   }

   static void calculateNearestZeroCrossing( const LevelSetImage& phi, const IndexType& pos, SubIndexType& crossing )
   {
      // Grad the crossing
      crossing.x = crossing.y = crossing.z = 0;

      const bool condsX[] = { 
         pos.x > 0            && abs( phi( pos.x-1, pos.y, pos.z ) ) <= 1.5,
         pos.x < phi.dimx()-1 && abs( phi( pos.x+1, pos.y, pos.z ) ) <= 1.5,
      };

      const bool condsY[] = { 
         pos.y > 0            && abs( phi( pos.x, pos.y-1, pos.z ) ) <= 1.5,
         pos.y < phi.dimy()-1 && abs( phi( pos.x, pos.y+1, pos.z ) ) <= 1.5,
      };

      const bool condsZ[] = { 
         pos.z > 0            && abs( phi( pos.x, pos.y, pos.z-1 ) ) <= 1.5,
         pos.z < phi.dimz()-1 && abs( phi( pos.x, pos.y, pos.z+1 ) ) <= 1.5,
      };

      PhiType maxAbsGrad = 0;

      // Calculate the x-gradient
      if ( condsX[0] && !condsX[1] )
      {
         crossing.x = phi( pos.x, pos.y, pos.z ) - phi( pos.x-1, pos.y, pos.z );
      }
      else if ( !condsX[0] && condsX[1] )
      {
         crossing.x = phi( pos.x+1, pos.y, pos.z ) - phi( pos.x, pos.y, pos.z );
      }
      else if ( condsX[0] && condsX[1] )
      {
         crossing.x = maxAbs( 
            phi( pos.x, pos.y, pos.z ) - phi( pos.x-1, pos.y, pos.z ),
            phi( pos.x+1, pos.y, pos.z ) - phi( pos.x, pos.y, pos.z )
            );
      }
      maxAbsGrad = maxAbs( maxAbsGrad, crossing.x );

      // Calculate the y-gradient
      if ( condsY[0] && !condsY[1] )
      {
         crossing.y = phi( pos.x, pos.y, pos.z ) - phi( pos.x, pos.y-1, pos.z );
      }
      else if ( !condsY[0] && condsY[1] )
      {
         crossing.y = phi( pos.x, pos.y+1, pos.z ) - phi( pos.x, pos.y, pos.z );
      }
      else if ( condsY[0] && condsY[1] )
      {
         crossing.y = maxAbs( 
            phi( pos.x, pos.y, pos.z ) - phi( pos.x, pos.y-1, pos.z ),
            phi( pos.x, pos.y+1, pos.z ) - phi( pos.x, pos.y, pos.z )
            );
      }
      maxAbsGrad = maxAbs( maxAbsGrad, crossing.y );

      // Calculate the y-gradient
      if ( condsZ[0] && !condsZ[1] )
      {
         crossing.z = phi( pos.x, pos.y, pos.z ) - phi( pos.x, pos.y, pos.z-1 );
      }
      else if ( !condsZ[0] && condsZ[1] )
      {
         crossing.z = phi( pos.x, pos.y, pos.z+1 ) - phi( pos.x, pos.y, pos.z );
      }
      else if ( condsZ[0] && condsZ[1] )
      {
         crossing.z = maxAbs( 
            phi( pos.x, pos.y, pos.z ) - phi( pos.x, pos.y, pos.z-1 ),
            phi( pos.x, pos.y, pos.z+1 ) - phi( pos.x, pos.y, pos.z )
            );
      }
      maxAbsGrad = maxAbs( maxAbsGrad, crossing.z );

      maxAbsGrad = abs( maxAbsGrad );

      PhiType zero_dot_zero = 0;

      if ( abs( crossing.x ) < maxAbsGrad ) crossing.x = 0;
      zero_dot_zero += crossing.x * crossing.x;

      if ( abs( crossing.y ) < maxAbsGrad ) crossing.y = 0;
      zero_dot_zero += crossing.y * crossing.y;

      if ( abs( crossing.z ) < maxAbsGrad ) crossing.z = 0;
      zero_dot_zero += crossing.z * crossing.z;

      if ( zero_dot_zero != 0 )
      {
         crossing.x /= zero_dot_zero;
         crossing.y /= zero_dot_zero;
         crossing.z /= zero_dot_zero;
      }

      // Now add on the position to this delta
      crossing.x += pos.x;
      crossing.y += pos.y;
      crossing.z += pos.z;
   }

   static void update2( LevelSetImage& phi, LabelType nlayers, LabelImage& label, IndexListMap& lists, const ForceList& force, const bool debuxg )
   {
      static int iters = 0;
      ++iters;
      PRINTVBL( iters );

      const bool debug = iters == DEBUGSTOP;

      // These are the membership lists
      IndexListMap L;
      L[ -nlayers-1 ] = new IndexList; // Add these outer layers for convenience
      L[ +nlayers+1 ] = new IndexList;
      for ( LabelType i = -nlayers; i <= nlayers; ++i )
         L[i] = lists[i];

      IndexNode* p;
      ForceNode* f;
      IndexType idx;
   #ifdef FULL_NEIGHBOURHOOD
      IndexType qs[26];
   #else
      IndexType qs[6];
   #endif

      if ( debug ) rainbowLabels( label, phi, __LINE__ );

      // Update phi for L0, without changing list membership
      p = L[ 0]->front;
      f = force.front;
      while ( p != NULL && f != NULL )
      {
         // Here, phi is actually updated
         phi( p->value.x, p->value.y, p->value.z ) += f->value;

         p = p->next;
         f = f->next;
      }

      // Use L[ 0] to update the values of phi currently in L[+1]
      // without changing list membership
      p = L[+1]->front;
      while ( p != NULL )
      {
         PhiType newValue = nlayers+1;

         Counter nqs = populateNeighbourhood<PhiType,1>( phi, p->value, qs );
         for ( Counter i = 0; i < nqs; ++i )
         {
            if ( label( qs[i].x, qs[i].y, qs[i].z ) == 0 )
               newValue = std::min( newValue, phi( qs[i].x, qs[i].y, qs[i].z ) );
         }

         phi( p->value.x, p->value.y, p->value.z ) = newValue + 1;

         p = p->next;
      }

      // Use L[ 0] to update the values of phi currently in L[-1]
      // without changing list membership
      p = L[-1]->front;
      while ( p != NULL )
      {
         PhiType newValue = -nlayers-1;

         Counter nqs = populateNeighbourhood<PhiType,1>( phi, p->value, qs );
         for ( Counter i = 0; i < nqs; ++i )
         {
            if ( label( qs[i].x, qs[i].y, qs[i].z ) == 0 )
               newValue = std::max( newValue, phi( qs[i].x, qs[i].y, qs[i].z ) );
         }

         phi( p->value.x, p->value.y, p->value.z ) = newValue - 1;

         p = p->next;
      }

      if ( debug ) rainbowLabels( label, phi, __LINE__ );

      // So far, no points have changed layer; simply their values have changed

      {
         LabelType is[] = { 0, +1, -1 };
         
         for( Counter ii = 0; ii < 3; ++ii )
         {
            if ( debug ) rainbowLabels( label, phi, __LINE__ );

            LabelType i = is[ii];

            // Check L[i] to see if any points need to swap lists
            p = L[i]->front;
            while ( p != NULL )
            {
               IndexNode* nextp = p->next;

               if ( label( p->value.x, p->value.y, p->value.z ) == i )
               {
                  if ( phi( p->value.x, p->value.y, p->value.z ) < i-0.5 )
                     swapLists( p, i-1, label, L );
                  else if ( phi( p->value.x, p->value.y, p->value.z ) >= i+0.5 )
                     swapLists( p, i+1, label, L );
               }
               else
               {
                  L[i]->remove( p );
                  delete p;
               }

               p = nextp;
            }
         }
      }

      {
         // Count the number of L[+1] points which are adjacent to L[-1] points
         Counter cnt = 0;
         p = L[+1]->front;
         while ( p != NULL )
         {
            IndexNode* nextp = p ->next;

            if ( label( p->value.x, p->value.y, p->value.z ) == 1 )
            {
               Counter nqs = populateNeighbourhood<PhiType,1>( phi, p->value, qs );
               for ( Counter i = 0; i < nqs; ++i )
               {
                  if ( label( qs[i].x, qs[i].y, qs[i].z ) == -1 )
                  {
                     PhiType pvalue = phi( p->value.x, p->value.y, p->value.z );
                     PhiType qvalue = phi( qs[i].x, qs[i].y, qs[i].z );

                     const PhiType absgrad = abs( pvalue - qvalue );

                     pvalue /= absgrad;
                     qvalue /= absgrad;

                     if ( pvalue < 0.5f )
                        swapLists( p, 0, label, L );
                     else if ( pvalue >= 1.5f )
                        swapLists( p, 2, label, L ); // Shouldn't actually do this, but here in case

                     if ( qvalue < -1.5f )
                        L[-2]->insert( qs[i] ); // Shouldn't actually do this, but here in case
                     else if ( qvalue >= -0.5f )
                        L[+0]->insert( qs[i] );

                     phi( p->value.x, p->value.y, p->value.z ) = pvalue;
                     phi( qs[i].x, qs[i].y, qs[i].z ) = qvalue;
                     /*
                     DEBUGMACRO( p->value.x << "," << p->value.y << "(" << (1*label(p->value.x,p->value.y,p->value.z)) << ") is next to " <<
                        qs[i].x << "," << qs[i].y << "(" << (1*label(qs[i].x,qs[i].y,qs[i].z)) << ")" );
                     */
                     ++cnt;

                     i = nqs;
                  }
               }
            }
            else
            {
               L[+1]->remove( p );
               delete( p );
            }

            p = nextp;
         }

         //if ( cnt != 0 )
         //   PAUSE;
      }

      // At this point, L[ 0] is now correct, and nothing more is added or removed on this update
      // It must be correct, because the layer can only ever move by 1 pixel at most, so what is now
      // in L[ 0] can only previously have been in L[+1] or L[-1], nowhere further out

      if ( debug ) rainbowLabels( label, phi, __LINE__ );

      // For each point in L[ 0], check to see if there are any far away members
      p = L[ 0]->front;

      while ( p != NULL )
      {
         IndexNode* nextp = p->next;

         if ( label( p->value.x, p->value.y, p->value.z ) == 0 )
         {
            Counter nqs = populateNeighbourhood<PhiType,1>( phi, p->value, qs );
            for ( Counter i = 0; i < nqs; ++i )
            {
               if ( label( qs[i].x, qs[i].y, qs[i].z ) < -1 )
               {
                  L[-1]->insert( qs[i] );
                  label( qs[i].x, qs[i].y, qs[i].z ) = -1;
               }
               else if ( label( qs[i].x, qs[i].y, qs[i].z ) > 1 )
               {
                  L[+1]->insert( qs[i] );
                  label( qs[i].x, qs[i].y, qs[i].z ) = +1;
               }
            }
         }
         else
         {
            // This has been moved into another list already
            L[ 0]->remove( p );
            delete p;
         }

         p = nextp;
      }

      if ( debug ) rainbowLabels( label, phi, __LINE__ );

      // Now go out through the layers updating points
      {
         LabelType is[] = { +1, -1, +2, -2 };

         for ( Counter ii = 0; ii < 4; ++ii )
         {
            const LabelType li = is[ii];

            const LabelType inner = li + ( ( li < 0 ) ?  1 : -1 );
            const LabelType outer = li + ( ( li < 0 ) ? -1 : +1 );

            p = L[li]->front;
            while ( p != NULL )
            {
               IndexNode* nextp = p->next;

               if ( label( p->value.x, p->value.y, p->value.z ) == li )
               {
                  // This label should be processed in this list - proceed

                  Counter cntInner = 0;
                  PhiType maxValue = -nlayers-1;
                  PhiType minValue = +nlayers+1;
                  Counter nqs = populateNeighbourhood<PhiType,1>( phi, p->value, qs );
                  for ( Counter i = 0; i < nqs; ++i )
                  {
                     if ( label( qs[i].x, qs[i].y, qs[i].z ) == inner )
                     {
                        ++cntInner;

                        maxValue = std::max<PhiType>( maxValue, phi( qs[i].x, qs[i].y, qs[i].z ) );
                        minValue = std::min<PhiType>( minValue, phi( qs[i].x, qs[i].y, qs[i].z ) );
                     }
                  }

                  if ( cntInner != 0 )
                  {
                     // There are adjacent inner points, so this stays in the list
                     phi( p->value.x, p->value.y, p->value.z ) = ( li < 0 ) ? ( maxValue - 1 ) : ( minValue + 1 );

                     // Bring outer neighbours in
                     for ( Counter i = 0; i < nqs; ++i )
                     {
                        if ( ( li > 0 && label( qs[i].x, qs[i].y, qs[i].z ) > outer ) 
                           || ( li < 0 && label( qs[i].x, qs[i].y, qs[i].z ) < outer ) )
                        {
                           L[outer]->insert( qs[i] );
                           label( qs[i].x, qs[i].y, qs[i].z ) = outer;
                        }
                     }
                  }
                  else
                  {
                     // No adjacent inner points; move outwards
                     swapLists( p, outer, label, L );
                  }
               }
               else
               {
                  // This has been moved into another list already
                  L[li]->remove( p );
                  delete p;
               }

               p = nextp;
            }
         }
      }

      if ( debug ) rainbowLabels( label, phi, __LINE__ );

      // Make sure that there are no straggly remnants in the lists
      {
         for ( LabelType li = -nlayers; li <= nlayers; ++li )
         {
            p = L[li]->front;
            while ( p != NULL )
            {
               IndexNode* nextp = p->next;

               if ( label( p->value.x, p->value.y, p->value.z ) != li )
               {
                  L[li]->remove( p );
                  delete p;
               }

               p = nextp;
            }
         }
      }

      // Tidy up the outer layers
      {
         LabelType is[] = { +nlayers+1, -nlayers-1 };

         for ( Counter ii = 0; ii < 2; ++ii )
         {
            const LabelType li = is[ii];

            p = L[li]->front;
            while ( p != NULL )
            {
               IndexNode* nextp = p->next;

               // Max(min) out the phi value
               phi( p->value.x, p->value.y, p->value.z ) = li;

               L[li]->remove( p );
               delete p;

               p = nextp;
            }

            L[li]->dispose( );
            delete L[li];
         }
      }

      checkLabelInvariant( label, phi, __LINE__ );
   }

   static void swapLists( IndexNode* p, const LabelType newLabel, LabelImage& labelImage, IndexListMap& lists )
   {
      const LabelType oldLabel = labelImage( p->value.x, p->value.y, p->value.z );

      labelImage( p->value.x, p->value.y, p->value.z ) = newLabel;
      IndexNode::switchLists( p, lists[oldLabel], lists[newLabel] );
   }

   static void update( LevelSetImage& phi, LabelType nlayers, LabelImage& label, IndexListMap& lists, const ForceList& force, const bool debug )
   {
      checkLabelInvariant( label, phi, __LINE__ );

      // Create the lists to keep track of movements
      // Create extra lists for convenience
      IndexListMap moveLists;
      for ( LabelType el = -nlayers-1; el <= nlayers+1; ++el )
         moveLists[ el ] = new IndexList;

      //if ( debug ) PRINTLISTSIZES;

      // Update the zero level set
      IndexNode* p = lists[0]->front;
      ForceNode* f = force.front;
      while ( p != NULL && f != NULL )
      {
         // Store this so that we can delete the current node if needed
         IndexNode* nextp = p->next;

         // Add F to phi at this point
         const PhiType newv = phi( p->value.x, p->value.y, p->value.z ) += f->value;

         if ( newv <= -0.5f )
         {
            // The point has moved out of this band, and so must be reassigned
            IndexNode::switchLists( p, lists[0], moveLists[-1] );
         }
         else if ( newv > 0.5f )
         {
            // The point has moved out of this band, and so must be reassigned
            IndexNode::switchLists( p, lists[0], moveLists[ 1] );
         }

         p = nextp;
         f = f->next;
      }

      checkLabelInvariant( label, phi, __LINE__ );
      //if ( debug ) PRINTLISTSIZES;

      // Just do a little bit of checking to make sure that the two lists are the same size
      if ( (f != NULL) || (p != NULL) )
         throw "Force and zero lists are different lengths!";

      IndexType idx;
   #ifdef FULL_NEIGHBOURHOOD
      IndexType qs[26];
   #else
      IndexType qs[6];
   #endif

      for ( LabelType si = 0; si < 2*nlayers; ++si )
      {
         // Update the self-th level set

         const LabelType self  = layerOrder( si );
         
         // This is the layer which you check to calculate the new value
         const LabelType inner = ( self > 0 ) ? (self-1) : (self+1);

         // This is the layer which this point gets demoted to unless it has the right neighbours
         const LabelType outer = ( self > 0 ) ? (self+1) : (self-1);

         const PhiType upperLimit = self + 0.5f;  // If value exceeds upperLimit, move to self+1 layer
         const PhiType lowerLimit = self - 0.5f;  // If value falls below lowerLimit, move to self-1 layer

         p = lists[self]->front;
         while ( p != NULL )
         {
            Counter cnt = 0;
            PhiType newValue;

            Counter nqs = populateNeighbourhood<PhiType,1>( phi, p->value, qs );

            if ( self > 0 )
            {
               // Pretty much the same as the self < 0 case (self != 0), but
               // essentially duplicated because of <= and std::min
               newValue = static_cast<PhiType>( nlayers + 2 );

               for ( Counter qi = 0; qi < nqs; ++qi )
               {
                  idx = qs[qi];

                  if ( label( idx.x, idx.y, idx.z ) <= inner )
                  {
                     if ( label( idx.x, idx.y, idx.z ) == inner )   
                        ++cnt;

                     newValue = std::min<PhiType>( newValue, phi( idx.x, idx.y, idx.z ) );
                  }
               }

               newValue += 1;
            }
            else
            {
               // Pretty much the same as the self > 0 case (self != 0), but
               // essentially duplicated because of >= and std::max
               newValue = -static_cast<PhiType>( nlayers + 2 );

               for ( Counter qi = 0; qi < nqs; ++qi )
               {
                  idx = qs[qi];

                  if ( label( idx.x, idx.y, idx.z ) >= inner )
                  {
                     if ( label( idx.x, idx.y, idx.z ) == inner )   
                        ++cnt;

                     newValue = std::max<PhiType>( newValue, phi( idx.x, idx.y, idx.z ) );
                  }
               }

               newValue -= 1;
            }

            // Store this for after the potential swappage
            IndexNode* nextp = p->next;

            if ( cnt == 0 )
            {
               // Swap into outer move list
               IndexNode::switchLists( p, lists[self], moveLists[outer] );
            }
            else
            {
               // Write the new value to the phi image
               phi( p->value.x, p->value.y, p->value.z ) = newValue;

               if ( newValue <= lowerLimit )
               {
                  // Put in the self-1 move list
                  IndexNode::switchLists( p, lists[self], moveLists[self-1] );
               }
               else if ( newValue > upperLimit )
               {
                  // Put in the self+1 move list
                  IndexNode::switchLists( p, lists[self], moveLists[self+1] );
               }
            }

            p = nextp;
         }
      }

      checkLabelInvariant( label, phi, __LINE__ );
      //if ( debug ) PRINTLISTSIZES;

      // Now deal with the points which changed status

      // Swap points into the zero level set
      while ( ( p = moveLists[0]->front ) != NULL )
      {
         label( p->value.x, p->value.y, p->value.z ) = 0;
         
         IndexNode* nextp = p->next;

         // Swap this node into the zero list
         IndexNode::switchLists( p, moveLists[0], lists[0] );

         if ( nlayers == 1 )
         {
            // This is necessary when there is only one layer either side because
            // otherwise the zero set may be left exposed
            Counter nqs = populateNeighbourhood<PhiType,1>( phi, p->value, qs );
            for ( Counter qi = 0; qi < nqs; ++qi )
            {
               idx = qs[qi];

               // Look for any neighbours which aren't in the +/- 1 sets
               if ( label( idx.x, idx.y, idx.z ) < -1 )
               {
                  label( idx.x, idx.y, idx.z ) = -1;
                  moveLists[-1]->insert( idx );
               }
               else if ( label( idx.x, idx.y, idx.z ) > +1 )
               {
                  label( idx.x, idx.y, idx.z ) = +1;
                  moveLists[+1]->insert( idx );
               }
            }
         }

         p = nextp;
      }

      // Remove isolated members of the zero set (i.e. contours which shrunk to a single pixel)
      /*
      p = lists[0]->front;
      while ( p != NULL )
      {
         IndexNode* nextp = p->next;

         Counter nqs = populateNeighbourhood<PhiType,1>( phi, p->value, qs );

         bool found = false;
         LabelType replace = 0;
         Counter nchecked = 0;

         for ( Counter qi = 0; qi < nqs && !found; ++qi )
         {
            Counter countSame = 0;

            if ( qs[qi].x == p->value.x ) ++countSame;
            if ( qs[qi].y == p->value.y ) ++countSame;
            if ( qs[qi].z == p->value.z ) ++countSame;

            if ( countSame == 2 )
            {
               ++nchecked;
               found = ( replace = label( qs[qi].x, qs[qi].y, qs[qi].z ) ) == 0;
            }
         }

         if ( nchecked && !found )
            IndexNode::switchLists( p, lists[0], lists[ replace ] );

         p = nextp;
      }
      */

      //checkLabelInvariant( label, phi, __LINE__ );
      if ( debug ) PRINTLISTSIZES;
      
      for ( LabelType si = 0; si < 2*nlayers; ++si )
      {
         // Update the self-th level set

         // This expression generates a sequence like this...
         // si   = [  0  1  2  3  4  5  6  7 ... ]
         // self = [  1 -1  2 -2  3 -3  4 -4 ... ]
         const LabelType self  = layerOrder( si );
         const LabelType outer = ( self > 0 ) ? (self+1) : (self-1);

         while ( ( p = moveLists[self]->front ) != NULL )
         {
            IndexNode* nextp = p->next;

            if ( label( p->value.x, p->value.y, p->value.z ) == self )
            {
               if ( abs( outer ) != nlayers+1 )
               {
                  Counter nqs = populateNeighbourhood<PhiType,1>( phi, p->value, qs );
                  for ( Counter qi = 0; qi < nqs; ++qi )
                  {
                     idx = qs[qi];

                     // Look for any neighbours which aren't in the outer layer
                     if ( abs( label( idx.x, idx.y, idx.z ) ) > nlayers )
                     {
                        label( idx.x, idx.y, idx.z ) = outer;
                        moveLists[outer]->insert( idx );
                     }
                  }
               }

               IndexNode::switchLists( p, moveLists[self], lists[self] );
               label( p->value.x, p->value.y, p->value.z ) = self;
            }
            else
            {
               moveLists[self]->remove( p );
               delete p;
            }
            
            p = nextp;
         }
      }

      checkLabelInvariant( label, phi, __LINE__ );
      if ( debug ) PRINTLISTSIZES;

      // Move points in the "convenience sets" to "far away"
      {
         // Delete the nodes as we go, they probably won't be used again
         // (They might, as a neighbour of another node... but rather than
         // checking, it's cheaper to delete and recreate).

         const LabelType farLayer = nlayers + 1;
         while ( ( p = moveLists[+farLayer]->front ) != NULL )
         {
            label( p->value.x, p->value.y, p->value.z ) = +farLayer;
            phi( p->value.x, p->value.y, p->value.z )   = static_cast<PhiType>( +farLayer );

            moveLists[+farLayer]->remove( p );
            delete p;
         }

         while ( ( p = moveLists[-farLayer]->front ) != NULL )
         {
            label( p->value.x, p->value.y, p->value.z ) = -farLayer;
            phi( p->value.x, p->value.y, p->value.z )   = static_cast<PhiType>( -farLayer );

            moveLists[-farLayer]->remove( p );
            delete p;
         }
      }

      checkLabelInvariant( label, phi, __LINE__ );
      if ( debug ) PRINTLISTSIZES;

      // Go through and delete the move lists
      for ( LabelType el = -nlayers-1; el <= nlayers+1; ++el )
         delete moveLists[ el ];

      checkLabelInvariant( label, phi, __LINE__ );
   }

   LabelType nlayers;
   
   struct Phase
   {
      LevelSetImage phi;
      LabelImage label;
      IndexListMap layerLists;
   };

   SegLabel nphases;
   Phase**  phases;

   ForceFunction* func;
};

AWT::SparseFieldSolver::SparseFieldSolver( ForceFunction* func, const SegmentedImage& initSeg )
{
   m_D = new D;

   func->setSolver( this );
   m_D->func = func;

   // Set the number of layers (depends on the form of the function)
   m_D->nlayers = func->getNumberOfLayers( );

   // This is the number of phases, i.e. regions in the segmentation
   m_D->nphases = initSeg.dimv( );
   m_D->phases  = new D::Phase*[ m_D->nphases ];

   SegmentedImage oneLabel( initSeg.dimx(), initSeg.dimy(), initSeg.dimz() );

   for ( SegLabel i = 0; i < m_D->nphases; ++i )
   {
      // Find the portion of the segmented image with this label
      cimg_forXYZ( oneLabel, x, y, z )
         oneLabel( x, y, z ) = ( initSeg(x,y,z,i) ) ? 1 : 0;

      D::Phase* thisPhase = m_D->phases[i] = new D::Phase;

      // Allocate the images to store the level set function and the layer membership labels
      thisPhase->phi   = LevelSetImage( initSeg.dimx(), initSeg.dimy(), initSeg.dimz() );
      thisPhase->label = LabelImage( initSeg.dimx(), initSeg.dimy(), initSeg.dimz() );

      // Populate the images and layer lists
      D::initialize( oneLabel, m_D->nlayers, thisPhase->label, thisPhase->phi, thisPhase->layerLists );
   }
}

AWT::SparseFieldSolver::~SparseFieldSolver( )
{
   // Deallocate all of the phases
   for ( D::Counter i = 0; i < m_D->nphases; ++i )
      delete m_D->phases[i];
   delete [] m_D->phases;

   delete m_D;
}

AWT::SparseFieldSolver::P AWT::SparseFieldSolver::getInstance( ForceFunction* func, const SegmentedImage& initSeg )
{
   AUTOGETINSTANCE( AWT::SparseFieldSolver, ( func, initSeg ) );
}

GETNAMEMACRO( AWT::SparseFieldSolver );

AWT::SparseFieldSolver::TimeType AWT::SparseFieldSolver::update( const bool debug )
{
   D::ForceList* forces = new D::ForceList[ m_D->nphases ];

   PhiType maxForce = 0;
   SubIndexType crossing;

   // Tell the compiler that this won't change
   const ForceFunction* func = m_D->func;

   // Firstly calculate the forces
   for ( SegLabel i = 0; i < m_D->nphases; ++i )
   {
      D::Phase* thisPhase = m_D->phases[i];

      if ( !thisPhase->layerLists[0]->empty() )
      {
         D::IndexNode* it = thisPhase->layerLists[0]->front;
         while ( it != NULL )
         {
            PhiType vv;
            if ( func->useSubPixelAccuracy() )
            {
               D::calculateNearestZeroCrossing( thisPhase->phi, it->value, crossing );
               vv = m_D->func->calculateForce( i, crossing );
            }
            else
            {
               vv = m_D->func->calculateForce( i, it->value );
            }

            forces[i].insert( vv );

            maxForce = std::max<PhiType>( maxForce, abs(vv) );
            
            it = it->next;
         }
      }
   }

   // Normalize the force so it isn't too big
   if ( maxForce != 0 )
   {
      const PhiType div = 0.05 / maxForce;

      for ( SegLabel i = 0; i < m_D->nphases; ++i )
      {
         for ( D::ForceNode* it = forces[i].front; it != NULL; it = it->next )
            it->value *= div;
      }

      for ( SegLabel i = 0; i < m_D->nphases; ++i )
      {
         D::update2( m_D->phases[i]->phi, m_D->nlayers, m_D->phases[i]->label, m_D->phases[i]->layerLists, forces[i], debug );
      }
   }


   // Clean up the force nodes to avoid memory leakage
   for ( SegLabel i = 0; i < m_D->nphases; ++i )
      forces[i].dispose( );

   delete [] forces;

   return maxForce;
}

const AWT::SparseFieldSolver::LevelSetImage& AWT::SparseFieldSolver::getLevelSetImage( const AWT::SparseFieldSolver::SegLabel label ) const
{
   return m_D->phases[ label ]->phi;
}