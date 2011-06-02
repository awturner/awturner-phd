#if 0
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

using namespace AWT::SimpleMesh;
using namespace AWT;
using namespace std;

void readVector( int& i, char** argv, Vector& params )
{
   for ( Index m = 0; m < params.size(); ++m )
   {
      const char* modeweightStr = argv[++i];

      if ( strcmp( modeweightStr, "-" ) == 0 )
         break;

      params[m] = atof( modeweightStr );
   }

   PRINTVBL( params );
}

Aligner::P aligner;
Index maxIters;
std::string costFunction;

ShapeModel::P model;
std::string modelFilename;
std::string modelSampler;

std::vector<std::string> fragmentFilenames;
std::vector<std::string> fragmentSamplers;

bool savefinalmodels;

std::ostream& printLameckerMeasures( std::ostream& os, const AWT::SimpleMesh::LameckerMeasures& lam )
{
   os << "MEAN " << lam.mean << std::endl;
   os << "RMS " << lam.rms << std::endl;
   os << "MAX " << lam.max << std::endl;
   os << "MODELSCALE " << lam.modelScale << std::endl;

   return os;
}

std::ostream& printState( std::ostream& os )
{
   const Vector modelParams = aligner->getModelParameters();

   // Write out the state of the system in a form which is easier to interpret than the command line

   os << "STATE START" << std::endl;
   os << "MODEL \"" << modelFilename << "\" " << modelParams << std::endl;

   for ( Index i = 0; i < aligner->getNumberOfFragments(); ++i )
      os << "FRAGMENT \"" << fragmentFilenames[i] << "\" " << aligner->getFragmentPose( i ) << std::endl;

   const LameckerMeasures lam = aligner->getLameckerMeasures( );
   printLameckerMeasures( os, lam );
   os << "STATE END" << std::endl;

   return os;
}

std::ostream& printCommandLine( std::ostream& os )
{
   const Vector modelParams = aligner->getModelParameters();

   // Let's reform the command line so that the iteration can be restarted
   os << "# SimpleMeshAlign";
   os << " " << "--maxiters " << maxIters;
   os << " " << "--costfunction " << costFunction;

   os << " " << "--sampler " << modelSampler;
   os << " " << "--model \"" << modelFilename << "\"";
   os << " " << modelParams.size();
   os << " " << modelParams;

   for ( Index i = 0; i < aligner->getNumberOfFragments(); ++i )
   {
      os << " " << "--sampler " << fragmentSamplers[i];
      os << " " << "--fragment \"" << fragmentFilenames[i] << "\"";
      os << " " << aligner->getFragmentPose( i );
   }

   if ( !savefinalmodels )
      os << " " << "--nowrite";

   os << std::endl;

   return os;
}

int main( int argc, char** argv )
{
   // For a modicum of randomness
   Noise<double>::timeSeed();

   SamplerFactory::P samplerFactory;
   Index             nsamples;

   bool alwaysResample = false;
   bool alwaysPause = false;
   
   *aligner = 0;

   std::vector<Mesh::P> fragmentmeshes;
   
   std::string lastsampler;
   std::string saveformat = "%s";

   double noise = 0;

   try
   {
      for ( int i = 1; i < argc; ++i )
      {
         PRINTVBL( i );
         PRINTVBL( argv[i] );

         if ( strcmp( argv[i], "--costfunction" ) == 0 )
         {
            ++i;

            if ( strcmp( argv[i], "pointtopoint" ) == 0 )
            {
               aligner = Aligner::getInstance( Aligner::CF_POINTTOPOINT );
               DEBUGMACRO( "Using Point-to-Point cost function" );
            }
            else if ( strcmp( argv[i], "pointtoplane" ) == 0 )
            {
               aligner = Aligner::getInstance( Aligner::CF_POINTTOPLANE );
               DEBUGMACRO( "Using Point-to-Plane cost function" );
            }
            else
               throw "Unknown cost function type";

            costFunction = std::string( argv[i] );
         }
         else if ( strcmp( argv[i], "--sampler" ) == 0 )
         {
            ++i;

            std::stringstream sstr;

            if ( strcmp( argv[i], "none" ) == 0 )
            {
               samplerFactory = UniformSamplerFactory::getInstance( );
               nsamples = 0;

               DEBUGMACRO( "Using None sampler" );
               sstr << argv[i];
            }
            else
            {
               sstr << argv[i];
               if ( strcmp( argv[i], "uniform" ) == 0 )
               {
                  samplerFactory = UniformSamplerFactory::getInstance( );
                  DEBUGMACRO( "Using uniform sampler" );
               }
               else if ( strcmp( argv[i], "normalspace" ) == 0 )
               {
                  const Index latdivs = atoi( argv[++i] );
                  samplerFactory = NormalSpaceSamplerFactory::getInstance( latdivs );
                  DEBUGMACRO( "Using normal space sampler" );

                  sstr << " " << latdivs;
               }
               else
               {
                  throw "Unknown sample type";
               }

               nsamples = atoi( argv[++i] );
               DEBUGMACRO( "Using " << nsamples << " samples" );

               sstr << " " << nsamples;
            }

            lastsampler = sstr.str();

            BLANKLINE;
         }
         else if ( strcmp( argv[i], "--model" ) == 0 )
         {
            if ( *aligner == 0 )
               throw "Must specify the cost function first!";

            if ( *samplerFactory == 0 )
               throw "Must specify the sampler type first!";

            if ( nsamples == INVALID_INDEX )
               throw "Must specify the number of samples first!";

            const char* filename = argv[++i];
            DEBUGMACRO( "Loading model from " << filename );
            DEBUGLINE;
            model = MeshIO::loadModel( std::ifstream( filename ) );
            DEBUGLINE;

            DEBUGMACRO( "Model loaded from " << filename << " with " << model->getNumberOfModes() << " modes." );

            {
               const char* nmodesStr = argv[++i];
               
               if ( strcmp( nmodesStr, "-" ) == 0 )
               {
                  DEBUGMACRO( "Using default mode weights" );
                  model->activateAllModes( );
               }
               else
               {
                  DEBUGMACRO( "Using only " << nmodesStr << " modes" );
                  const Index nmodes = atoi( nmodesStr );
                  model->setNumberOfModes( nmodes );
               }
            }

            Vector params( model->getNumberOfModes() );
            params.fill( 0 );
            params[0] = 1;

            readVector( i, argv, params );

            aligner->setModel( model, samplerFactory, nsamples, params );
            
            // Reset these so that they have to be set again
            *samplerFactory = 0;
            nsamples = INVALID_INDEX;

            BLANKLINE;

            modelFilename = filename;
            modelSampler = lastsampler;
         }
         else if ( strcmp( argv[i], "--fragment" ) == 0 )
         {
            if ( *aligner == 0 )
               throw "Must specify the cost function first!";

            if ( *samplerFactory == 0 )
               throw "Must specify the sampler type first!";

            if ( nsamples == INVALID_INDEX )
               throw "Must specify the number of samples first!";

            const char* filename = argv[++i];
            PRINTVBL( filename );
            fragmentFilenames.push_back( std::string( filename ) );

            Mesh::P fragMesh = MeshIO::loadMesh( std::ifstream( filename ) );
            
            fragmentmeshes.push_back( fragMesh );

            Vector params(7);
            params.fill( 0 );
            params[0] = 1;
            readVector( i, argv, params );

            aligner->addFragment( fragMesh, samplerFactory->createInstance( fragMesh ), nsamples, params );
            
            Transformation trans = aligner->getFragmentTransformation( aligner->getNumberOfFragments()-1 );

            DEBUGMACRO( "Loaded fragment from " << filename << " with pose vector " << params );
            PRINTVBLNL( trans );

            BLANKLINE;

            fragmentSamplers.push_back( lastsampler );
         }
         else if ( strcmp( argv[i], "--noise" ) == 0 )
         {
            noise = atof( argv[++i] );
         }
         else if ( strcmp( argv[i], "--maxiters" ) == 0 )
         {
            maxIters = atoi( argv[++i] );
         }
         else if ( strcmp( argv[i], "--alwaysresample" ) == 0 )
         {
            alwaysResample = true;
         }
         else if ( strcmp( argv[i], "--alwayspause" ) == 0 )
         {
            alwaysPause = true;
         }
         else if ( strcmp( argv[i], "--saveformat" ) == 0 )
         {
            saveformat = std::string( argv[++i] );
         }
         else if ( strcmp( argv[i], "--nowrite" ) == 0 )
         {
            savefinalmodels = false;
         }
         else
         {
            DEBUGMACRO( "unknown option=" << argv[i] );
            throw "Unknown option";
         }
      }

      for ( unsigned int i = 0; i < fragmentFilenames.size(); ++i )
         PRINTVBL( fragmentFilenames[i] );

      if ( *aligner == 0 )
         throw "No cost function specified!";

      aligner->setFragmentNoise( noise );
      DEBUGMACRO( "Set noise level to " << noise );

      aligner->setSaveFormat( saveformat );

      aligner->resample( );

      // Find the correspondences and print the state beforehand
      {
         aligner->findCorrespondences( );
         printState( std::cout );
      }

      Index iters;
      for ( iters = 0; iters < maxIters; ++iters )
      {
         PRINTVBL( iters );

         const bool converged = aligner->stepUpdate( );

         printState( std::cout );

         if ( converged )
            break;

         // Resample if necessary
         if ( alwaysResample )
            aligner->resample( );

         if ( alwaysPause )
            PAUSE;

         // Re-find the correspondences for the next iteration
         aligner->findCorrespondences( );
      }

      //aligner->calculateLamecker( );

      if ( savefinalmodels )
      {
         aligner->writeModels( );
      }

      PRINTVBL2( "finaliters", iters );
   }
   catch ( const char* ex )
   {
      DEBUGMACRO( ex );
      DEBUGLINEANDEXIT( 99 );
   }

}
#endif