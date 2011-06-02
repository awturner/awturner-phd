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
#include "VTKMeshLoader.h"

#include "Useful/PrintMacros.h"

#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkCleanPolyData.h"
#include "vtkCleanPolyData.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkPLYReader.h"
#include "vtkOBJReader.h"
#include "vtkAlgorithm.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

#include "SimpleMesh/MeshIO.h"
#include <fstream>

#include <string>

#include "Mesh/Mesh.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/MeshBoundaryFinder.h"

#include "Useful/Exception.h"

template <class T>
struct AWT::VTKMeshLoader<T>::D
{
   static vtkAlgorithm* getVTKReader( const char* filename )
   {
      vtkPolyDataReader* reader = vtkPolyDataReader::New( );
      reader->SetFileName( filename );

      reader->Update( );
      
      return reader;
   }

   static vtkAlgorithm* getPLYReader( const char* filename )
   {
      vtkPLYReader* reader = vtkPLYReader::New( );
      reader->SetFileName( filename );

      reader->Update( );

      return reader;
   }

   static vtkAlgorithm* getOBJReader( const char* filename )
   {
      vtkOBJReader* reader = vtkOBJReader::New( );
      reader->SetFileName( filename );

      reader->Update( );

      return reader;
   }

   static typename Mesh<T>::P simpleMeshToMesh( const char* filename )
   {
      SimpleMesh::Mesh::P simpleMesh = SimpleMesh::MeshIO::loadMesh( std::ifstream( filename ) );

      Mesh<T>::P ret = MeshImpl<T>::getInstance( simpleMesh->nv, simpleMesh->nf );

      SimpleMesh::Points& simpleVertices = simpleMesh->getVertices();
      SimpleMesh::Faces&  simpleFaces    = simpleMesh->getFaces();

      T vtx[3];
      for ( MeshIndex i = 0; i < simpleMesh->nv; ++i )
      {
         for ( MeshIndex ax = 0; ax < 3; ++ax )
            vtx[ax] = simpleVertices(ax,i);

         ret->setVertex( i, vtx );
      }

      MeshIndex vs[3];
      for ( MeshIndex i = 0; i < simpleMesh->nf; ++i )
      {
         for ( MeshIndex ax = 0; ax < 3; ++ax )
            vs[ax] = simpleFaces(ax,i);

         ret->setFaceIndices( i, vs[0], vs[1], vs[2] );
      }

      SimpleMesh::TexCoords& tex = simpleMesh->getTexCoords();
      PRINTVBL(filename);
      DEBUGMACRO(tex.cols() << "\t" << tex.rows());
      if (tex.cols() == simpleMesh->nv)
      {
         Tuples<T>::P texCoord = TuplesImpl<T>::getInstance(2,simpleMesh->nv);
         T zero[] = { 0, 0 };

         for ( MeshIndex i = 0; i < simpleMesh->nv; ++i )
         {
            texCoord->setPoint(i,zero);
            for ( MeshIndex ax = 0; ax < 2; ++ax )
               texCoord->setPointElement(i, ax, tex(ax,i));
         }

         ret->setTextureCoords(texCoord);
      }

      return ret;
   }
};

template <class T>
AWT::VTKMeshLoader<T>::VTKMeshLoader( )
{
}

template <class T>
typename AWT::Mesh<T>::P AWT::VTKMeshLoader<T>::load( const char* filename, bool doClean )
{
   std::string name( filename );

   if ( 0 == name.compare( name.length( )-6, 6, ".smesh" ) )
   {
      return D::simpleMeshToMesh( filename );
   }

   vtkAlgorithm* algo = 0;

   //PRINTVBL( filename );

   if ( 0 == name.compare( name.length( )-4, 4, ".vtk" ) )
      algo = D::getVTKReader( filename );
   else if ( 0 == name.compare( name.length( )-4, 4, ".ply" ) )
      algo = D::getPLYReader( filename );
   else if ( 0 == name.compare( name.length( )-4, 4, ".obj" ) )
      algo = D::getOBJReader( filename );

   if ( algo == 0 )
   {
      AWTEXCEPTIONTHROW_STREAM( "No suitable loader for " << filename );
   }

   //std::cerr << "Loading mesh from " << filename << std::endl;
   vtkPolyData* poly;

   if ( doClean )
   {
      vtkCleanPolyData* clean1 = vtkCleanPolyData::New( );
      clean1->SetInputConnection( algo->GetOutputPort( ) );

      clean1->Update( );
      
      vtkTriangleFilter* trig = vtkTriangleFilter::New( );
      trig->SetInputConnection( clean1->GetOutputPort( ) );
      trig->PassLinesOff( );
      trig->PassVertsOff( );

      vtkCleanPolyData* clean2 = vtkCleanPolyData::New( );
      clean2->SetInputConnection( trig->GetOutputPort( ) );

      clean2->Update( );

      poly = vtkPolyData::New( );
      poly->DeepCopy( clean2->GetOutput( ) );

      clean2->Delete( );
      trig->Delete( );
      clean1->Delete( );
   }
   else
   {
      poly = vtkPolyData::New( );
      poly->DeepCopy( reinterpret_cast<vtkPolyDataAlgorithm*>( algo )->GetOutput( ) );
   }

   typename AWT::Mesh<T>::P ret = convert( poly );

   poly->Delete( );
   algo->Delete( );

   //std::cerr << "Finished!" << std::endl;
   return ret;
}

#include "vtkPolyDataConnectivityFilter.h"

template <class T>
typename AWT::Mesh<T>::P AWT::VTKMeshLoader<T>::convert( vtkPolyData* poly )
{
   typename AWT::Mesh<T>::P ret = AWT::MeshImpl<T>::getInstance( poly->GetNumberOfPoints( ), poly->GetNumberOfCells( ) );

   //PRINTVBL( poly->GetNumberOfPoints( ) );
   double vertDouble[3];
   T vertT[3];
   for ( vtkIdType v = 0, vmax = poly->GetNumberOfPoints(); v < vmax; ++v )
   {
      poly->GetPoint( v, vertDouble );

      vertT[0] = static_cast<T>( vertDouble[0] );
      vertT[1] = static_cast<T>( vertDouble[1] );
      vertT[2] = static_cast<T>( vertDouble[2] );

      ret->setVertex( v, vertT );
   }
   //PRINTVBL( ret->getNumberOfVertices( ) );

   poly->BuildCells( );

   vtkIdType npts;
   vtkIdType* pts;
   vtkIdType cc = 0;

   for ( vtkIdType c = 0; c < poly->GetNumberOfCells( ); ++c )
   {
      // pts is allocated in this function call; don't delete it
      poly->GetCellPoints( c, npts, pts );

      double pointA[3]; poly->GetPoint( pts[0], pointA );
      double pointB[3]; poly->GetPoint( pts[1], pointB );
      double pointC[3]; poly->GetPoint( pts[2], pointC );

      double det = pointA[0]*pointB[1]*pointC[2]-pointA[0]*pointC[1]*pointB[2]-pointA[1]*pointB[0]*pointC[2]+pointA[1]*pointC[0]*pointB[2]+pointA[2]*pointB[0]*pointC[1]-pointA[2]*pointC[0]*pointB[1];
      
      if ( true || det != 0 )
         ret->setFaceIndices( cc++, pts[0], pts[1], pts[2] );
      else
         DEBUGMACRO( "Rejecting face " << c );
   }

   vtkDataArray* coords = poly->GetPointData( )->GetTCoords( );
   if ( coords != 0 )
   {
      vtkIdType N = coords->GetNumberOfTuples( );
      Tuples<T>::P tcoords = TuplesImpl<T>::getInstance( 2, N );

      double tc[2];
      T ttc[2];
      for ( vtkIdType i = 0; i < N; ++i )
      {
         coords->GetTuple( i, tc );
         ttc[0] = tc[0]; ttc[1] = tc[1];
         tcoords->setPoint( i, ttc );
      }

      ret->setTextureCoords( tcoords );
   }
   else
   {
      //DEBUGMACRO( "No texture coordinates!" );
   }

   return ret;
}

template class AWT::VTKMeshLoader<double>;
template class AWT::VTKMeshLoader<float>;