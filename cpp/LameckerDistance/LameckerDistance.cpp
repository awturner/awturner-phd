#include "Mesh/Mesh.h"
#include "Mesh/MeshSimilarity.h"
#include "Mesh/VTKMeshLoader.h"
#include "Useful/ProfTimer.h"

using namespace AWT;

typedef double T;

int main( int argc, char** argv )
{
   Mesh<T>::P meshA = VTKMeshLoader<T>::load( argv[1] );
   Mesh<T>::P meshB = VTKMeshLoader<T>::load( argv[2] );

   const T thresh = atof( argv[3] );
   const unsigned int subdivs = atoi( argv[4] );
   const unsigned int incorr  = atoi( argv[5] );

   MeshSimilarity<T>::P meshSim = MeshSimilarity<T>::getInstance( meshA, meshB, thresh, subdivs, incorr != 0 );

   std::cout << "similarity = {" << std::endl;

   std::cout << "\t" << "meshes = {" << std::endl;
   std::cout << "\t\t" << "\"" << argv[1] << "\"," << std::endl;
   std::cout << "\t\t" << "\"" << argv[2] << "\"" << std::endl;
   std::cout << "\t" << "}," << std::endl;

   std::cout << "\t" << "mean = " << meshSim->getMeanDistance() << "," << std::endl;
   std::cout << "\t" << "rms  = " << meshSim->getRmsDistance() << "," << std::endl;
   std::cout << "\t" << "max  = " << meshSim->getMaximumDistance() << "," << std::endl;
   std::cout << "\t" << "aod  = " << meshSim->getAreaOfDeviation() << "," << std::endl;

   std::cout << "\t" << "aodthresh  = " << meshSim->getAreaOfDeviationThreshold() << "," << std::endl;
   std::cout << "\t" << "subdivs    = " << meshSim->getNumberOfSubdivisions() << std::endl;

   std::cout << "}" << std::endl;
   
   /*
   for ( unsigned int subdivs = 0; subdivs < 15; ++subdivs )
   {
      ProfilingTimer timer;
      DEBUGMACRO( subdivs );

      timer.start();
      MeshSimilarity<T>::P meshSim = MeshSimilarity<T>::getInstance( meshA, meshB, thresh, subdivs, incorr != 0 );
      timer.stop();

      std::cout << subdivs << "\t" << meshSim->getMeanDistance() << "\t" << meshSim->getRmsDistance() << "\t" << meshSim->getMaximumDistance() << "\t" << meshSim->getAreaOfDeviation() << "\t" << meshSim->getAreaOfDeviationThreshold() << "\t" << timer.getDurationInSecs() << std::endl;
   }
   */
}