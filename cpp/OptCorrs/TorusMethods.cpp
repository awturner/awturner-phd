#include "TorusMethods.h"

#include "CommonMethods.h"

#include "AlignParametric/SampledTorusOptimizer.h"
#include "AlignParametric/DeterminantOfCovariance.h"
#include "AlignParametric/MinimumDescriptionLength.h"
#include "Mesh/VTKMeshLoader.h"

using namespace AWT;
using namespace AWT::AlignParametric;
using namespace XmlRpc;

void remoteMethod( AWT::AlignParametric::Torus::initialize )
{
   // Clear any existing state
   CommonMethods::reset( );

   if ( params.size() != 2 )
      throw XmlRpcException( "Need exactly 2 arguments" );

DEBUGLINE;
   const std::string smFilename = params[0];
DEBUGLINE;
   const int         kernels    = params[1];

   DEBUGMACRO( "Creating an optimizer" );
   SampledTorusOptimizer::P sto = SampledTorusOptimizer::getInstance( );
DEBUGLINE;

   // Set up the sampling mesh
DEBUGLINE;
   
   MeshType::P samplingMesh = VTKMeshLoader<T>::load( smFilename.c_str(), false );
DEBUGLINE;

   if ( !samplingMesh->hasTextureCoords())
      throw XmlRpcException( "Sampling mesh must have texture coordinates" );
DEBUGLINE;

   sto->setSamplingMesh( samplingMesh );
DEBUGLINE;

   DEBUGMACRO( "Loaded sampling mesh from " << smFilename );
   DEBUGMACRO( "\tnv = " << samplingMesh->getNumberOfVertices() );
   DEBUGMACRO( "\tnf = " << samplingMesh->getNumberOfFaces() );
DEBUGLINE;

   // Set up the initial number of kernels
   sto->setNumberOfKernels( kernels );
DEBUGLINE;

   // Store in the general pointer
   eigOpt = sto;
DEBUGLINE;

   DEBUGLINE;
}

void remoteMethod( AWT::AlignParametric::Torus::addMesh )
{
   const std::string filename = params[0];

   DEBUGMACRO( "Adding surface from " << filename );

   MeshType::P modelMesh = VTKMeshLoader<T>::load( filename.c_str(), false );
   if ( !modelMesh->hasTextureCoords() )
      throw XmlRpcException( "Mesh does not have texture coordinates!" );

   SampledTorusOptimizer* sto = dynamic_cast<SampledTorusOptimizer*>( *eigOpt );

   DEBUGMACRO( "Adding to optimizer" );
   SampledSurface::P surf = sto->addMesh( modelMesh );
   sampledSurfaces.push_back( surf );
   
   DEBUGMACRO( "Done." );
}


void remoteMethod( AWT::AlignParametric::Torus::refine )
{
   for ( Idx i = 0; i < sampledSurfaces.size(); ++i )
      sampledSurfaces[i]->refine();
}


void AWT::AlignParametric::Torus::addMethods( AWT::RemoteMethods& methods )
{
   methods.push_back( RemoteMethod( "torus.initialize", initialize ) );
   methods.push_back( RemoteMethod( "torus.addMesh", addMesh ) );
   methods.push_back( RemoteMethod( "torus.refine", refine ) );
}