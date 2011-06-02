#ifndef __LMICP_ALLMOVABLE_H__
#define __LMICP_ALLMOVABLE_H__

#include "ObjectManagement/ManagedObject.h"

#include "vnl/vnl_least_squares_function.h"

#include "Mesh/Tuples.h"
#include "vnl/vnl_matrix_fixed.h"

#include "LMICP.h"

namespace AWT
{
   class LMICP_AllMovable : public ManagedObject, public vnl_least_squares_function
   {
   public:
      typedef ManagedAutoPointer<LMICP_AllMovable> P;

   protected:
      LMICP_AllMovable( const unsigned int nparams, const unsigned int nresiduals );
      virtual ~LMICP_AllMovable( );

   public:
      std::string getClassName( ) const;
      static P getInstance( const unsigned int nparams, const unsigned int nresiduals );

      void addPoints( Tuples<double>::P pointSet );
      void addCorrespondence( const MeshIndex psA, const MeshIndex pntA, const MeshIndex psB, const MeshIndex pntB );

      virtual void f( const vnl_vector<double>& x, vnl_vector<double>& residuals );

      virtual void debugOn( );
      virtual void debugOff( );


   protected:
      struct D;
      D* m_D;
   };
}

#endif // __LMICP_ALLMOVABLE_H__