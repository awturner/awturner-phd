#ifndef __TUPLES_TEST_H__
#define __TUPLES_TEST_H__

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "Mesh/TuplesImpl.h"

class TuplesTest  : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE (TuplesTest);
   CPPUNIT_TEST (doubleCreationTest);
   CPPUNIT_TEST (doubleInsertionTest);
   CPPUNIT_TEST_SUITE_END( );

public:
   void setUp (void);
   void tearDown (void);

protected:
   void doubleCreationTest( );
   void doubleInsertionTest( );

private:
   AWT::Tuples<double>::P doubleTuple_1;
   AWT::Tuples<double>::P doubleTuple_2;
   AWT::Tuples<double>::P doubleTuple_3;
   AWT::Tuples<double>::P doubleTuple_4;

   double defaultDoubleTuple_1[1];
   double defaultDoubleTuple_2[2];
   double defaultDoubleTuple_3[3];
   double defaultDoubleTuple_4[4];
};

#endif // __TUPLES_TEST_H__