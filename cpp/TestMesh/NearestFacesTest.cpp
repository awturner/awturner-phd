#include "NearestFacesTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( NearestFacesTest );

#include "MeshSearching/FacesNearestPointSearch.h"

#include "Useful/PrintMacros.h"
#include "Useful/Noise.h"

#include "Mesh/MeshImpl.h"

using namespace AWT;

void NearestFacesTest::setUp( )
{
   m_Mesh = AWT::MeshImpl<double>::getInstance( 8, 12 );

   m_Npts = 1000;

   AWT::MeshIndex i = 0;
   m_Mesh->setVertex( i++, -0.5, -0.5, -0.5 );
   m_Mesh->setVertex( i++, -0.5,  0.5, -0.5 );
   m_Mesh->setVertex( i++,  0.5,  0.5, -0.5 );
   m_Mesh->setVertex( i++,  0.5, -0.5, -0.5 );
   m_Mesh->setVertex( i++, -0.5, -0.5,  0.5 );
   m_Mesh->setVertex( i++, -0.5,  0.5,  0.5 );
   m_Mesh->setVertex( i++,  0.5,  0.5,  0.5 );
   m_Mesh->setVertex( i++,  0.5, -0.5,  0.5 );

   i = 0;
   m_Mesh->setFaceIndices( i++, 0, 1, 2 );
   m_Mesh->setFaceIndices( i++, 0, 2, 3 );
   m_Mesh->setFaceIndices( i++, 3, 2, 6 );
   m_Mesh->setFaceIndices( i++, 3, 6, 7 );
   m_Mesh->setFaceIndices( i++, 7, 6, 5 );
   m_Mesh->setFaceIndices( i++, 7, 5, 4 );
   m_Mesh->setFaceIndices( i++, 4, 5, 1 );
   m_Mesh->setFaceIndices( i++, 4, 1, 0 );
   m_Mesh->setFaceIndices( i++, 1, 5, 6 );
   m_Mesh->setFaceIndices( i++, 1, 6, 2 );
   m_Mesh->setFaceIndices( i++, 4, 0, 3 );
   m_Mesh->setFaceIndices( i++, 4, 3, 7 );

   m_Mesh->prepareToSearchVertices( );
}

void NearestFacesTest::tearDown( )
{
   m_Mesh->release( );
}

void NearestFacesTest::nearestFacesTestSelf( )
{
   AWT::FacesNearestPointSearch<double>::P searcher = AWT::FacesNearestPointSearch<double>::getInstance( );

   //for ( AWT::MeshIndex i = 0; i < m_Mesh->getNumberOfVertices( ); ++i )
   MESH_EACHVERTEX( m_Mesh, i )
   {
      double pnt[3];

      m_Mesh->getVertex( i, pnt );

      searcher->reset( );
      searcher->setTestPoint( pnt );
      searcher->setSearchType( AWT::SEARCH_RECURSIVE );
      m_Mesh->searchFaces( searcher );
      
      double nearestRecursive[3];
      searcher->getNearestPoint( nearestRecursive );

      for ( int j = 0; j < 3; ++j )
         CPPUNIT_ASSERT( abs(pnt[j]-nearestRecursive[j]) < 1e-8 );

      searcher->reset( );
      searcher->setTestPoint( pnt );
      searcher->setSearchType( AWT::SEARCH_EXHAUSTIVE );
      m_Mesh->searchFaces( searcher );
      
      double nearestExhaustive[3];
      searcher->getNearestPoint( nearestExhaustive );

      for ( int j = 0; j < 3; ++j )
         CPPUNIT_ASSERT( abs(pnt[j]-nearestExhaustive[j]) < 1e-8 );

   }
}

void NearestFacesTest::nearestFacesTestNotSelf( )
{
   AWT::FacesNearestPointSearch<double>::P searcher = AWT::FacesNearestPointSearch<double>::getInstance( );

   for ( AWT::MeshIndex i = 0; i < m_Npts; ++i )
   {
      double pnt[3];

      pnt[0] = AWT::Noise<double>::randu( -10, 10 );
      pnt[1] = AWT::Noise<double>::randu( -10, 10 );
      pnt[2] = AWT::Noise<double>::randu( -10, 10 );

      searcher->reset( );
      searcher->setTestPoint( pnt );
      searcher->setSearchType( AWT::SEARCH_RECURSIVE );
      m_Mesh->searchFaces( searcher );
      
      double nearestRecursive[3];
      searcher->getNearestPoint( nearestRecursive );

      searcher->reset( );
      searcher->setTestPoint( pnt );
      searcher->setSearchType( AWT::SEARCH_EXHAUSTIVE );
      m_Mesh->searchFaces( searcher );
      
      double nearestExhaustive[3];
      searcher->getNearestPoint( nearestExhaustive );

      for ( int j = 0; j < 3; ++j )
         CPPUNIT_ASSERT( abs(nearestRecursive[j]-nearestExhaustive[j]) < 1e-8 );

   }
}
