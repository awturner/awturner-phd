#include "FixedSurface.h"

#include "vnl/vnl_matlab_filewrite.h"
#include "Maths/BSpline.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/TriangleProjection.h"
#include "Mesh/TuplesFunctions.h"
#include "MeshSearching/FacesNearestPointSearch.h"
#include "MeshSearching/FacesRayIntersectionSearch.h"
#include "Useful/ProfTimer.h"
#include "Useful/WalkInASpiral.h"
#include "vnl/vnl_sparse_matrix.h"
#include "vnl/algo/vnl_svd.h"

#include <vector>
#include <algorithm>
#include <fstream>

#include "vnl/vnl_sparse_matrix_linear_system.h"
#include "vnl/algo/vnl_lsqr.h"
#include "vnl/vnl_matlab_write.h"

using namespace AWT;
using namespace AWT::AlignParametric;

struct AWT::AlignParametric::FixedSurface::D
{
   MeshType::P          mesh;
};

AWT::AlignParametric::FixedSurface::FixedSurface( MeshType::P mesh )
{
   m_D = new D;

   if ( mesh->getVertices()->getTupleSize() != 3 )
      AWTEXCEPTIONTHROW( "Mesh must be 3-dimensional!" );

   // Keep hold of the mesh
   m_D->mesh = mesh;
}

AWT::AlignParametric::FixedSurface::~FixedSurface( )
{
   delete m_D;
}

AWT::AlignParametric::FixedSurface::P AWT::AlignParametric::FixedSurface::getInstance( MeshType::P mesh )
{
   AUTOGETINSTANCE( AWT::AlignParametric::FixedSurface, ( mesh ) );
}

GETNAMEMACRO(AWT::AlignParametric::FixedSurface);

// Get the number of parameter components describing each sample
Idx AWT::AlignParametric::FixedSurface::getParameterDimensionality( ) const
{
   return 1;
}

// Get the number of parameters which control this sampling
Idx AWT::AlignParametric::FixedSurface::getNumberOfParameters( ) const
{
   return 1;
}

Idx AWT::AlignParametric::FixedSurface::getNumberOfSamples( ) const
{
   return m_D->mesh->getNumberOfVertices();
}

// Get the current sample locations
void AWT::AlignParametric::FixedSurface::getSamples( MatrixType& samples ) const
{
   samples.set_size(4, m_D->mesh->getNumberOfVertices());

   T vtx[4];
   MESH_EACHVERTEX( m_D->mesh, v )
   {
      m_D->mesh->getVertex(v,vtx);
      vtx[3] = 1;

      samples.set_column(v,vtx);
   }
}

// How far can samples be moved in order to maintain stability?
T AWT::AlignParametric::FixedSurface::getMaxMove( ) const
{
   return 0;
}

// Get the current set of control values
void AWT::AlignParametric::FixedSurface::getParameters( MatrixType& controls ) const
{
   controls.set_size(1,1);
   controls.fill(0);
}

// Update the current set of control values
void AWT::AlignParametric::FixedSurface::setParameters( MatrixType& controls )
{
   if ( controls.rows() != 1 || controls.columns() != 1 )
      AWTEXCEPTIONTHROW( "Controls matrix is wrong size!");

   // Do nothing
}

// Calculates the jacobian matrix of the el-th sample location w.r.t. the p-th parameters
void AWT::AlignParametric::FixedSurface::jacobian( const Idx el, const Idx p, MatrixType& matrix ) const
{
   matrix.set_size( 3, 2 );
   matrix.fill( 0 );
}

// Iterator functions - allows you to skip all the zero jacobians
// Takes the internal iterator back to the start
void AWT::AlignParametric::FixedSurface::resetIterator( )
{
}

// Advances to the next non-zero jacobian pair
bool AWT::AlignParametric::FixedSurface::next( Idx& el, Idx& p )
{
   el = p = INVALID_INDEX;
   
   return false;
}

void AWT::AlignParametric::FixedSurface::refine()
{
}

// Calculate the regularization cost
T AWT::AlignParametric::FixedSurface::regularizationCost( )
{
   return 0;
}

// Calculate a regularization term
void AWT::AlignParametric::FixedSurface::regularizationGradient( MatrixType& reg, const Transformation& trans )
{
   // Actually, this surface type is sort-of self-regularizing, because of the smoothness of the parametrization
   // I suppose you could build in some sort of curvature regularization, but I don't want to

   // Just make it compatible for addition, and fill with zeros
   reg.set_size( 1, 1 );
   reg.fill( 0 );
}
