#include "ModelObject.h"

#include "MeshFunctions.h"

using namespace AWT::SimpleMesh;

struct AWT::SimpleMesh::ModelObject::D
{
   SamplerFactory::P fact;
   ShapeModel::P model;
};

AWT::SimpleMesh::ModelObject::ModelObject( ShapeModel::P model, SamplerFactory::P fact, Index nsamples )
: AlignObject( nsamples )
{
   m_D = new D;

   m_D->model = model;
   m_D->fact = fact;

   // Initialize the parameters
   params.set_size( model->getNumberOfModes() );
   if ( params.size() > 0 )
   {
      params.fill( 0 );
      params(0) = 1;
   }

   setParameters( params );
}

AWT::SimpleMesh::ModelObject::~ModelObject( )
{
   delete m_D;
}

AWT::SimpleMesh::ModelObject::P AWT::SimpleMesh::ModelObject::getInstance( ShapeModel::P model, SamplerFactory::P fact, Index nsamples )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::ModelObject, ( model, fact, nsamples ) );
}

GETNAMEMACRO( AWT::SimpleMesh::ModelObject );

ShapeModel::P AWT::SimpleMesh::ModelObject::getModel( )
{
   return m_D->model;
}

void AWT::SimpleMesh::ModelObject::setParameters( const Vector& params )
{
   AlignObject::setParameters( params );
   
   mesh    = m_D->model->getMesh( this->params );

   sampler = m_D->fact->createInstance( mesh );

   // Invalidate the searcher so that it has to be recreated
   searcher.nullify( );

   // Update the existing samples for the change in mesh shape
   for ( PIWs::iterator it = samples.begin(), en = samples.end(); it != en; ++it )
      it->p = calculatePoint( mesh, it->i, it->w );
}

PointIndexWeights AWT::SimpleMesh::ModelObject::search( const Point& pnt, const Point& nml, SearchFilter::P filter )
{
   // If the searcher hasn't been created yet, make sure that it is
   if ( *searcher == 0 )
   {
      DEBUGMACRO( "Creating a new searcher" );
      searcher = Search::getInstance( mesh, true );
   }

   // No need to transform before searching
   PointIndexWeights ret = searcher->search( pnt, nml, filter );
   return ret;
}

void AWT::SimpleMesh::ModelObject::calculateJacobian( const PointIndexWeights& piw, Matrix& jac )
{
   // Delegate calculation to the shape model
   m_D->model->jacobian( piw.i, piw.w, jac );
   //jac.fill( 0. );
}

void AWT::SimpleMesh::ModelObject::activateAllModes( )
{
   m_D->model->activateAllModes( );
   setNumberOfModes( m_D->model->getNumberOfModes() );
}

void AWT::SimpleMesh::ModelObject::setNumberOfModes( const AWT::SimpleMesh::Index ii )
{
   m_D->model->setNumberOfModes( ii );

   // The actual number of modes
   const Index i = m_D->model->getNumberOfModes();

   PRINTVBL( ii );
   PRINTVBL( i );
   PRINTVBL( params.size() );

   Vector newWeights(i); 
   newWeights.fill(0);

   Index j = 0;

   while ( j < i && j < params.size() )
   {
      newWeights[j] = params[j];
      ++j;
   }

   while ( j < i )
   {
      newWeights[j] = 0;
      ++j;
   }

   if ( params.size() == 0 )
      newWeights[0] = 1;

   params.set_size(i);
   params = newWeights;

   PRINTVBL( params );
}

AWT::SimpleMesh::Index AWT::SimpleMesh::ModelObject::getNumberOfModes( ) const
{
   return params.size( );
}