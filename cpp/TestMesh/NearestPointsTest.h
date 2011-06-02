#ifndef __NEARESTPOINTS_TEST_H__
#define __NEARESTPOINTS_TEST_H__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "Mesh/Mesh.h"

class NearestPointsTest  : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE (NearestPointsTest);
   CPPUNIT_TEST (nearestPointTestSelf);
   CPPUNIT_TEST (nearestPointTestNotSelf);
   CPPUNIT_TEST_SUITE_END( );

public:
   void setUp (void);
   void tearDown (void);

protected:
   void nearestPointTestSelf( );
   void nearestPointTestNotSelf( );

private:
   AWT::Mesh<double>::P m_Mesh;
   AWT::MeshIndex       m_Npts;
};

#endif // __TUPLES_TEST_H__