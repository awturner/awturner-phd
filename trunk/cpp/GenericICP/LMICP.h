#ifndef __LMICP_H__
#define __LMICP_H__

#include "ObjectManagement/ManagedObject.h"

#include "vnl/vnl_least_squares_function.h"

#include "Mesh/Tuples.h"
#include "vnl/vnl_matrix_fixed.h"

namespace AWT
{
   class LMICP : public ManagedObject, public vnl_least_squares_function
   {
   public:
      typedef ManagedAutoPointer<LMICP> P;

   protected:
      LMICP( Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) );
      virtual ~LMICP( );

   public:
      static vnl_matrix_fixed<double,4,4> makeTransformation( const vnl_vector<double>& params, const double centroid[3] );
   protected:
      struct D;
      D* m_D;

   };

   class LMICP_PointPoint : public LMICP
   {
   public:
      typedef ManagedAutoPointer<LMICP> P;

   protected:
      LMICP_PointPoint( Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) );
      virtual ~LMICP_PointPoint( );

   public:
      static P getInstance( Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) );
      std::string getClassName( ) const;

      virtual void f( const vnl_vector<double>& x, vnl_vector<double>& residuals );
   };

   class LMICP_PointPlane : public LMICP
   {
   public:
      typedef ManagedAutoPointer<LMICP> P;

   protected:
      LMICP_PointPlane( Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) );
      virtual ~LMICP_PointPlane( );

   public:
      static P getInstance( Tuples<double>::P movable, Tuples<double>::P movableNormals, Tuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) );
      std::string getClassName( ) const;

      virtual void f( const vnl_vector<double>& x, vnl_vector<double>& residuals );
   };
}

#endif // __LMICP_H__