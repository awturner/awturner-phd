/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
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
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp (void);
   void tearDown (void);

protected:
   void doubleCreationTest();
   void doubleInsertionTest();

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