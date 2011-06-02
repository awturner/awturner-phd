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
#include "vtkOBJExporter.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"

#include <sstream>

int main(int argc, char** argv)
{
   for (int i = 1; i < argc; ++i)
   {
      vtkPolyDataReader* reader = vtkPolyDataReader::New();
      reader->SetFileName(argv[i]);

      vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
      mapper->SetInputConnection(reader->GetOutputPort());

      vtkActor* actor = vtkActor::New();
      actor->SetMapper(mapper);

      vtkRenderer* ren = vtkRenderer::New();
      ren->AddActor(actor);

      vtkRenderWindow* renWin = vtkRenderWindow::New();
      renWin->AddRenderer(ren);

      vtkOBJExporter* writer = vtkOBJExporter::New();
      
      writer->SetFilePrefix(argv[i]);
      writer->SetRenderWindow(renWin);

      writer->Write();

      writer->Delete();
      renWin->Delete();
      ren->Delete();
      actor->Delete();
      mapper->Delete();
      reader->Delete();
   }
}