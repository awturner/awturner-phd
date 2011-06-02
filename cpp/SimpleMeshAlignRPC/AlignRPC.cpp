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
#include "SimpleMesh/TypeDefs.h"
#include "SimpleMesh/MeshIO.h"
#include "SimpleMesh/MeshFunctions.h"
#include "SimpleMesh/Connectivity.h"
#include "SimpleMesh/Search.h"
#include "SimpleMesh/ShapeModel.h"
#include "SimpleMesh/Aligner.h"
#include "SimpleMesh/NormalSpaceSampler.h"
#include "SimpleMesh/UniformSampler.h"

#include "Useful/Noise.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "Useful/PrintMacros.h"
#include "Useful/Noise.h"
#include "Useful/ArrayVectorFunctions.h"

#include "XmlRpc.h"
#include <iostream>
#include "Windows.h"
#include <map>

using namespace XmlRpc;

#define RPC_METHOD_NAME(server, MethodName, RPName) class MethodName : public XmlRpcServerMethod \
{ \
public: \
   MethodName(XmlRpcServer* server) : XmlRpcServerMethod(#RPName, server) { std::cout << #RPName << std::endl; } \
   void execute(XmlRpcValue& params, XmlRpcValue& result)

#define RPC_METHOD(server, MethodName) RPC_METHOD_NAME(server, MethodName, MethodName)

#define RPC_X(server, MethodName) } __inst_##MethodName(&server);

using namespace AWT::SimpleMesh;

// The server
XmlRpcServer s;

// The aligner
Aligner::P aligner;

// The currently selected sampler
SamplerFactory::P samplerFactory;
Index             samplerNSamples;

std::map<std::string,Mesh::P> meshStore;

Mesh::P getFromMeshStore(const std::string& filename)
{
   if (meshStore.find(filename) == meshStore.end())
   {
      DEBUGMACRO("Loading fragment from " << filename);
      Mesh::P mesh = MeshIO::loadMesh(std::ifstream(filename.c_str()));

      meshStore[filename] = mesh;
   }
   else
   {
      DEBUGMACRO("Already loaded");
   }
   
   return meshStore[filename];
}

void reset()
{
   DEBUGMACRO("Creating a new aligner instance");
   aligner = Aligner::getInstance(Aligner::CF_POINTTOPOINT);

   samplerFactory.set(0);
   samplerNSamples = INVALID_INDEX;
}

void copyVectorIntoXmlRpcValue(const Vector& vec, XmlRpcValue& val)
{
   val.setSize(vec.size());
   for (Index i = 0; i < vec.size(); ++i)
      val[i] = vec[i];
}

void copyXmlRpcValueIntoVector(XmlRpcValue& val, Vector& vec, const Index length)
{
   if (val.size() != length)
   {
      PRINTVBL(val.size());
      PRINTVBL(length);
      throw XmlRpcException("Argument is the wrong length");
   }

   vec.set_size(val.size());
   for (Index i = 0; i < (Index)val.size(); ++i)
   {
      switch (val[i].getType())
      {
      case XmlRpcValue::TypeInt:
         vec[i] = (double)((int)val[i]);
         break;
      default:
         vec[i] = (double)val[i];
      }
   }
}

RPC_METHOD_NAME(s, Reset, reset)
{
   reset();

   result.setSize(0);
}
RPC_X(s, Reset);

RPC_METHOD_NAME(s, GetCostFunction, getCostFunction)
{
   switch (aligner->getCostFunctionType())
   {
   case Aligner::CF_POINTTOPOINT:
      result = "pointtopoint";
      break;

   case Aligner::CF_POINTTOPLANE:
      result = "pointtoplane";
      break;
   }
}
RPC_X(s, GetCostFunction); 

RPC_METHOD_NAME(s, SetCostFunction, setCostFunction)
{
   const std::string cft = params[0];

   if (cft == "pointtopoint")
   {
      DEBUGMACRO("Setting cost function to pointtopoint");
      aligner->setCostFunctionType(Aligner::CF_POINTTOPOINT);
      result[0] = true;
   }
   else if (cft == "pointtoplane")
   {
      DEBUGMACRO("Setting cost function to pointtoplane");
      aligner->setCostFunctionType(Aligner::CF_POINTTOPLANE);
      result[0] = true;
   }
   else
   {
      DEBUGMACRO("Unknown cost function type " << cft);
      result[0] = false;
   }

   result.setSize(0);
}
RPC_X(s, SetCostFunction); 

RPC_METHOD_NAME(s, SelectSampler, selectSampler)
{
   const std::string type = params[0];

   if (type == "uniform")
   {
      samplerFactory = UniformSamplerFactory::getInstance();
      DEBUGMACRO("Selected uniform sampler.");
   }
   else if (type == "normalspace")
   {
      const Index latdivs = (int)params[1];

      samplerFactory = NormalSpaceSamplerFactory::getInstance(latdivs);

      DEBUGMACRO("Selected normal space sampler (" << latdivs << " latitude divisions).");
   }

   result.setSize(0);
}
RPC_X(s, SelectSampler); 

RPC_METHOD_NAME(s, SetNumberOfSamples, setNumberOfSamples)
{
   samplerNSamples = (int)params[0];
   
   DEBUGMACRO("Set number of samples to " << samplerNSamples);

   result.setSize(0);
}
RPC_X(s, SetNumberOfSamples); 

RPC_METHOD_NAME(s, GetNumberOfSamples, getNumberOfSamples)
{
   result = (int)samplerNSamples;
}
RPC_X(s, GetNumberOfSamples); 

RPC_METHOD_NAME(s, SetFragmentNoise, setFragmentNoise)
{
   const double noise = params[0];
   aligner->setFragmentNoise(noise);
   
   DEBUGMACRO("Set fragment noise to " << aligner->getFragmentNoise());

   result.setSize(0);
}
RPC_X(s, SetFragmentNoise); 

RPC_METHOD_NAME(s, GetFragmentNoise, getFragmentNoise)
{
   result = aligner->getFragmentNoise();
}
RPC_X(s, GetFragmentNoise); 

RPC_METHOD_NAME(s, SetTrimFactor, setTrimFactor)
{
   aligner->setTrimFactor(params[0]);
   result.setSize(0);
}
RPC_X(s, SetTrimFactor); 

RPC_METHOD_NAME(s, GetTrimFactor, getTrimFactor)
{
   result = aligner->getTrimFactor();
}
RPC_X(s, GetTrimFactor);

RPC_METHOD_NAME(s, SetModel, setModel)
{
   if (*aligner == 0)
      throw XmlRpcException("Must specify the cost function first!");

   if (*samplerFactory == 0)
      throw XmlRpcException("Must specify the sampler type first!");

   if (samplerNSamples == INVALID_INDEX)
      throw XmlRpcException("Must specify the number of samples first!");

   const std::string filename = params[0];

   DEBUGMACRO("Loading model from " << filename);

   ShapeModel::P model = MeshIO::loadModel(std::ifstream(filename.c_str()));
   model->activateAllModes();

   Vector weights;
   if (params.size() == 1)
   {
      DEBUGMACRO("No weights specified; assuming all modes active");
      weights.set_size(model->getNumberOfModes());
      // Default values
      weights.fill(0);
      if (weights.size() > 0) weights[0] = 1;
   }
   else if (params.size() >= 2)
   {
      params.write(std::cerr);
      std::cerr << std::endl;

      XmlRpcValue xweights = params[1];
      PRINTVBL(xweights.getType());

      if (params.size() == 2)
      {
         model->setNumberOfModes(xweights.size());
         weights.set_size(xweights.size());
      }
      else
      {
         DEBUGMACRO("Unspecified weights set to zero");
         weights.set_size(model->getNumberOfModes());
      }

      // Default values
      weights.fill(0);
      if (weights.size() > 0) weights[0] = 1;

      // Read the weights
      for (Index i = 0; i < (Index)xweights.size(); ++i)
      {
         switch (xweights[i].getType())
         {
         case XmlRpcValue::TypeInt:
            weights[i] = (double)((int)xweights[i]);
            break;
         default:
            weights[i] = (double)xweights[i];
         }
      }

      DEBUGMACRO("Weights set to " << weights);
   }

   aligner->setModel(model, samplerFactory, samplerNSamples, weights);
   DEBUGMACRO("Model set.");

   result.setSize(0);
}
RPC_X(s, SetModel); 

RPC_METHOD_NAME(s, GetModelWeights, getModelWeights)
{
   copyVectorIntoXmlRpcValue(aligner->getModelParameters(), result);
}
RPC_X(s, GetModelWeights);

RPC_METHOD_NAME(s, SetModelWeights, setModelWeights)
{
   Vector vec = aligner->getModelParameters();
   copyXmlRpcValueIntoVector(params[0], vec, vec.size());
   aligner->setModelParameters(vec);

   result.setSize(0);
}
RPC_X(s, SetModelWeights);

RPC_METHOD_NAME(s, AddFragment, addFragment)
{
   if (*aligner == 0)
      throw XmlRpcException("Must specify the cost function first!");

   if (*samplerFactory == 0)
      throw XmlRpcException("Must specify the sampler type first!");

   if (samplerNSamples == INVALID_INDEX)
      throw XmlRpcException("Must specify the number of samples first!");

   const std::string filename = params[0];

   Mesh::P fragMesh = getFromMeshStore(filename);
   
   Vector transform(7);
   if (params.size() == 1)
   {
      DEBUGMACRO("No transform specified; assuming identity");
      transform.fill(0);
      transform[0] = 1;
   }
   else if (params.size() >= 2)
   {
      copyXmlRpcValueIntoVector(params[1], transform, 7);
   }

   aligner->addFragment(fragMesh, samplerFactory->createInstance(fragMesh), samplerNSamples, transform);
   DEBUGMACRO("Fragment added.");

   result.setSize(0);
}
RPC_X(s, AddFragment); 

RPC_METHOD_NAME(s, GetNumberOfFragments, getNumberOfFragments)
{
   result = (int)aligner->getNumberOfFragments();
}
RPC_X(s, GetNumberOfFragments);

RPC_METHOD_NAME(s, getFragmentPose, getFragmentPose)
{
   const Index i = (int)params[0];
   copyVectorIntoXmlRpcValue(aligner->getFragmentPose(i), result);
}
RPC_X(s, getFragmentPose);

RPC_METHOD_NAME(s, setFragmentPose, setFragmentPose)
{
   const Index i = (int)params[0];

   Vector transform(7);
   copyXmlRpcValueIntoVector(params[1], transform, 7);

   aligner->setFragmentPose(i, transform);

   result.setSize(0);
}
RPC_X(s, setFragmentPose);

RPC_METHOD_NAME(s, getLameckerMeasures, getLameckerMeasures)
{
   LameckerMeasures lm = aligner->getLameckerMeasures();

   result["mean"] = lm.mean;
   result["rms"]  = lm.rms;
   result["max"]  = lm.max;
   result["aod"]  = lm.aod;

   result["modelScale"] = lm.modelScale;
}
RPC_X(s, getLameckerMeasures);

RPC_METHOD_NAME(s, getTargetRegistrationError, getTargetRegistrationError)
{
   const Index i = (int)params[0];
   const Index j = (int)params[1];

   const Vector tre = aligner->getTargetRegistrationError(i, j);

   copyVectorIntoXmlRpcValue(tre, result);
}
RPC_X(s, getTargetRegistrationError);

RPC_METHOD_NAME(s, resample, resample)
{
   aligner->resample();

   result.setSize(0);
}
RPC_X(s, resample);

RPC_METHOD_NAME(s, writeModels, writeModels)
{
   const std::string saveFormat = params[0];

   aligner->writeModels(saveFormat);

   result.setSize(0);
}
RPC_X(s, writeModels);

RPC_METHOD_NAME(s, findCorrespondences, findCorrespondences)
{
   result = (int)aligner->findCorrespondences();
}
RPC_X(s, findCorrespondences);

RPC_METHOD_NAME(s, stepUpdate, stepUpdate)
{
   result = aligner->stepUpdate();
}
RPC_X(s, stepUpdate);

RPC_METHOD_NAME(s, setNumberOfModes, setNumberOfModes)
{
   int modes = params[0];
   if (modes < 0)
      aligner->activateAllModes();
   else
      aligner->setNumberOfModes(modes);

   result = (int)aligner->getNumberOfModes();
}
RPC_X(s, setNumberOfModes);

RPC_METHOD_NAME(s, meshInfo, meshInfo)
{
   Mesh::P mesh = getFromMeshStore(params[0]);

   result["surfaceArea"] = calculateSurfaceArea(mesh);
   
   Point centroid = calculateCentroid(mesh);

   copyVectorIntoXmlRpcValue(centroid, result["centroid"]);
}
RPC_X(s, meshInfo);

RPC_METHOD_NAME(s, poseToTransform, poseToTransform)
{
   Vector pose(7);
   copyXmlRpcValueIntoVector(params[0], pose, 7);

   Transformation trans;
   calculateTransformation(pose, trans);

   for (Index r = 0; r < 4; ++r)
   {
      result[r] = XmlRpcValue();

      for (Index c = 0; c < 4; ++c)
         result[r][c] = trans(r,c);
   }
}
RPC_X(s, poseToTransform);

#include "vnl/algo/vnl_svd.h"
RPC_METHOD_NAME(s, relativeRotation, relativeRotation)
{
   // Need to get the two transformation matrices
   Index fragIndexes[] = {
      (int)params[0], (int)params[1]
   };

   Vector fragPoses[] = {
      aligner->getFragmentPose(fragIndexes[0]),
      aligner->getFragmentPose(fragIndexes[1]),
   };

   Transformation trans[2];
   calculateTransformation(fragPoses[0], trans[0]);
   calculateTransformation(fragPoses[1], trans[1]);

   const Transformation relative = vnl_svd<double>(trans[0]).inverse() * trans[1];

   // Now, calculate the trace of relative
   const double trace = relative(0,0) + relative(1,1) + relative(2,2);

   // Extract the rotation
   const double rot = acos(std::min<double>(std::max<double>((trace - 1) / 2, -1), 1));

   result = rot;
}
RPC_X(s, relativeRotation);

RPC_METHOD_NAME(s, centroidPosition, centroidPosition)
{
   params.write(std::cerr);
   BLANKLINE;

   const std::string filename = params[0];
   Mesh::P mesh = getFromMeshStore(filename);
   Point centroid = calculateCentroid(mesh);

   PRINTVBL(filename);

   Vector transform(7);
   copyXmlRpcValueIntoVector(params[1], transform, 7);

   Transformation trans;
   calculateTransformation(transform, trans);

   Point transCentroid = trans * centroid;

   copyVectorIntoXmlRpcValue(transCentroid, result);
}
RPC_X(s, centroidPosition);

RPC_METHOD_NAME(s, pickDistraction, pickDistraction)
{
   PRINTVBL(params.size());
   XmlRpcValue filenames = params[0];
   XmlRpcValue trans     = params[1];
   XmlRpcValue rots      = params[2];

   Point centroid[2];

   for (Index i = 0; i < 2; ++i)
   {
      const std::string filename = filenames[i];
      Mesh::P mesh = getFromMeshStore(filename);
      centroid[i] = calculateCentroid(mesh);
   }

   const Point centre = (centroid[0] + centroid[1]) * 0.5;
   const Point axis   = (centroid[0] - centroid[1]).normalize();

   Point dist;
   const double transSigma = trans[0];
   dist(0) = transSigma/2;
   dist(1) = trans[1];
   dist(2) = trans[2];

   Transformation Ttrans; Ttrans.set_identity();
   Ttrans(0,3) = (double)trans[0] / 2.0;
   Ttrans(1,3) = (double)trans[1] / 2.0;
   Ttrans(2,3) = (double)trans[2] / 2.0;

   // Construct transformations in the distraction coordinate space
   Transformation R[2];
   for (Index i = 0; i < 2; ++i)
   {
      const double a = rots[i][0];
      const double b = rots[i][1];
      const double c = rots[i][2];

      R[i](0,0) = cos(b)*cos(c);                        R[i](0,1) = -cos(b)*sin(c);                       R[i](0,2) = sin(b);
      R[i](1,0) = cos(a)*sin(c) + cos(c)*sin(a)*sin(b); R[i](1,1) = cos(a)*cos(c) - sin(a)*sin(b)*sin(c); R[i](1,2) = -cos(b)*sin(a);
      R[i](2,0) = sin(a)*sin(c) - cos(a)*cos(c)*sin(b); R[i](2,1) = cos(c)*sin(a) + cos(a)*sin(b)*sin(c); R[i](2,2) = cos(a)*cos(b);

      R[i].set_column(3, ((i==1) ? -1.0 : 1.0) * dist);
      
      R[i].set_row(3,0.0);
      R[i](3,3) = 1;

      if (i == 0)
         R[i] = Ttrans * R[i];
      else
         R[i] = vnl_svd<double>(Ttrans).inverse() * R[i];
      //PRINTVBL(R[i]);
   }

   //PRINTVBL(axis);

   Transformation T;
   T.set_column(0, axis);

   while (true)
   {
      //PRINTVBL(T);
      for (Index i = 0; i < 3; ++i)
         T(i,2) = 1.0; //AWT::Noise<double>::randu(-1, 1);
      T(3,2) = 0;

      T.set_column(2, T.get_column(2).normalize());

      Point crossP;

      AWT::cross<double>(T.get_column(2).data_block(), T.get_column(0).data_block(), crossP.data_block());
      crossP(3) = 0;

      if (crossP.squared_magnitude() != 0)
      {
         T.set_column(1,crossP);
         break;
      }
   }
   T.set_column(1, T.get_column(1).normalize());
   {
      Point crossP;
      AWT::cross<double>(T.get_column(0).data_block(), T.get_column(1).data_block(), crossP.data_block());
      crossP(3) = 0;
      T.set_column(2,crossP);
   }
   T.set_row(3, 0.0);

   T.set_column(3, 0.0);
   T(3,3) = 1;

   result.setSize(2);

   for (Index i = 0; i < 2; ++i)
   {
      Transformation Tcentre; Tcentre.set_identity();
      Tcentre.set_column(3, centroid[i]);

      R[i] = Tcentre * T * Ttrans * R[i] * vnl_svd<double>(Tcentre * T).inverse();
      R[i].set_row(3, 0.0);
      R[i](3,3) = 1;

      Vector pose = matrixToPose(R[i]);
      Transformation trans = poseToMatrix(pose);

      copyVectorIntoXmlRpcValue(pose, result[i]);

      PRINTVBL(R[i]*centroid[i]);
   }
}
RPC_X(s, pickDistraction);

////////////////////////////////////////////////////////////////////////////////

RPC_METHOD_NAME(s, Cd, cd)
{
   const std::string dir = params[0];

   DEBUGMACRO("Changing directory to " << dir);
   SetCurrentDirectory(dir.c_str());

}
RPC_X(s, Cd); 

RPC_METHOD_NAME(s, Exit, exit)
{
   DEBUGMACRO("Exit received");
   s.exit();
}
RPC_X(s, Exit); 

RPC_METHOD_NAME(s, Report, report)
{
   DEBUGMACRO("Report request received");
   AWT::ReferenceCountedObject::report();
}
RPC_X(s, Report); 

int main(int argc, char** argv)
{
   int port = atoi(argv[1]);

   if (port < 1024 || port >= 65536)
   {
      std::cerr << "Port number out of range." << std::endl;
      exit(1);
   }

   // Create the server socket on the specified port
   XmlRpcValue::setDoubleFormat("%.8g");
   s.bindAndListen(port);

   std::cerr << "XMLRPC Server listening on port " << port << "." << std::endl;

   // Wait for requests and process indefinitely (Ctrl-C to exit)
   s.work(-1.0);

   return 0;
}