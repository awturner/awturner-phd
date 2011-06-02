#ifndef __NEARESTFACES_TEST_H__
#define __NEARESTFACES_TEST_H__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "Mesh/Mesh.h"

class NearestFacesTest  : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE (NearestFacesTest);
   CPPUNIT_TEST (nearestFacesTestSelf);
   CPPUNIT_TEST (nearestFacesTestNotSelf);
   CPPUNIT_TEST_SUITE_END( );

public:
   void setUp (void);
   void tearDown (void);

protected:
   void nearestFacesTestSelf( );
   void nearestFacesTestNotSelf( );

private:
   AWT::Mesh<double>::P m_Mesh;
   AWT::MeshIndex       m_Npts;
};

#endif // __NEARESTFACES_TEST_H__