#ifndef __BSPLINE_H__
#define __BSPLINE_H__

namespace AWT
{
   template <class T>
   class BSpline
   {
   public:
      static T evaluate( T u );

      BSpline( const int npoints );
      ~BSpline( );

      T getValue( T u ) const;

   protected:
      T* m_Points;
      int m_NPoints;
   };
}

#endif // __BSPLINE_H__