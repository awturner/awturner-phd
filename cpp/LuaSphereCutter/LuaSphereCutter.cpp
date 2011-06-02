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
#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"
#include "vtkClipPolyData.h"
#include "vtkSphere.h"
#include "vtkPolyDataWriter.h"

#include "LuaPlus.h"

#include "Useful/PrintMacros.h"

using namespace LuaPlus;

int main( int argc, char** argv )
{
   LuaStateOwner state;

   for ( int i = 1; i < argc; ++i )
   {
      int isOK = state->DoFile( argv[i] );

      if ( isOK != 0 )
      {
         PRINTVBL( isOK );
         DEBUGLINEANDEXIT( 1 );
      }

      LuaObject objFilename = state->GetGlobal( "filename" );
      if ( !objFilename.IsString( ) )
         DEBUGLINEANDEXIT( 1 );

      vtkPolyDataReader* reader = vtkPolyDataReader::New( );
      reader->SetFileName( objFilename.GetString( ) );

      PRINTVBL( objFilename.GetString( ) );

      reader->Update( );

      vtkPolyData* poly = reader->GetOutput( );

      vtkClipPolyData* cutter = vtkClipPolyData::New( );
      vtkSphere* sphere = vtkSphere::New( );

      vtkPolyDataWriter* writerInside = vtkPolyDataWriter::New( );
      vtkPolyDataWriter* writerOutside = vtkPolyDataWriter::New( );

      cutter->SetClipFunction( sphere );
      cutter->SetInputConnection( reader->GetOutputPort( ) );
      cutter->GenerateClippedOutputOn( );

      writerInside->SetInputConnection( cutter->GetOutputPort( 1 ) );
      writerOutside->SetInputConnection( cutter->GetOutputPort( 0 ) );

      LuaObject objPoints = state->GetGlobal( "points" );
      PRINTVBL( objPoints.GetTableCount( ) );
      for ( int i = 1; i <= objPoints.GetTableCount( ); ++i )
      {
         LuaObject thisPoint = objPoints.GetByIndex( i );

         LuaObject objPointName = thisPoint.GetByIndex( 1 );
         PRINTVBL( objPointName.GetString( ) );

         LuaObject objX = thisPoint.GetByIndex( 2 );
         LuaObject objY = thisPoint.GetByIndex( 3 );
         LuaObject objZ = thisPoint.GetByIndex( 4 );

         const double centre[] = { objX.GetNumber( ), objY.GetNumber( ), objZ.GetNumber( ) };

         PRINTVEC( centre, 3 );

         sphere->SetCenter( centre[0], centre[1], centre[2] );

         double pnt[3];
         double maxNorm2 = 0;

         // Calculate the furthest point from this centre
         for ( vtkIdType i = 0, imax = poly->GetNumberOfPoints( ); i < imax; ++i )
         {
            poly->GetPoint( i, pnt );

            double norm2 = 0;
            for ( int i = 0; i < 3; ++i )
            {
               pnt[i] -= centre[i];
               norm2 += pnt[i]*pnt[i];
            }

            if ( norm2 > maxNorm2 )
               maxNorm2 = norm2;
         }

         const double maxR = sqrt( maxNorm2 );

         const int levels[] = { 2, 4, 6, 8, 10, 20, 30, 40, 50 };

         for ( int ii = 0; ii < 9; ++ii )
         {
            const int i = levels[ii];

            sphere->SetRadius( i / 100.0 * maxR );

            PRINTVBL( sphere->GetRadius( ) );

            char fnInside[512];
            sprintf_s( fnInside, "%s.%s.%03d.in.vtk", objFilename.GetString( ), objPointName.GetString( ), i );
            writerInside->SetFileName( fnInside );

            char fnOutside[512];
            sprintf_s( fnOutside, "%s.%s.%03d.out.vtk", objFilename.GetString( ), objPointName.GetString( ), i );
            writerOutside->SetFileName( fnOutside );

            writerInside->Update( );
            writerOutside->Update( );

            PRINTVBL( fnInside );
            PRINTVBL( fnOutside );
         }

         PRINTVBL( maxR );
      }

      writerInside->Delete( );
      cutter->Delete( );
      sphere->Delete( );
      reader->Delete( );
   }
}