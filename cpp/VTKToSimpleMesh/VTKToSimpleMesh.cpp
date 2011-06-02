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
#include "Mesh/VTKMeshLoader.h"

#include <fstream>

using namespace AWT;

int main(int argc, char** argv)
{
   for (int i = 1; i < argc; ++i)
   {
      Mesh<double>::P mesh = VTKMeshLoader<double>::load(argv[i], false);

      char buffer[256];
      sprintf_s(buffer, "%s.smesh", argv[i]);

      std::ofstream os(buffer);

      // Write the vertices
      os << "v" "\t" << mesh->getNumberOfVertices() << std::endl;

      double vtx[3];
      MESH_EACHVERTEX(mesh, v)
      {
         mesh->getVertex(v, vtx);

         os << vtx[0] << "\t" << vtx[1] << "\t" << "\t" << vtx[2] << "\n";
      }
      
      // Write the faces
      os << "f" "\t" << mesh->getNumberOfFaces() << std::endl;

      MeshIndex fac[3];
      MESH_EACHFACE(mesh, f)
      {
         mesh->getFaceIndices(f, fac);

         os << fac[0] << "\t" << fac[1] << "\t" << "\t" << fac[2] << "\n";
      }

      if (mesh->hasTextureCoords())
      {
         Tuples<double>::P tex = mesh->getTextureCoords();

         os << "t" "\t" << tex->getNumberOfPoints() << "\n";

         double vtx[3];
         MESH_EACHVERTEX(mesh, v)
         {
            tex->getPoint(v,vtx);

            os << vtx[0] << "\t" << vtx[1] << "\n";
         }
      }

      os << "e" << std::endl;
      os.close();

      DEBUGMACRO("Written " << argv[i] << " to " << buffer);
   }
}