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
#include "Mesh/MeshImpl.h"
#include "Mesh/TuplesImpl.h"
#include "Mesh/TuplesFunctions.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"

#include "Useful/PrintMacros.h"
#include "Useful/Noise.h"
#include <iomanip>

#include <vector>
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_svd.h"

typedef double T;
using namespace AWT;

typedef AWT::Mesh<T> MeshType;
typedef std::vector<MeshType::P> MeshList;

typedef vnl_matrix<T> MatrixType;
typedef vnl_vector<T> VectorType;

template <class T>
class MeanCalculator
{
public:
   MeanCalculator()
   {
      reset();
   }

   void reset()
   {
      sum_x = sum_xx = 0;
      count = 0;
   }

   void add(const T v)
   {
      sum_x += v;
      sum_xx += v*v;

      ++count;
   }

   T sampleMean() const { return sum_x / (count-1); }

   T sampleVar() const { return (sum_xx - sum_x*sum_x/count) / (count-1); }

   T sampleStd() const { return sqrt(sampleVar); }

   T populationMean() const { return sum_x / count; }

   T populationVar() const { return (sum_xx - sum_x*sum_x/count) / count; }

   T populationStd() const { return sqrt(sampleVar); }

protected:
   T sum_x;
   T sum_xx;
   unsigned int count;
};

struct ShapeModel
{
   MatrixType P;

   Tuples<MeshIndex>::P faces;

   VectorType mu;
   MatrixType U;
   VectorType eig;
};

VectorType tuplesToVector(Tuples<T>::P tuples)
{
   VectorType ret(3*tuples->getNumberOfPoints());

   T vtx[3];
   for (MeshIndex v = 0; v < tuples->getNumberOfPoints(); ++v)
   {
      tuples->getPoint(v, vtx);

      FOREACHAXIS(ax)
         ret(3*v+ax) = vtx[ax];
   }

   return ret;
}

void vectorToTuples(const VectorType& vec, Tuples<T>::P tuples)
{
   const unsigned int N = vec.size() / 3;
   T vtx[3];
   for (MeshIndex v = 0; v < N; ++v)
   {
      FOREACHAXIS(ax)
         vtx[ax] = vec(3*v+ax);

      tuples->setPoint(v, vtx);
   }
}

void vectorToMesh(const VectorType& vec, MeshType::P mesh)
{
   vectorToTuples(vec, mesh->getVertices());
}

MatrixType calculateY(const ShapeModel& model)
{
   const unsigned int M = model.P.cols();

   MatrixType ret = model.P;
   for (unsigned int i = 0; i < M; ++i)
      ret.set_column(i, ret.get_column(i) - model.mu);

   return ret;
}

T eigenvalueScale(const ShapeModel& model)
{
   return model.mu.squared_magnitude() * 3 / model.mu.size();
}

ShapeModel construct(const MatrixType& P, Tuples<MeshIndex>::P faces)
{
   ShapeModel ret;

   const unsigned int M = P.cols();

   // Keep P with the model
   ret.P = P;

   // Keep faces with the model
   ret.faces = TuplesImpl<MeshIndex>::getInstance(faces);

   // Calculate the mean shape
   ret.mu.set_size(P.rows());
   ret.mu.fill(0);
   for (unsigned int i = 0; i < M; ++i)
   {
      ret.mu += P.get_column(i);
   }
   ret.mu /= M;

   // Convert to P zero-mean-shifted matrix
   const MatrixType Y = calculateY(ret);

   // Now calculate the modes
   vnl_svd<T> svd(Y / sqrt(static_cast<T>(M)));

   // Scale the modes so that the eigenvalues are implicit
   ret.U = svd.U() * svd.W().asMatrix().get_n_columns(0, M-1);

   ret.eig.set_size(M - 1);
   for (unsigned int i = 0; i < (M-1); ++i)
      ret.eig(i) = svd.W(i, i);

   return ret;
}

ShapeModel construct(const std::vector<MeshType::P>& meshes)
{
   const unsigned int N = meshes[0]->getNumberOfVertices();
   const unsigned int M = meshes.size();

   MatrixType P(N*3, M);

   for (unsigned int i = 0; i < M; ++i)
   {
      // Convert the i-th shape to the i-th column
      P.set_column(i, tuplesToVector(meshes[i]->getVertices()));
   }

   return construct(P, meshes[0]->getFaces());
}

int sum(int argc, char** argv)
{
   MeshType::P base = VTKMeshLoader<T>::load(argv[1], false);
   MeshType::P add  = VTKMeshLoader<T>::load(argv[2], false);
   T w              = atof(argv[3]);

   T vtx[3];
   T vtxAdd[3];
   MESH_EACHVERTEX(base, v)
   {
      base->getVertex(v, vtx);
      add->getVertex(v, vtxAdd);

      FOREACHAXIS(ax)
         vtx[ax] += w*vtxAdd[ax];

      base->setVertex(v, vtx);
   }

   VTKMeshWriter<T>::write(base, argv[4]);

   return 0;
}

ShapeModel newmodel(const std::vector<std::string>& filenames)
{
   ShapeModel ret;

   std::vector< MeshType::P > meshes;

   for (unsigned int i = 0; i < filenames.size(); ++i)
   {
      MeshType::P mesh = VTKMeshLoader<T>::load(filenames[i].c_str(), false);

      if (!meshes.empty())
      {
         if (mesh->getNumberOfVertices() != meshes[0]->getNumberOfVertices())
         {
            PRINTVBL(mesh->getNumberOfVertices());
            PRINTVBL(meshes[0]->getNumberOfVertices());
            std::cerr << "Wrong number of vertices in " << filenames[i] << "; exiting." << std::endl;
            exit(1);
         }
      }

      meshes.push_back(mesh);

      std::cerr << "Loaded mesh from " << filenames[i] << "." << std::endl;
   }

   if (!meshes.empty())
   {
      return construct(meshes);
   }
   else
   {
      return ShapeModel();
   }
}

ShapeModel getModesByCount(const ShapeModel& model, const unsigned int n)
{
   ShapeModel ret(model);

   ret.U   = ret.U.get_n_columns(0, n);
   ret.eig = ret.eig.extract(0, n);

   return ret;
}

ShapeModel getModesByVarianceFraction(const ShapeModel& model, const T fraction)
{
   if (fraction < 0 || fraction > 1)
   {
      std::cerr << "Variance fraction must be between 0 and 1; exiting." << std::endl;
      exit(1);
   }

   // Get the eigenvalues
   VectorType eigs = model.eig;

   // Calculate the cumulative sum of the eigenvalues
   for (unsigned int i = 1; i < eigs.size(); ++i)
      eigs(i) += eigs(i-1);

   // Normalize by total
   eigs /= eigs(eigs.size()-1);

   unsigned int r;
   for (r = 0; r < eigs.size(); ++r)
   {
      if (eigs(r) >= fraction)
         break;
   }

   if (r != eigs.size()) ++r;

   return getModesByCount(model, r);
}

void leaveAllIn(const ShapeModel& model)
{
   const unsigned int N = model.U.rows() / 3;
   const unsigned int M = model.P.cols();
   const unsigned int R = model.U.cols();

   // Each column is the fitting parameters for that shape
   const MatrixType Y = calculateY(model);
   const MatrixType bb = vnl_svd<T>(model.U).pinverse()*Y;

   DEBUGMACRO("Leave-all-in");
   std::cerr << "# Modes" << "\t" << "Mean" << "\t" << "Std" << std::endl;

   // Calculate the RMS distance as successive modes are excluded
   for (unsigned int m = 0; m < R; ++m)
   {
      VectorType stats(M);

      for (unsigned int s = 0; s < M; ++s)
      {
         VectorType b = bb.get_column(s);

         // Discard the higher modes
         for (unsigned int mm = m+1; mm < R; ++mm)
            b(mm) = 0;

         VectorType y = model.U*b;

         stats(s) = (y - Y.get_column(s)).rms();
      }

      std::cerr << (m+1) << "\t" << stats.mean() << "\t" << (stats - stats.mean()).rms() << std::endl;
   }
}

ShapeModel constructLeaveOneOutModel(const ShapeModel& model, const unsigned int shapeToExclude)
{
   MatrixType P(model.P.rows(), model.P.cols()-1);

   P.set_columns(0,              model.P.get_n_columns(0,shapeToExclude));
   P.set_columns(shapeToExclude, model.P.get_n_columns(shapeToExclude+1, model.P.cols()-1-shapeToExclude));

   return construct(P, model.faces);
}

void alignMeshToModel(const ShapeModel& leftOutModel, Mesh<T>::P mesh, vnl_matrix_fixed<T,4,4>& transformation)
{

}

void alignKnownCorrespondencesToModel(const ShapeModel& leftOutModel, const unsigned int m, VectorType& x, VectorType& xest)
{
   const unsigned int N = leftOutModel.U.rows() / 3;
   const unsigned int R = leftOutModel.U.cols();

   vnl_svd<T> solver(leftOutModel.U);

   VectorType b;

   Tuples<T>::P tuplesX = TuplesImpl<T>::getInstance(3, N);
   Tuples<T>::P tuplesY = TuplesImpl<T>::getInstance(3, N);

   T lastRms = std::numeric_limits<T>::infinity();

   // Need to optimize the pose
   while (true)
   {
      // Calculate the difference from the mean shape
      VectorType y = x - leftOutModel.mu;

      // Solve for the best values of b
      b = solver.solve(y);

      // Implicitly discard these modes
      for (unsigned int r = m+1; r < R; ++r)
         b(r) = 0;

      // This is the shape estimated from the model
      xest = leftOutModel.mu + leftOutModel.U * b;

      const T rms = (x - xest).squared_magnitude() / N;
      const T grad = lastRms - rms;
      if (abs(grad) < 1e-4)
         break;

      lastRms = rms;

      vnl_matrix_fixed<T,4,4> trans = TuplesFunctions<T>::absoluteOrientation(x, xest, true);

      TuplesFunctions<T>::transformPoints(x, trans);
   }
}

void leaveOneOut(const ShapeModel& model)
{
   const unsigned int N = model.U.rows() / 3;
   const unsigned int M = model.P.cols();
   const unsigned int R = model.U.cols();

   VectorType sum_x2(R-1); sum_x2.fill(0);
   VectorType sum_x(R-1);  sum_x.fill(0);

   for (unsigned int s = 0; s < M; ++s)
   {
      ShapeModel leftOutModel = constructLeaveOneOutModel(model, s);

      for (unsigned int m = 0; m < R-1; ++m)
      {
         VectorType x = model.P.get_column(s);
         VectorType xest(x.size());

         alignKnownCorrespondencesToModel(leftOutModel, m, x, xest);
         
         const T rms = sqrt((x - xest).squared_magnitude() / N);

         sum_x2(m) += rms*rms;
         sum_x(m)  += rms;
      }
   }

   DEBUGMACRO("Leave-one-out");
   std::cerr << "# Modes" << "\t" << "Mean" << "\t" << "Std" << std::endl;

   for (unsigned int r = 0; r < R-1; ++r)
   {
      const T mean = sum_x(r) / M;
      const T std = sqrt((sum_x2(r) - sum_x(r)*sum_x(r)/M) / M);

      std::cerr << (r+1) << "\t" << mean << "\t" << std << std::endl;
   }

   //PRINTVBLNL(leaveOutData);
}

void specificityGeneralization(const ShapeModel& model, const unsigned int NR)
{
   const unsigned int N = model.U.rows() / 3;
   const unsigned int M = model.P.cols();

   VectorType genMins(M);
   genMins.fill(std::numeric_limits<T>::infinity());

   T specificitySum  = 0;
   T specificity2Sum = 0;

   const T scale = sqrt(eigenvalueScale(model));
   PRINTVBL(scale);

   for (unsigned int i = 0; i < NR; ++i)
   {
      // Pick a random model weight vector
      VectorType b(model.U.cols());
      b.fill(0);

      for (unsigned int bi = 0; bi < 2/*model.U.cols()*/; ++bi)
      {
         do
         {
            b(bi) = Noise<T>::randn();
         }
         while (b(bi) != b(bi) || abs(b(bi)) == std::numeric_limits<T>::infinity());
      }

      VectorType x = model.mu + model.U * b;

      T specMin = std::numeric_limits<T>::infinity();

      for (unsigned int s = 0; s < M; ++s)
      {
         // Calculate the distance to this shape

         const T rms = sqrt((x - model.P.get_column(s)).squared_magnitude() / N) / scale;

         specMin    = std::min<T>(specMin, rms);
         genMins(s) = std::min<T>(genMins(s), rms);
      }

      specificitySum  += specMin;
      specificity2Sum += specMin*specMin;

      if (i % 1000 == 0)
      {
         const T specNow = (specificitySum / (i+1));
         const T specErr = sqrt((specificity2Sum - specificitySum*specificitySum / (i+1)) / (i+1));

         T genSum = 0;
         T gen2Sum = 0;
         for (unsigned int s = 0; s < M; ++s)
         {
            genSum  += genMins(s);
            gen2Sum += genMins(s) * genMins(s);
         }

         const T genNow = genSum / M;
         const T genErr = sqrt((gen2Sum - genSum*genSum / M) / M);

         DEBUGMACRO(i << "\t" << "S=" << specNow << "+/-" << specErr << "\t" << "G=" << genNow << "+/-" << genErr);
      }
   }

   // Now we've gone over all the random shapes, work out that specificity!
   const T specificity = specificitySum / NR;
   const T generalization = genMins.mean();

   PRINTVBL(specificity);
   PRINTVBL(generalization);
}

void write(const ShapeModel& model, const char* outformat)
{
   const unsigned int r = model.U.cols();

   Mesh<T>::P outMesh = MeshImpl<T>::getInstance();
   char filename[256];

   outMesh->setFaces(model.faces);

   // Write the mean mesh
   vectorToMesh(model.mu, outMesh);

   sprintf_s(filename, outformat, 0);
   VTKMeshWriter<T>::write(outMesh, filename);
   std::cerr << "\t" << "Written mean shape to " << filename << std::endl;

   // Write the modes
   vectorToMesh(model.mu, outMesh);
   for (unsigned int i = 0; i < r; ++i)
   {
      vectorToMesh(model.U.get_column(i), outMesh);

      sprintf_s(filename, outformat, i+1);
      VTKMeshWriter<T>::write(outMesh, filename);

      std::cerr << "\t" << "Written mode " << (i+1) << " to " << filename << std::endl;
   }
}

ShapeModel theModel;

int main(int argc, char** argv)
{
   Noise<T>::timeSeed();

   if (argc <= 1)
   {
      std::cerr << "Please specify a verb" << std::endl;
      exit(1);
   }

   for (int i = 1; i < argc; ++i)
   {
      if (strcmp(argv[i], "newmodel") == 0)
      {
         std::vector< std::string > filenames;

         while (++i < argc)
         {
            std::string thenext(argv[i]);

            if (thenext == "end")
               break;

            filenames.push_back(thenext);
         }

         theModel = newmodel(filenames);
      }
      else if (strcmp(argv[i], "specgen") == 0)
      {
         BLANKLINE;
         specificityGeneralization(theModel, atoi(argv[++i]));
         BLANKLINE;
      }
      else if (strcmp(argv[i], "stats") == 0)
      {
         BLANKLINE;
         specificityGeneralization(theModel, 5000);
         BLANKLINE;
         leaveAllIn(theModel);
         BLANKLINE;
         leaveOneOut(theModel);
         BLANKLINE;
      }
      else if (strcmp(argv[i], "leaveallin") == 0)
      {
         BLANKLINE;
         leaveAllIn(theModel);
         BLANKLINE;
      }
      else
      {
         std::cerr << "Unknown verb: " << argv[i] << std::endl;
         exit(1);
      }
   }
}