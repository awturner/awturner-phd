#include "TreeData.h"

#include "MeshFunctions.h"

#include <algorithm>

struct AWT::SimpleMesh::TreeData::D
{
   Mesh::P simpleMesh;
   Matrix bounds;
};

AWT::SimpleMesh::TreeData::TreeData( Mesh::P mesh, const bool _useFaces )
: useFaces(_useFaces)
{
   m_D = new D;
   m_D->simpleMesh = mesh;
   
   SimpleMesh::Point vs[3];

   if (useFaces)
   {
      m_D->bounds.set_size( 6, mesh->nf );
      for ( SimpleMesh::Index i = 0; i < mesh->nf; ++i )
      {
         getFaceVertices( mesh, i, vs );

         for ( SimpleMesh::Index ax = 0; ax < 3; ++ax )
            m_D->bounds(0+ax,i) = m_D->bounds(3+ax,i) = vs[0][ax];

         for ( SimpleMesh::Index v = 1; v < 3; ++v )
         {
            for ( SimpleMesh::Index ax = 0; ax < 3; ++ax )
            {
               m_D->bounds(0+ax,i) = std::min<double>( m_D->bounds(0+ax,i), vs[v][ax] );
               m_D->bounds(3+ax,i) = std::max<double>( m_D->bounds(3+ax,i), vs[v][ax] );
            }
         }
      }
   }
   else
   {
      m_D->bounds.set_size( 6, mesh->nv );
      for ( SimpleMesh::Index i = 0; i < mesh->nv; ++i )
      {
         Point p = mesh->getVertex(i);

         for ( SimpleMesh::Index ax = 0; ax < 3; ++ax )
            m_D->bounds(0+ax,i) = m_D->bounds(3+ax,i) = p[ax];
      }
   }
}

AWT::SimpleMesh::TreeData::~TreeData( )
{
   delete m_D;
}

AWT::SimpleMesh::TreeData::P AWT::SimpleMesh::TreeData::getInstance( Mesh::P mesh, const bool _useFaces )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::TreeData, ( mesh, _useFaces ) );
}

GETNAMEMACRO( AWT::SimpleMesh::TreeData );

AWT::OEKDTree::ObjectIndex AWT::SimpleMesh::TreeData::getNumberOfObjects( ) const
{
   return m_D->bounds.cols( );
}

double AWT::SimpleMesh::TreeData::getCoordinate( AWT::OEKDTree::ObjectIndex objectIndex, AWT::OEKDTree::AxisIndex axis ) const
{
   return ( m_D->bounds(0+axis,objectIndex) + m_D->bounds(3+axis,objectIndex) ) / 2;
}

void AWT::SimpleMesh::TreeData::getPosition( const AWT::OEKDTree::ObjectIndex objectIndex, double* pos ) const
{
   for ( AWT::OEKDTree::AxisIndex axis = 0; axis < 3; ++axis )
      pos[axis] = ( m_D->bounds(0+axis,objectIndex) + m_D->bounds(3+axis,objectIndex) ) / 2;
}

double AWT::SimpleMesh::TreeData::getMinimumBound( AWT::OEKDTree::ObjectIndex objectIndex, AWT::OEKDTree::AxisIndex axis ) const
{
   return m_D->bounds(0+axis,objectIndex);
}

double AWT::SimpleMesh::TreeData::getMaximumBound( AWT::OEKDTree::ObjectIndex objectIndex, AWT::OEKDTree::AxisIndex axis ) const
{
   return m_D->bounds(3+axis,objectIndex);
}

AWT::SimpleMesh::Mesh::P AWT::SimpleMesh::TreeData::getMesh( )
{
   return m_D->simpleMesh;
}
