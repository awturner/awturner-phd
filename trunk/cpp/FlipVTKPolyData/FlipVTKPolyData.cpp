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
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkTransformFilter.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataWriter.h"
#include "vtkReverseSense.h"
#include "vtkMatrixToHomogeneousTransform.h"

#include "Useful/PrintMacros.h"

#include <iostream>

void usage()
{
   std::cerr << "Usage: FlipVTKPolyData <input file 1> <output file 1> [<input file 2> <output file 2> ...]" << std::endl;
}

int main(int argc, char** argv)
{
   if (argc < 3)
   {
      usage();
      DEBUGLINEANDEXIT(1);
   }

   int curArg = 1;

   int flipAxis = 0;

   if (!strcmp(argv[curArg], "-x"))
   {
      ++curArg;
      flipAxis = 0;
   }
   else if (!strcmp(argv[curArg], "-y"))
   {
      ++curArg;
      flipAxis = 1;
   }
   else if (!strcmp(argv[curArg], "-z"))
   {
      ++curArg;
      flipAxis = 2;
   }

   vtkMatrix4x4* matrix = vtkMatrix4x4::New();
   (*matrix)[flipAxis][flipAxis] = -1;

   vtkMatrixToHomogeneousTransform* matToHom = vtkMatrixToHomogeneousTransform::New();
   matToHom->SetInput(matrix);

   for (int i = curArg; i < argc; i += 2)
   {
      vtkPolyDataReader* reader = vtkPolyDataReader::New();
      reader->SetFileName(argv[i+0]);

      reader->Update();
      std::cerr << "Polydata read from " << argv[i+0] << std::endl;

      vtkPolyData* poly = reader->GetOutput();
      double point[3];
      double centroid[] = { 0, 0, 0 };
      for (int p = 0; p < poly->GetNumberOfPoints(); ++p)
      {
         poly->GetPoint(p, point);

         centroid[0] += point[0];
         centroid[1] += point[1];
         centroid[2] += point[2];
      }

      centroid[0] /= poly->GetNumberOfPoints();
      centroid[1] /= poly->GetNumberOfPoints();
      centroid[2] /= poly->GetNumberOfPoints();

      std::cerr << "Centroid: " << centroid[0] << " " << centroid[1] << " " << centroid[2] << std::endl;
      (*matrix)[0][3] = -centroid[0];
      (*matrix)[1][3] = -centroid[1];
      (*matrix)[2][3] = -centroid[2];

      (*matrix)[flipAxis][3] *= -1;

      vtkTransformFilter* transformFilter = vtkTransformFilter::New();
      transformFilter->SetInputConnection(reader->GetOutputPort());
      transformFilter->SetTransform(matToHom);

      std::cerr << *matToHom << std::endl;

      vtkReverseSense* reverse = vtkReverseSense::New();
      reverse->SetInputConnection(transformFilter->GetOutputPort());

      vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
      writer->SetInputConnection(reverse->GetOutputPort());
      writer->SetFileName(argv[i+1]);

      writer->Update();
      std::cerr << "Polydata written to " << argv[i+1] << std::endl;

      writer->Delete();
      reverse->Delete();
      transformFilter->Delete();
      reader->Delete();
   }

   matrix->Delete();
   matToHom->Delete();
}