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
#include "vtkCleanPolyData.h"
#include "vtkPolyDataWriter.h"
#include "vtkTriangleFilter.h"

int main(int argc, char** argv)
{
   vtkPolyDataReader* reader = vtkPolyDataReader::New();
   vtkCleanPolyData*  clean  = vtkCleanPolyData::New();
   vtkTriangleFilter* trig   = vtkTriangleFilter::New();
   vtkCleanPolyData*  clean2 = vtkCleanPolyData::New();
   vtkPolyDataWriter* writer = vtkPolyDataWriter::New();

   trig->PassLinesOff();
   trig->PassVertsOff();

   clean->SetInputConnection(reader->GetOutputPort());
   trig->SetInputConnection(clean->GetOutputPort());
   clean2->SetInputConnection(trig->GetOutputPort());
   writer->SetInputConnection(clean2->GetOutputPort());

   for (int i = 1; i < argc; i += 2)
   {
      reader->SetFileName(argv[i]);
      writer->SetFileName(argv[i+1]);

      writer->Update();
   }

   writer->Delete();
   clean2->Delete();
   trig->Delete();
   clean->Delete();
   reader->Delete();
}