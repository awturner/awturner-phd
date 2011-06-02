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
#include "VTKMeshWriter.h"

#include "Mesh/Mesh.h"

#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyDataWriter.h"
#include "vtkPLYWriter.h"
#include "vtkSTLWriter.h"
#include "vtkPolyDataNormals.h"

template <class T>
void AWT::VTKMeshWriter<T>::write(typename AWT::Mesh<T>::P mesh, const char *filename)
{
   vtkPolyData* poly = vtkPolyData::New();

   vtkPoints*    points = vtkPoints::New();
   vtkCellArray* cells  = vtkCellArray::New();

   T vtx[3];
   MESH_EACHVERTEX(mesh, v)
   {
      mesh->getVertex(v, vtx);

      points->InsertNextPoint(vtx);
   }

   MeshIndex idxs[3];
   vtkIdType vidxs[3];

   MESH_EACHFACE(mesh, f)
   {
      mesh->getFaceIndices(f, idxs);

      for (int i = 0; i < 3; ++i)
         vidxs[i] = idxs[i];

      cells->InsertNextCell(3, vidxs);
   }

   poly->SetPoints(points);
   poly->SetPolys(cells);

   if (mesh->hasTextureCoords())
   {
      DEBUGMACRO("Has Texture Coords!");

      vtkDataArray* pd = vtkDoubleArray::New();
      pd->SetNumberOfComponents(2);
      pd->SetNumberOfTuples(mesh->getNumberOfVertices());

      Tuples<T>::P tc = mesh->getTextureCoords();
      T tcv[2];
      MESH_EACHVERTEX(mesh, v)
      {
         tc->getPoint(v, tcv);
         pd->InsertTuple(v, tcv);
      }
      poly->GetPointData()->SetTCoords(pd);
   }
   /*
   else
   {
      vtkDataArray* pd = vtkDoubleArray::New();
      
      pd->SetNumberOfComponents(2);
      pd->SetNumberOfTuples(mesh->getNumberOfVertices());

      T tcv[] = { -100, 100 };
      MESH_EACHVERTEX(mesh, v)
      {
         pd->InsertTuple(v, tcv);
      }
      poly->GetPointData()->SetTCoords(pd);
   }
   */

   vtkPolyDataNormals* normals = vtkPolyDataNormals::New();
   normals->SetInput(poly);
   normals->SplittingOff();

   vtkPolyDataWriter* writer = 0;

   const std::string name(filename);

   if (0 == name.compare(name.length()-4, 4, ".vtk"))
      writer = vtkPolyDataWriter::New();
   else if (0 == name.compare(name.length()-4, 4, ".ply"))
   {
      writer = vtkPLYWriter::New();
      writer->SetFileTypeToBinary();
   }
   else if (0 == name.compare(name.length()-4, 4, ".stl"))
   {
      writer = vtkSTLWriter::New();
      writer->SetFileTypeToBinary();
   }

   if (writer == 0)
      AWTEXCEPTIONTHROW("No writer found");

   writer->SetInputConnection(normals->GetOutputPort());
   writer->SetFileName(filename);

   writer->Update();

   points->Delete();
   cells->Delete();
   writer->Delete();
   poly->Delete();
}

template class AWT::VTKMeshWriter<double>;
template class AWT::VTKMeshWriter<float>;