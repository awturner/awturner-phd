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

#include "Mesh/Mesh.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"

#include "Useful/Exception.h"
#include "Useful/PrintMacros.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_svd.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

using namespace AWT;
using namespace std;

typedef double T;

int main(int argc, char** argv)
{
   int m, n;

   bool firstTime = true;

   vnl_matrix<T> matrix;
   vnl_vector<T> mean;

   for (int i = 2; i < argc; ++i)
   {
      DEBUGMACRO("Loading from " << argv[i]);
      Mesh<T>::P mesh = VTKMeshLoader<T>::load(argv[i], false);

      if (!firstTime)
      {
         if (3*mesh->getNumberOfVertices() != m)
            AWTEXCEPTIONTHROW("Incompatible numbers of vertices!");
      }
      else
      {
         // This is run first time round
         m = 3*mesh->getNumberOfVertices();
         n = argc - 2;

         PRINTVBL(m);
         PRINTVBL(n);

         matrix.set_size(m, n);
         mean.set_size(m);

         matrix.fill(0);
         mean.fill(0);    

         firstTime = false;
      }

      T vtx[3];
      int vv;
      //for (MeshIndex v = 0; v < mesh->getNumberOfVertices(); ++v)
      MESH_EACHVERTEX(mesh, v)
      {
         mesh->getVertex(v, vtx);

         vv = v;

         for (int ax = 0; ax < 3; ++ax)
         {
            mean(3*vv+ax) += (matrix(3*vv+ax, i-2) = vtx[ax]);
         }
      }
   }

   DEBUGMACRO("Calculating mean...");
   mean /= n;

   DEBUGMACRO("Writing mean shape...");
   {
      Mesh<T>::P meanMesh = VTKMeshLoader<T>::load(argv[1], false);
      for (int v = 0; v < m; v += 3)
      {
         meanMesh->setVertex(v/3, mean(v+0), mean(v+1), mean(v+2));
      }
      VTKMeshWriter<T>::write(meanMesh, "meanMesh.vtk");
   }

   DEBUGMACRO("Shifting vectors...");
   for (int w = 0; w < n; ++w)
   {
      matrix.set_column(w, matrix.get_column(w) - mean);
   }

   vnl_svd<T> svd(matrix);

   vnl_matrix<T> mimU = svd.U();
   vnl_matrix<T> mimS = svd.W();

   vnl_matrix<T> modes = mimU * mimS;

   T totalS = 0;
   for (int w = 0; w < n; ++w)
   {
      totalS += mimS(w,w);
   }

   T cumSum = 0;
   for (int w = 0; w < n; ++w)
   {
      cumSum += mimS(w,w);
      std::cerr << (w+1) << "\t" << mimS(w,w) << "\t" << cumSum/totalS << std::endl;
   }

   char filename[MAX_PATH];

   for (int w = 0; w < n-1; ++w)
   {
      sprintf_s(filename, "mode.%04d.dat", w+1);

      DEBUGMACRO("Writing mode " << (w+1) << " to " << filename);

      ofstream os(filename);

      for (int v = 0; v < m; v += 3)
         os << modes(v+0,w) << " " << modes(v+1,w) << " " << modes(v+2,w) << std::endl;

      os.close();
   }

   {
      TCHAR szDirectory[MAX_PATH] = "";
      GetCurrentDirectory(sizeof(szDirectory)-1, szDirectory);

      sprintf_s(filename, "%s\\model.model", szDirectory);

      DEBUGMACRO("Writing model file to " << filename);

      ofstream os(filename);

      sprintf_s(filename, "%s\\meanMesh.vtk", szDirectory);
      os << filename << std::endl;

      for (int w = 0; w < n; ++w)
      {
         sprintf_s(filename, "%s\\mode.%04d.dat", szDirectory, w+1);

         os << filename << std::endl;
      }

      os.close();

      DEBUGMACRO("Done.");
   }   


   DEBUGMACRO("Done.");
}