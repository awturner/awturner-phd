#ifndef __QUADTREEIMAGE_H__
#define __QUADTREEIMAGE_H__

#include "ObjectManagement/ManagedObject.h"
#include "Useful/Exception.h"

#include "ImageVisitor.h"

namespace AWT
{
   template <class T>
   class QuadTreeImage;

   template <class T>
   class QuadTree
   {
      friend class QuadTreeImage<T>;

   public:
      QuadTree( const T value );

      ~QuadTree( );

      void createChildren( );

      void destroyChildren( );

      T minValue;
      T maxValue;

      QuadTree* children[4];
   };

   template <class T>
   class QuadTreeImage : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<QuadTreeImage> P;

   protected:
      QuadTreeImage( const unsigned int w, const unsigned int h );
      ~QuadTreeImage( );

   public:
      static P getInstance( const unsigned int w, const unsigned int h );
      std::string getClassName( ) const;

      void setValue( const unsigned int x, const unsigned int y, const T value );
      T getValue( const unsigned int x, const unsigned int y ) const;

      void fill( const T value );

      unsigned int getWidth( ) const;
      unsigned int getHeight( ) const;

      void visit( const T lower, const T upper, typename ImageVisitor<T>::P visitor );

      friend std::ostream& operator<<( std::ostream& os, const typename QuadTreeImage<T>::P& im )
      {
         for ( unsigned int y = 0; y < im->getHeight( ); ++y )
         {
            for ( unsigned int x = 0; x < im->getWidth( ); ++x )
            {
               os << im->getValue( x, y ) << "  ";
            }
            os << std::endl;
         }

         return os;
      }

   protected:
      struct D;
      D* m_D;

   };
}

template <class T>
struct AWT::QuadTreeImage<T>::D
{
   D( )
   {
      m_Root = 0;
      fill( 0 );
   }

   ~D( )
   {
      delete m_Root;
   }

   // To fill the image, we can get rid of the entire tree
   // and replace it with an unbranched root with
   void fill( T value )
   {
      if ( m_Root != 0 )
         delete m_Root;

      m_Root = new QuadTree<T>( value );
   }

   void visit( const T lower, const T upper, typename AWT::ImageVisitor<T>::P visitor )
   {
      visitRecursive( lower, upper, visitor, m_Root, 0, 0, m_Width, m_Height );
   }

   bool visitRecursive( const T lower, const T upper, typename AWT::ImageVisitor<T>::P visitor,
      QuadTree<T>* branch, const unsigned int x0, const unsigned int y0,
      const unsigned int x1, const unsigned int y1 )
   {
      bool keepGoing = true;

      if ( lower <= branch->maxValue && upper >= branch->minValue )
      {
         // This means that there are points to be found...

         if ( branch->children[0] != 0 )
         {
            // We need to descend into all branches
            const unsigned int xs[] = { x0, ( x0 + x1 ) / 2, x1 };
            const unsigned int ys[] = { y0, ( y0 + y1 ) / 2, y1 };

            for ( unsigned int yi = 0; yi < 2 && keepGoing; ++yi )
               for ( unsigned int xi = 0; xi < 2 && keepGoing; ++xi )
               {
                  // Checking here should be more efficient, since things haven't had to be pushed on
                  // and off the stack
                  if ( lower <= branch->children[ 2*yi + xi ]->maxValue && upper >= branch->children[ 2*yi + xi ]->minValue )
                  {
                     //DEBUGMACRO( "Descending into ("<<xs[xi+0]<<","<<ys[yi+0]<<") -> ("<<xs[xi+1]<<","<<ys[yi+1]<<")" );
                     keepGoing = visitRecursive( lower, upper, visitor, branch->children[ 2*yi + xi ], xs[xi+0], ys[yi+0], xs[xi+1], ys[yi+1] );
                  }
                  else
                  {
                     //DEBUGMACRO( "Avoiding ("<<xs[xi+0]<<","<<ys[yi+0]<<") -> ("<<xs[xi+1]<<","<<ys[yi+1]<<")" );
                  }
               }
         }
         else
         {
            // We can't descend, this branch is "flat" valued, so we just visit everything in the region
            for ( unsigned int y = y0; y < y1 && keepGoing; ++y )
               for ( unsigned int x = x0; x < x1 && keepGoing; ++x )
                  keepGoing = visitor->visit( x, y, 0, branch->minValue );
         }
      }
      else
      {
         //DEBUGMACRO( "Skipping ("<<x0<<","<<y0<<") -> ("<<x1<<","<<y1<<")" );
      }

      return keepGoing;
   }

   T getValue( const unsigned int x, const unsigned int y )
   {
      // Check to make sure it is inside the image
      if ( x < 0 || x >= m_Width || y < 0 || y >= m_Height )
         AWTEXCEPTIONTHROW( "Requested pixel is outside the image!" );

      return getValueRecursive( x, y, m_Root, 0, 0, m_Width, m_Height );
   }

   void setValue( const unsigned int x, const unsigned int y, const T value )
   {
      // Check to make sure it is inside the image
      if ( x < 0 || x >= m_Width || y < 0 || y >= m_Height )
         AWTEXCEPTIONTHROW( "Requested pixel is outside the image!" );

      setValueRecursive( x, y, value, m_Root, 0, 0, m_Width, m_Height );
   }

   T getValueRecursive( const unsigned int x, const unsigned int y, QuadTree<T>* branch, 
      const unsigned int x0, const unsigned int y0,
      const unsigned int x1, const unsigned int y1 )
   {
      // Check to see if we need to descend further
      // If the min and maxes are the same, we don't
      if ( branch->minValue == branch->maxValue )
         return branch->minValue;

      // Now, we need to work out which of the branches to descend into
      const unsigned int xs[] = { x0, ( x0 + x1 ) / 2, x1 };
      const unsigned int ys[] = { y0, ( y0 + y1 ) / 2, y1 };

      // Assumed child ordering: 
      // [ 0 1 ] === [ TL TR ]
      // [ 2 3 ]     [ BL BR ]
      const unsigned int xi = ( x < xs[1] ) ? 0 : 1;
      const unsigned int yi = ( y < ys[1] ) ? 0 : 1;

      return getValueRecursive( x, y, branch->children[xi + 2*yi], xs[0+xi], ys[0+yi], xs[1+xi], ys[1+yi] );
   }

   void setValueRecursive( const unsigned int x, const unsigned int y, const T value, QuadTree<T>* branch, 
      const unsigned int x0, const unsigned int y0,
      const unsigned int x1, const unsigned int y1 )
   {
      // Check to see if we need to descend further
      // If the min, max and new value are the same, we don't need to do anything
      if ( branch->minValue == branch->maxValue && branch->minValue == value )
         return;

      // Right.  So.  Now I need to know whether to descend further or not
      // We need to descend until we get down to a single pixel
      if ( x1-x0 == 1 && y1-y0 == 1 )
      {
         // This is a single pixel, there is no need to descend any further
         // Set the min and max values to the new value
         branch->minValue = branch->maxValue = value;
      }
      else
      {
         // This is still not a single pixel, so we need to decide which subtree to descend into

         // In fact, we first need to make sure that the subtrees exist
         if ( branch->children[0] == 0 )
            branch->createChildren( );

         // Now, we need to work out which of the branches to descend into
         // Find the mid-point of the box
         const unsigned int xs[] = { x0, ( x0 + x1 ) / 2, x1 };
         const unsigned int ys[] = { y0, ( y0 + y1 ) / 2, y1 };

         // Assumed child ordering: 
         // [ 0 1 ] === [ TL TR ]
         // [ 2 3 ]     [ BL BR ]
         const unsigned int xi = ( x < xs[1] ) ? 0 : 1;
         const unsigned int yi = ( y < ys[1] ) ? 0 : 1;

         setValueRecursive( x, y, value, branch->children[xi + 2*yi], xs[0+xi], ys[0+yi], xs[1+xi], ys[1+yi] );

         // Unlike getting, we don't stop after setting.  We will update the min and max values,
         
         branch->minValue = branch->children[0]->minValue;
         branch->maxValue = branch->children[0]->maxValue;

         for ( unsigned int i = 1; i < 4; ++i )
         {
            branch->minValue = std::min( branch->minValue, branch->children[i]->minValue );
            branch->maxValue = std::max( branch->maxValue, branch->children[i]->maxValue );
         }

         // If the max and min values are the same, we can prune all child branches!
         if ( branch->minValue == branch->maxValue )
            branch->destroyChildren( );
      }
   }

   unsigned int m_Width;
   unsigned int m_Height;

   QuadTree<T>* m_Root;
};

template <class T>
AWT::QuadTreeImage<T>::QuadTreeImage( const unsigned int w, const unsigned int h )
{
   m_D = new D;

   m_D->m_Width = w;
   m_D->m_Height = h;
}

template <class T>
AWT::QuadTreeImage<T>::~QuadTreeImage( )
{
   delete m_D;
}

template <class T>
typename AWT::QuadTreeImage<T>::P AWT::QuadTreeImage<T>::getInstance( const unsigned int w, const unsigned int h )
{
   AUTOGETINSTANCE( AWT::QuadTreeImage<T>, ( w, h ) );
}

template <class T>
GETNAMEMACRO( AWT::QuadTreeImage<T> );

template <class T>
void AWT::QuadTreeImage<T>::setValue( const unsigned int x, const unsigned int y, const T value )
{
   m_D->setValue( x, y, value );
}

template <class T>
T AWT::QuadTreeImage<T>::getValue( const unsigned int x, const unsigned int y ) const
{
   return m_D->getValue( x, y );
}

template <class T>
void AWT::QuadTreeImage<T>::fill( const T value )
{
   m_D->fill( value );
}

template <class T>
unsigned int AWT::QuadTreeImage<T>::getWidth( ) const
{
   return m_D->m_Width;
}

template <class T>
unsigned int AWT::QuadTreeImage<T>::getHeight( ) const
{
   return m_D->m_Height;
}

template <class T>
void AWT::QuadTreeImage<T>::visit( const T lower, const T upper, typename AWT::ImageVisitor<T>::P visitor )
{
   m_D->visit( lower, upper, visitor );
}

template <class T>
AWT::QuadTree<T>::QuadTree( const T value )
{
   for ( unsigned int i = 0; i < 4; ++i )
      children[i] = 0;

   minValue = maxValue = value;
}

template <class T>
AWT::QuadTree<T>::~QuadTree( )
{
   // Recursively delete the children
   destroyChildren( );
}

template <class T>
void AWT::QuadTree<T>::createChildren( )
{
   if ( minValue != maxValue || children[0] != 0 )
      AWTEXCEPTIONTHROW( "Already have children!" );

   for ( unsigned int i = 0; i < 4; ++i )
      children[i] = new QuadTree( minValue );
}

template <class T>
void AWT::QuadTree<T>::destroyChildren( )
{
   for ( unsigned int i = 0; i < 4; ++i )
   {
      if ( children[i] != 0 )
         delete children[i];

      children[i] = 0;
   }
}

#endif // __QUADTREEIMAGE_H__

