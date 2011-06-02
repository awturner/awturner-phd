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
#include "vtkPLYReader.h"
#include "vtkSTLReader.h"
#include "vtkSTLWriter.h"
#include "vtkPLYWriter.h"

#include <sstream>

int main(int argc, char** argv)
{
   char suffix[5];
   vtkPolyDataWriter* writer;

   if (strcmp(argv[1], "ply") == 0)
   {
      strcpy_s(suffix, argv[1]);
      writer = vtkPLYWriter::New();
      writer->SetFileTypeToBinary();
   }
   else if (strcmp(argv[1], "stl") == 0)
   {
      strcpy_s(suffix, argv[1]);
      writer = vtkSTLWriter::New();
      writer->SetFileTypeToBinary();
   }
   else if (strcmp(argv[1], "vtk") == 0)
   {
      strcpy_s(suffix, argv[1]);
      writer = vtkPolyDataWriter::New();
   }
   else
   {
      std::cerr << "Invalid type: " << argv[1] << "; exiting" << std::endl;
      exit(1);
   }

   for (int i = 2; i < argc; ++i)
   {
      std::stringstream sstr;
      sstr << argv[i] << "." << suffix;
      writer->SetFileName(sstr.str().c_str());

      std::string name(argv[i]);
      if (0 == name.compare(name.length()-4, 4, ".vtk"))
      {
         vtkPolyDataReader* reader = vtkPolyDataReader::New();
         reader->SetFileName(argv[i]);

         std::cerr << "Reading " << name << "..." << std::endl;
         reader->Update();
         std::cerr << "Done." << std::endl;

         writer->SetInputConnection(reader->GetOutputPort());

         std::cerr << "Writing " << sstr.str() << "..." << std::endl;
         writer->Update();
         std::cerr << "Done." << std::endl;

         reader->Delete();
      }
      else
      {
         vtkPolyDataAlgorithm* reader;

         if (0 == name.compare(name.length()-4, 4, ".ply"))
         {
            reader = vtkPLYReader::New();
            dynamic_cast<vtkPLYReader*>(reader)->SetFileName(argv[i]);
         }
         else if (0 == name.compare(name.length()-4, 4, ".stl"))
         {
            reader = vtkSTLReader::New();
            dynamic_cast<vtkSTLReader*>(reader)->SetFileName(argv[i]);
         }

         std::cerr << "Reading " << name << "..." << std::endl;
         reader->Update();
         std::cerr << "Done." << std::endl;

         writer->SetInputConnection(reader->GetOutputPort());

         std::cerr << "Writing " << sstr.str() << "..." << std::endl;
         writer->Update();
         std::cerr << "Done." << std::endl;

         reader->Delete();
      }
   }

   writer->Delete();
}