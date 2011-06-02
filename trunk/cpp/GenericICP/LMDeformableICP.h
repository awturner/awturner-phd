#ifndef __LMDEFORMABLEICP_H__
#define __LMDEFORMABLEICP_H__

#include "ObjectManagement/ManagedObject.h"

#include "vnl/vnl_least_squares_function.h"

#include "DeformableMesh/AddTuples.h"
#include "Mesh/Tuples.h"
#include "vnl/vnl_matrix_fixed.h"


namespace AWT
{
   class LMDeformableICP : public ManagedObject, public vnl_least_squares_function
   {
   public:
      typedef ManagedAutoPointer<LMDeformableICP> P;

   protected:
      LMDeformableICP( Tuples<double>::P movable, Tuples<double>::P movableNormals, AddTuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) );
      virtual ~LMDeformableICP( );

   public:
      static vnl_matrix_fixed<double,4,4> makeTransformation( const vnl_vector<double>& params, const double centroid[3] );

   protected:
      struct D;
      D* m_D;
   };

   class LMMultiDeformableICP : public ManagedObject, public vnl_least_squares_function
   {
   public:
      typedef ManagedAutoPointer<LMMultiDeformableICP> P;

   protected:
      LMMultiDeformableICP( unsigned int nvars, unsigned int nresid, double (*func)( double ) );
      virtual ~LMMultiDeformableICP( );

   public:
      static P getInstance( unsigned int nvars, unsigned int nresid, double (*func)( double ) );
      std::string getClassName( ) const;

      void addPair( Tuples<double>::P movable, Tuples<double>::P movableNormals, AddTuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3] );

      virtual void f( const vnl_vector<double>& x, vnl_vector<double>& residuals );

   protected:
      struct D;
      D* m_D;
   };

   class LMDeformableICP_PointPoint : public LMDeformableICP
   {
   public:
      typedef ManagedAutoPointer<LMDeformableICP> P;

   protected:
      LMDeformableICP_PointPoint( Tuples<double>::P movable, Tuples<double>::P movableNormals, AddTuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) );
      virtual ~LMDeformableICP_PointPoint( );

   public:
      static P getInstance( Tuples<double>::P movable, Tuples<double>::P movableNormals, AddTuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) );
      std::string getClassName( ) const;

      virtual void f( const vnl_vector<double>& x, vnl_vector<double>& residuals );
   };

   class LMDeformableICP_PointPlane : public LMDeformableICP
   {
   public:
      typedef ManagedAutoPointer<LMDeformableICP> P;

   protected:
      LMDeformableICP_PointPlane( Tuples<double>::P movable, Tuples<double>::P movableNormals, AddTuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) );
      virtual ~LMDeformableICP_PointPlane( );

   public:
      static P getInstance( Tuples<double>::P movable, Tuples<double>::P movableNormals, AddTuples<double>::P fixed, Tuples<double>::P fixedNormals, const double centroid[3], double (*func)( double ) );
      std::string getClassName( ) const;

      virtual void f( const vnl_vector<double>& x, vnl_vector<double>& residuals );
   };
}

#endif // __LMDEFORMABLEICP_H__