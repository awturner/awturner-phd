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
#include "DeformableMesh/DeformableMesh.h"
#include "DeformableMesh/DeformableMeshLoader.h"

#include "Useful/PrintMacros.h"
#include "Useful/VNLTypeDefs.h"

#include <fstream>
#include <string>

#include "Useful/PrintMacros.h"


using namespace AWT;

int main(int argc, char** argv)
{
   std::string modelFilename;
   std::string weightFilename;
   std::string outModelFilename;

   bool modelFilename_set    = false;
   bool weightFilename_set   = false;
   bool outModelFilename_set = false;

   for (int i = 1; i < argc; ++i)
   {
      if (!strcmp("-model", argv[i]))
      {
         if (++i >= argc)
            PRINTANDEXIT(1, "Missing argument");

         modelFilename = std::string(argv[i]);
         modelFilename_set = true;
      }
      else if (!strcmp("-weights", argv[i]))
      {
         if (++i >= argc)
            PRINTANDEXIT(1, "Missing argument");

         weightFilename = std::string(argv[i]);
         weightFilename_set = true;
      }
      else if (!strcmp("-outModel", argv[i]))
      {
         if (++i >= argc)
            PRINTANDEXIT(1, "Missing argument");

         outModelFilename = std::string(argv[i]);
         outModelFilename_set = true;
      }
      else
      {
         PRINTANDEXIT(1, "Unknown argument: " << argv[i]);
      }
   }
   
   if (!modelFilename_set)
      PRINTANDEXIT(1, "Must set model filename");

   if (!outModelFilename_set)
      PRINTANDEXIT(1, "Must set output model filename");
   
   DeformableMesh<double>::P model = DeformableMeshLoader<double>::load(modelFilename.c_str());
   
   DoubleVector weights(model->getNumberOfActiveModes());
   if (weightFilename_set)
   {
      std::ifstream is(weightFilename.c_str());

      if (!is.good())
         PRINTANDEXIT(1, "Could not open " << weightFilename);

      unsigned int i;
      for (i = 0; i < model->getNumberOfActiveModes() && !is.eof(); ++i)
         is >> weights[i];

      PRINTVBL(model->getNumberOfActiveModes());
      PRINTVBL(i);

      if (i < model->getNumberOfActiveModes())
         PRINTANDEXIT(1, weightFilename << " has too few weight values.");

   }
   else
   {
      weights.fill(0);
   }

   std::vector<int> pointReindex(model->getNumberOfVertices());
   for (unsigned int i = 0; i < model->getNumberOfVertices(); ++i)
      pointReindex[i] = -1;

   MeshIndex vs[3];
   MeshIndex faceIndex = 0;
   
   //for (int f = 0; f < model->getNumberOfFaces(); ++f)
   MESH_EACHFACE(model, f)
   {
      model->getFaceIndices(f, vs);

      for (int v = 0; v < 3; ++v)
      {
         if (pointReindex[ vs[v] ] == -1)
         {
            pointReindex[ vs[v] ] = faceIndex++;
         }
      }
   }

   std::string outPointsFilename("points_");
   outPointsFilename.append(outModelFilename);

   std::cerr << "Writing points to " << outPointsFilename << std::endl;
   std::ofstream outPointsStream(outPointsFilename.c_str());

   model->setModeWeights(weights.data_block());

   faceIndex = 0;
   DoublePoint point;
   //for (unsigned int i = 0; i < model->getNumberOfVertices(); ++i)
   MESH_EACHVERTEX(model, i)
   {
      if (pointReindex[i] != -1)
      {
         pointReindex[i] = faceIndex++;
         model->getVertex(i, point.data_block());

         outPointsStream << point[0] << " " << point[1] << " " << point[2] << std::endl;
      }
   }

   outPointsStream.close();

   std::string outFacesFilename("faces_");
   outFacesFilename.append(outModelFilename);

   std::cerr << "Writing points to " << outFacesFilename << std::endl;
   std::ofstream outFacesStream(outFacesFilename.c_str());

   //for (int i = 0; i < model->getNumberOfFaces(); ++i)
   MESH_EACHFACE(model, f)
   {
      model->getFaceIndices(f, vs);

      outFacesStream << pointReindex[ vs[0] ] << " " << pointReindex[ vs[1] ] << " " << pointReindex[ vs[2] ] << std::endl;
   }

   outFacesStream.close();

   std::cerr << "Writing model to " << outModelFilename << std::endl;

   std::ofstream modelStream(outModelFilename.c_str());

   modelStream << "mode " << outPointsFilename << std::endl;
   modelStream << "face " << outFacesFilename  << std::endl;

   modelStream.close();
}