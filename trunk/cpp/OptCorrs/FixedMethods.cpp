#include "FixedMethods.h"

#include "CommonMethods.h"

#include "AlignParametric/FixedOptimizer.h"
#include "Mesh/VTKMeshLoader.h"

using namespace AWT;
using namespace AWT::AlignParametric;
using namespace XmlRpc;

void remoteMethod( AWT::AlignParametric::Fixed::initialize )
{
   // Clear any existing state
   CommonMethods::reset( );

   DEBUGMACRO( "Creating an \"optimizer\"" );
   eigOpt = FixedOptimizer::getInstance( );
}

void remoteMethod( AWT::AlignParametric::Fixed::addMesh )
{
   const std::string filename = params[0];

   DEBUGMACRO( "Adding surface from " << filename );

   MeshType::P modelMesh = VTKMeshLoader<T>::load( filename.c_str(), false );

   FixedOptimizer* sto = dynamic_cast<FixedOptimizer*>( *eigOpt );

   DEBUGMACRO( "Adding to optimizer" );
   SampledSurface::P surf = sto->addMesh( modelMesh );
   sampledSurfaces.push_back( surf );
   
   DEBUGMACRO( "Done." );
}

void AWT::AlignParametric::Fixed::addMethods( AWT::RemoteMethods& methods )
{
   methods.push_back( RemoteMethod( "fixed.initialize", initialize ) );
   methods.push_back( RemoteMethod( "fixed.addMesh", addMesh ) );
}