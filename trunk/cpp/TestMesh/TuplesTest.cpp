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
#include "TuplesTest.h"

#include <ctime>

#include "Useful/Noise.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TuplesTest );

using namespace AWT;

void TuplesTest::setUp( )
{
   Noise<double>::timeSeed( );

   defaultDoubleTuple_1[0] = rand( ) / (RAND_MAX + 0.0);

   defaultDoubleTuple_2[0] = rand( ) / (RAND_MAX + 0.0);
   defaultDoubleTuple_2[1] = rand( ) / (RAND_MAX + 0.0);

   defaultDoubleTuple_3[0] = rand( ) / (RAND_MAX + 0.0);
   defaultDoubleTuple_3[1] = rand( ) / (RAND_MAX + 0.0);
   defaultDoubleTuple_3[2] = rand( ) / (RAND_MAX + 0.0);

   defaultDoubleTuple_4[0] = rand( ) / (RAND_MAX + 0.0);
   defaultDoubleTuple_4[1] = rand( ) / (RAND_MAX + 0.0);
   defaultDoubleTuple_4[2] = rand( ) / (RAND_MAX + 0.0);
   defaultDoubleTuple_4[3] = rand( ) / (RAND_MAX + 0.0);

   doubleTuple_1 = 0;
   doubleTuple_2 = 0;
   doubleTuple_3 = 0;
   doubleTuple_4 = 0;
}

void TuplesTest::tearDown( )
{
   doubleTuple_1.set( 0 );
   doubleTuple_2.set( 0 );
   doubleTuple_3.set( 0 );
   doubleTuple_4.set( 0 );
}

void TuplesTest::doubleCreationTest( )
{
   // Make sure that the variables are all uninitialized to start with
   CPPUNIT_ASSERT_EQUAL( *doubleTuple_1, *AWT::Tuples<double>::P( ) );
   CPPUNIT_ASSERT_EQUAL( *doubleTuple_2, *AWT::Tuples<double>::P( ) );
   CPPUNIT_ASSERT_EQUAL( *doubleTuple_3, *AWT::Tuples<double>::P( ) );
   CPPUNIT_ASSERT_EQUAL( *doubleTuple_4, *AWT::Tuples<double>::P( ) );

   doubleTuple_1.set( *AWT::TuplesImpl<double>::getInstance( 1, &defaultDoubleTuple_1[0], 100 ) );
   CPPUNIT_ASSERT_ASSERTION_PASS( doubleTuple_1 );
   CPPUNIT_ASSERT_EQUAL( doubleTuple_1->getNumberOfPoints( ), static_cast<AWT::MeshIndex>( 0 ) );
   CPPUNIT_ASSERT_EQUAL( doubleTuple_1->getTupleSize( ), static_cast<AWT::TupleIndex>( 1 ) );
   
   doubleTuple_2.set( *AWT::TuplesImpl<double>::getInstance( 2, &defaultDoubleTuple_2[0], 100 ) );
   CPPUNIT_ASSERT_ASSERTION_PASS( doubleTuple_2 );
   CPPUNIT_ASSERT_EQUAL( doubleTuple_2->getNumberOfPoints( ), static_cast<AWT::MeshIndex>( 0 ) );
   CPPUNIT_ASSERT_EQUAL( doubleTuple_2->getTupleSize( ), static_cast<AWT::TupleIndex>( 2 ) );

   doubleTuple_3.set( *AWT::TuplesImpl<double>::getInstance( 3, &defaultDoubleTuple_3[0], 100 ) );
   CPPUNIT_ASSERT_ASSERTION_PASS( doubleTuple_3 );
   CPPUNIT_ASSERT_EQUAL( doubleTuple_3->getNumberOfPoints( ), static_cast<AWT::MeshIndex>( 0 ) );
   CPPUNIT_ASSERT_EQUAL( doubleTuple_3->getTupleSize( ), static_cast<AWT::TupleIndex>( 3 ) );

   doubleTuple_4.set( *AWT::TuplesImpl<double>::getInstance( 4, &defaultDoubleTuple_4[0], 100 ) );
   CPPUNIT_ASSERT_ASSERTION_PASS( doubleTuple_4 );
   CPPUNIT_ASSERT_EQUAL( doubleTuple_4->getNumberOfPoints( ), static_cast<AWT::MeshIndex>( 0 ) );
   CPPUNIT_ASSERT_EQUAL( doubleTuple_4->getTupleSize( ), static_cast<AWT::TupleIndex>( 4 ) );
}

#include "Useful/PrintMacros.h"

void TuplesTest::doubleInsertionTest( )
{
   doubleTuple_3 = AWT::TuplesImpl<double>::getInstance( 3, &defaultDoubleTuple_3[0], 100 );
   double point[3];

   CPPUNIT_ASSERT_ASSERTION_PASS( *doubleTuple_3 );

   MeshIndex startNumberOfPoints = doubleTuple_3->getNumberOfPoints( );

   for ( MeshIndex i = 0; false && i < 10; ++i )
   {
      CPPUNIT_ASSERT_EQUAL( doubleTuple_3->getNumberOfPoints( ), i + startNumberOfPoints );

      MeshIndex currentNumberOfPoints = doubleTuple_3->getNumberOfPoints( );

      point[0] = rand( ) / (RAND_MAX+0.0);
      point[1] = rand( ) / (RAND_MAX+0.0);
      point[2] = rand( ) / (RAND_MAX+0.0);
      doubleTuple_3->setPoint( currentNumberOfPoints, &point[0] );

      CPPUNIT_ASSERT_EQUAL( doubleTuple_3->getNumberOfPoints( ), currentNumberOfPoints + 1 );

      CPPUNIT_ASSERT_EQUAL( doubleTuple_3->getPointElement( startNumberOfPoints + 1, 0 ), point[0] );
      CPPUNIT_ASSERT_EQUAL( doubleTuple_3->getPointElement( startNumberOfPoints + 1, 1 ), point[1] );
      CPPUNIT_ASSERT_EQUAL( doubleTuple_3->getPointElement( startNumberOfPoints + 1, 2 ), point[2] );
   }
}

template class AWT::Tuples<double>;
template class AWT::Tuples<float>;