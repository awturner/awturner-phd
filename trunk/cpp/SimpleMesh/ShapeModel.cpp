#include "ShapeModel.h"

#include <vector>
#include "MeshFunctions.h"

struct AWT::SimpleMesh::ShapeModel::D
{
   Index nmodes;
   std::vector< Points > modes;
   Faces faces;
};

AWT::SimpleMesh::ShapeModel::ShapeModel( Mesh::P meanMesh )
{
   m_D = new D;

   m_D->nmodes = 0;

   Vector meanVec( meanMesh->nv );
   meanVec.fill( 1.0 / meanMesh->nv );

   m_D->modes.push_back( meanMesh->getVertices() );
   m_D->faces = meanMesh->getFaces();
}

AWT::SimpleMesh::ShapeModel::~ShapeModel( )
{
   delete m_D;
}

AWT::SimpleMesh::ShapeModel::P AWT::SimpleMesh::ShapeModel::getInstance( Mesh::P meanMesh )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::ShapeModel, ( meanMesh ) );
}

GETNAMEMACRO( AWT::SimpleMesh::ShapeModel );

AWT::SimpleMesh::Mesh::P AWT::SimpleMesh::ShapeModel::getMesh( const Vector& weights )
{
   Points pts;
   
   if ( weights.size() > 0 )
   {
      pts = weights[0] * m_D->modes[0];

      const Index imax = getNumberOfModes();
      for ( Index i = 1; i < imax; ++i )
      {
         pts += weights(i) * m_D->modes[i];
      }

      pts.set_row( 3, 1 );
   }
   else
   {
      pts = m_D->modes[0];
   }

   Mesh::P ret = Mesh::getInstance( pts.cols(), m_D->faces.cols() );
   ret->getVertices().update( pts );
   ret->getFaces().update( m_D->faces );
   return ret;
}

void AWT::SimpleMesh::ShapeModel::addMode( const AWT::SimpleMesh::Points& mode )
{
   if ( mode.cols() != m_D->modes[0].cols() || mode.rows() != 4 )
      throw "Mode is not compatible!";

   m_D->modes.push_back( mode );

   activateAllModes( );
}

AWT::SimpleMesh::Index AWT::SimpleMesh::ShapeModel::getNumberOfModes( ) const
{
   return m_D->nmodes;
}

void AWT::SimpleMesh::ShapeModel::jacobian( const Index v, Matrix& jac )
{
   const Index nmodes = getNumberOfModes();
   ensureSize( jac, 4, nmodes );

   for ( Index i = 0; i < nmodes; ++i )
      jac.set_column( i, m_D->modes[i][v] );

   jac.set_row( 3, 0.0 );
}

void AWT::SimpleMesh::ShapeModel::jacobian( const Index f, const ConvexWeights& ws, Matrix& jac )
{
   const Index nmodes = getNumberOfModes();
   ensureSize( jac, 4, nmodes );

   Face face = m_D->faces.get_column( f );
   Point vertices[3];
   for ( Index i = 0; i < nmodes; ++i )
   {
      for ( Index ax = 0; ax < 3; ++ax )
         vertices[ax] = m_D->modes[i].get_column( face[ax] );

      jac.set_column( i, ( 1 - ws[0] - ws[1] )*vertices[0] + ws[0]*vertices[1] + ws[1]*vertices[2] );
   }

   jac.set_row( 3, 0.0 );
}

void AWT::SimpleMesh::ShapeModel::activateAllModes( )
{
   m_D->nmodes = static_cast<Index>( m_D->modes.size() );
   modified();
}

void AWT::SimpleMesh::ShapeModel::setNumberOfModes( const Index n )
{
   if ( n > m_D->modes.size() )
      DEBUGMACRO( "Not that many modes in model!" );

   m_D->nmodes = std::min<Index>( n, m_D->modes.size() );
   modified();
}