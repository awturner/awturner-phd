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
#include "NearestPointsTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( NearestPointsTest );

#include "Mesh/MeshImpl.h"
#include "Mesh/MeshTypeDefs.h"

#include "MeshSearching/VerticesNearestVertexSearch.h"

#include "Useful/Noise.h"
#include "Useful/PrintMacros.h"

typedef AWT::Noise<double> DoubleNoise;

using namespace AWT;

void NearestPointsTest::setUp( )
{
   m_Mesh = AWT::MeshImpl<double>::getInstance( 10, 0 );

   m_Npts = 1000;

   for ( AWT::MeshIndex i = 0; i < m_Npts; ++i )
   {
      double pnt[3];

      pnt[0] = DoubleNoise::randu( -10, 10 );
      pnt[1] = DoubleNoise::randu( -10, 10 );
      pnt[2] = DoubleNoise::randu( -10, 10 );
      m_Mesh->setVertex( i, pnt );
   }

   m_Mesh->prepareToSearchVertices( );
}

void NearestPointsTest::tearDown( )
{
   m_Mesh->release( );
}

void NearestPointsTest::nearestPointTestSelf( )
{
   AWT::VerticesNearestVertexSearch<double>::P searcher = AWT::VerticesNearestVertexSearch<double>::getInstance( );

   for ( AWT::MeshIndex i = 0; i < m_Npts; ++i )
   {
      double pntTest[3];
      m_Mesh->getVertex( i, &pntTest[0] );
      
      double pntFoundRecursive[3];
      double pntFoundExhaustive[3];
      AWT::MeshIndex idxFoundRecursive;
      AWT::MeshIndex idxFoundExhaustive;

      searcher->reset( );
      searcher->setTestPoint( &pntTest[0] );
      searcher->setSearchType( AWT::SEARCH_RECURSIVE );
      m_Mesh->searchVertices( searcher );
      idxFoundRecursive = searcher->getNearestPoint( pntFoundRecursive );

      searcher->reset( );
      searcher->setTestPoint( &pntTest[0] );
      searcher->setSearchType( AWT::SEARCH_EXHAUSTIVE );
      m_Mesh->searchVertices( searcher );
      idxFoundExhaustive = searcher->getNearestPoint( pntFoundExhaustive );
      
      CPPUNIT_ASSERT( idxFoundExhaustive == idxFoundRecursive );

      AWT::MeshIndex bestV;
      double bestDistance = std::numeric_limits<double>::infinity( );
      double bestPoint[3];
      double point[3];
      double distance, tmp;

      //for ( MeshIndex v = 0; v < m_Mesh->getNumberOfVertices( ); ++v )
      MESH_EACHVERTEX( m_Mesh, v )
      {
         m_Mesh->getVertex( v, point );

         distance = 0;
         for ( int d = 0; d < 3; ++d )
         {
            tmp = point[d] - pntFoundRecursive[d];
            distance += tmp*tmp;
         }

         if ( distance < bestDistance )
         {
            bestDistance = distance;
            bestPoint[0] = point[0]; bestPoint[1] = point[1]; bestPoint[2] = point[2];
            bestV = v;
         }
      }

      CPPUNIT_ASSERT( abs(bestPoint[0] - pntFoundRecursive[0]) < 1e-6 );
      CPPUNIT_ASSERT( abs(bestPoint[1] - pntFoundRecursive[1]) < 1e-6 );
      CPPUNIT_ASSERT( abs(bestPoint[2] - pntFoundRecursive[2]) < 1e-6 );
      CPPUNIT_ASSERT_EQUAL( idxFoundExhaustive, bestV );
   }
}

void NearestPointsTest::nearestPointTestNotSelf( )
{
   AWT::VerticesNearestVertexSearch<double>::P searcher = AWT::VerticesNearestVertexSearch<double>::getInstance( );

   for ( AWT::MeshIndex i = 0; i < m_Npts; ++i )
   {
      double pntTest[3];
      pntTest[0] = DoubleNoise::randu( -100, 100 );
      pntTest[1] = DoubleNoise::randu( -100, 100 );
      pntTest[2] = DoubleNoise::randu( -100, 100 );
      
      double pntFoundRecursive[3];
      double pntFoundExhaustive[3];
      AWT::MeshIndex idxFoundRecursive;
      AWT::MeshIndex idxFoundExhaustive;

      searcher->reset( );
      searcher->setTestPoint( &pntTest[0] );
      searcher->setSearchType( AWT::SEARCH_RECURSIVE );
      m_Mesh->searchVertices( searcher );
      idxFoundRecursive = searcher->getNearestPoint( pntFoundRecursive );

      searcher->reset( );
      searcher->setTestPoint( &pntTest[0] );
      searcher->setSearchType( AWT::SEARCH_EXHAUSTIVE );
      m_Mesh->searchVertices( searcher );
      idxFoundExhaustive = searcher->getNearestPoint( pntFoundExhaustive );
      
      CPPUNIT_ASSERT_EQUAL( idxFoundExhaustive, idxFoundRecursive );
      
      MeshIndex bestV;
      double bestDistance = std::numeric_limits<double>::infinity( );
      double bestPoint[3];
      double point[3];
      double distance, tmp;

      //for ( MeshIndex v = 0; v < m_Mesh->getNumberOfVertices( ); ++v )
      MESH_EACHVERTEX( m_Mesh, v )
      {
         m_Mesh->getVertex( v, point );

         distance = 0;
         for ( int d = 0; d < 3; ++d )
         {
            tmp = point[d] - pntFoundRecursive[d];
            distance += tmp*tmp;
         }

         if ( distance < bestDistance )
         {
            bestDistance = distance;
            bestPoint[0] = point[0]; bestPoint[1] = point[1]; bestPoint[2] = point[2];
            bestV = v;
         }
      }

      CPPUNIT_ASSERT( abs(bestPoint[0] - pntFoundRecursive[0]) < 1e-6 );
      CPPUNIT_ASSERT( abs(bestPoint[1] - pntFoundRecursive[1]) < 1e-6 );
      CPPUNIT_ASSERT( abs(bestPoint[2] - pntFoundRecursive[2]) < 1e-6 );
      CPPUNIT_ASSERT_EQUAL( idxFoundExhaustive, bestV );
   }
}