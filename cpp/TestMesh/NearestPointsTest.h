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
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp (void);
   void tearDown (void);

protected:
   void nearestPointTestSelf();
   void nearestPointTestNotSelf();

private:
   AWT::Mesh<double>::P m_Mesh;
   AWT::MeshIndex       m_Npts;
};

#endif // __TUPLES_TEST_H__