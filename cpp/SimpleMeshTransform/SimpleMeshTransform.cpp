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
#include "SimpleMesh/Mesh.h"
#include "SimpleMesh/MeshIO.h"
#include "SimpleMesh/MeshFunctions.h"
#include "SimpleMesh/ShapeModel.h"

#include <fstream>

using namespace AWT::SimpleMesh;

void readVector(int& i, char** argv, Vector& params)
{
   for (Index m = 0; m < params.size(); ++m)
   {
      const char* modeweightStr = argv[++i];

      if (strcmp(modeweightStr, "-") == 0)
         break;

      params[m] = atof(modeweightStr);
   }

   PRINTVBL(params);
}

void calculateTransformation(const Vector& v, Transformation& mat, const bool normalize = true)
{
   Vector q(4);
   q.set(v.data_block());

   if (normalize)
      q.normalize();

   mat(0,0) = q(0) * q(0) + q(1) * q(1) - q(2) * q(2) - q(3) * q(3);
   mat(0,1) = 2 * q(1) * q(2) - 2 * q(0) * q(3);
   mat(0,2) = 2 * q(1) * q(3) + 2 * q(0) * q(2);

   mat(1,0) = 2 * q(1) * q(2) + 2 * q(0) * q(3);
   mat(1,1) = q(0) * q(0) - q(1) * q(1) + q(2) * q(2) - q(3) * q(3);
   mat(1,2) = 2 * q(2) * q(3) - 2 * q(0) * q(1);       

   mat(2,0) = 2 * q(1) * q(3) - 2 * q(0) * q(2);
   mat(2,1) = 2 * q(2) * q(3) + 2 * q(0) * q(1);
   mat(2,2) = q(0) * q(0) - q(1) * q(1) - q(2) * q(2) + q(3) * q(3);

   mat(0,3) = v(4);
   mat(1,3) = v(5);
   mat(2,3) = v(6);

   mat.set_row(3, 0.0);
   mat(3,3) = 1;
}

int main(int argc, char** argv)
{
   int i = 0;

   Mesh::P model = MeshIO::loadMesh(std::ifstream(argv[++i]));

   const double scale = atof(argv[++i]);
   PRINTVBL(scale);

   Vector vec(7);
   vec.fill(0);
   vec[0] = 1;
   readVector(i, argv, vec);

   PRINTVBL(vec);

   Transformation trans;
   calculateTransformation(vec, trans);

   PRINTVBLNL(trans);

   affineTransform(model->getVertices(), trans);

   // Isotropically scale
   model->getVertices() *= scale;
   model->getVertices().set_row(3, 1);

   const char* outFilename = argv[++i];
   PRINTVBL(outFilename);

   MeshIO::saveMesh(std::ofstream(outFilename), model);
}