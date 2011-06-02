#include "DrawDeformableMeshFitting.h"

#include "DeformableMesh/DeformableMesh.h"

#include "DeformableMeshFitting/DeformableMeshFitting.h"
#include "DeformableMeshFitting/Fragment.h"

#include "DrawableFramework/DrawableFactory.h"

#include "Mesh/AreaAveragedNormalCalculator.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/TuplesFunctions.h"
#include "Mesh/MeshConnectivity.h"
#include "Mesh/MeshDijkstra.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/TuplesImpl.h"

#include "OpenGLColourMapper.h"
#include "OpenGLDrawableCorrespondences.h"
#include "OpenGLDrawableMesh.h"
#include "OpenGLKilgardMaterial.h"
#include "OpenGLSimpleColour.h"

#include "Useful/HueWheelSequence.h"
#include "Useful/Noise.h"

#include <limits>
#include <set>
#include <queue>
#include <ctime>

template <class T>
struct AWT::DrawDeformableMeshFitting<T>::D : public DeformableMeshFittingListener<T>
{
   virtual void correspondencesFound( typename DeformableMeshFitting<T>::P dmf )
   {
      if ( *dmf != *m_Dmf )
         return;

      while ( m_Correspondencess.size( ) > dmf->getNumberOfTargetMeshes( ) )
         m_Correspondencess.pop_back( );

      for ( TargetIndex i = 0; i < dmf->getNumberOfTargetMeshes( ); ++i )
      {
         CorrespondenceFinder<T>::P corr = dmf->getCorrespondences( i );
         
         if ( m_Correspondencess.size( ) >= i )
         {
            m_Correspondencess.push_back( OpenGLDrawableCorrespondences<T>::getInstance( corr ) );
         }
         else
         {
            m_Correspondencess[i]->setCorrespondences( corr );
         }
      }

      modified( );
   }

   virtual std::string getClassName( ) const
   {
      return "AWT::DrawDeformableMeshFitting<T>::D";
   }

   bool                              m_Visible;

   typename DeformableMeshFitting<T>::P                       m_Dmf;
   typename DrawableMesh<T>::P                                m_DeformableMesh;

   std::vector<typename DrawableMesh<T>::P >                  m_TargetMeshes;
   std::vector<typename OpenGLDrawableCorrespondences<T>::P > m_Correspondencess;

   std::vector<typename DrawableMesh<T>::P >                  m_EdgeMeshes;

   std::vector<Drawable::P>                                   m_RandomDrawables;

   DrawableAssembly* m_Parent;
};

template <class T>
AWT::DrawDeformableMeshFitting<T>::DrawDeformableMeshFitting( typename DeformableMeshFitting<T>::P dmf, typename DrawableFactory<T>::P fact )
{
   m_D = new D;
   m_D->m_Parent = 0;

   m_D->m_Dmf = dmf;

   dmf->addListener( m_D );

   T maxD = 0;
   std::vector< OpenGLColourMapper<T>::P > colMaps;

   {
      HueWheelSequence::P hws = HueWheelSequence::getInstance( dmf->getNumberOfTargetMeshes( ) );

      float rgba[4];
      for ( TargetIndex i = 0; i < dmf->getNumberOfTargetMeshes( ); ++i )
      {
         Fragment<T>::P frag = dmf->getFragment( i );
         Mesh<T>::P mesh = frag->getMesh( );

         const T NOT_ON_BOUNDARY       = std::numeric_limits<T>::infinity();
         const T ON_BOUNDARY_UNVISITED = 1;
         const T ON_BOUNDARY_VISITED   = 0;

         typename Tuples<T>::P dijkstraData = TuplesImpl<T>::getInstance( 1, &NOT_ON_BOUNDARY, mesh->getNumberOfVertices() );

         hws->nextColour( rgba );

         typename OpenGLDrawableMesh<T>::P targetMesh = OpenGLDrawableMesh<T>::getInstance( mesh );

         targetMesh->setMaterial( OpenGLSimpleColour::getInstance( rgba[0], rgba[1], rgba[2], 1, true ) );

         m_D->m_TargetMeshes.push_back( targetMesh );
         
         targetMesh->setDrawAs( AWT::DRAWAS_SOLID );

         typename MeshConnectivity<T>::P conn = MeshConnectivity<T>::getInstance( mesh );

         // Prepare the initial data for running Dijkstra's algo
         //for ( MeshIndex v = 0; v < mesh->getNumberOfVertices( ); ++v )
         MESH_EACHVERTEX( mesh, v )
         {
            if ( conn->isVertexOnBoundary( v ) )
            {
               // Distance = zero if you're on the boundary
               dijkstraData->setPointElement( v, 0, ON_BOUNDARY_UNVISITED );
            }
            else
            {
               // Distance = inf initially if not on the boundary
               dijkstraData->setPointElement( v, 0, NOT_ON_BOUNDARY );
            }
         }

         {
            MeshIndex bns[8];

            // Go around all of the boundary points and see how long the cycles are
            // Prune out very short cycles
            //for ( MeshIndex vi = 0; vi < mesh->getNumberOfVertices(); ++vi )
            MESH_EACHVERTEX( mesh, vi )
            {
               const MeshIndex v = vi;

               if ( dijkstraData->getPointElement( v, 0 ) == ON_BOUNDARY_UNVISITED )
               {
                  // Hello, we have a boundary point.  Now then... let's find its cycle

                  std::vector<MeshIndex> pointsInCycle;

                  MeshIndex       lastVertex  = v;
                  MeshIndex       newVertex   = v;

                  const MeshIndex startVertex = v;

                  do
                  {
                     // Flag it so we know that we've already visited it
                     dijkstraData->setPointElement( lastVertex, 0, ON_BOUNDARY_VISITED );
                     pointsInCycle.push_back( lastVertex );

                     MeshIndex countBns = conn->getBoundaryNeighbours( lastVertex, bns );

                     if ( countBns > 2 )
                     {
                        PRINTVBL( v );
                        PRINTVEC( bns, countBns );
                     }

                     for ( MeshIndex b = 0; b < countBns; ++b )
                     {
                        // If we've not visited this vertex yet, move to this vertex
                        if ( dijkstraData->getPointElement( bns[b], 0 ) == ON_BOUNDARY_UNVISITED )
                        {
                           newVertex = bns[b];

                           b = countBns;
                        }
                     }

                     if ( newVertex == lastVertex )
                     {
                        if ( pointsInCycle.size() >= 3 )
                        {
                           if ( pointsInCycle.size() > 3 )
                           {
                              // Have we got back to the start?
                              countBns = conn->getBoundaryNeighbours( pointsInCycle.back(), bns );
                              for ( MeshIndex b = 0; b < countBns; ++b )
                              {
                                 if ( bns[b] == v )
                                    break;
                              }
                           }
                        }
                        else
                        {
                           break;
                        }

                        // Check for triangular loop
                        bool isTrigLoop = false;
                        countBns = conn->getBoundaryNeighbours( pointsInCycle.back(), bns );
                        for ( MeshIndex b = 0; !isTrigLoop && b < countBns; ++b )
                        {
                           if ( bns[b] == pointsInCycle[pointsInCycle.size()-3] )
                              isTrigLoop = true;
                        }

                        if ( isTrigLoop )
                        {
                           // Unflag the last two vertices
                           dijkstraData->setPointElement( pointsInCycle.back(), 0, NOT_ON_BOUNDARY );
                           pointsInCycle.pop_back();

                           dijkstraData->setPointElement( pointsInCycle.back(), 0, NOT_ON_BOUNDARY );
                           pointsInCycle.pop_back();

                           // Now, look at the boundary vertices of the current last vertex in the queue.
                           // If there are other unvisited boundary vertices, move to one of those.
                           // Otherwise, break.

                           countBns = conn->getBoundaryNeighbours( pointsInCycle.back(), bns );
                           bool hasAnotherPath = false;
                           for ( MeshIndex b = 0; !hasAnotherPath && b < countBns; ++b )
                           {
                              if ( dijkstraData->getPointElement( bns[b], 0 ) == ON_BOUNDARY_UNVISITED  )
                              {
                                 newVertex = bns[b];
                                 hasAnotherPath = true;
                              }
                           }

                           if ( !hasAnotherPath )
                           {
                              PRINTVBL( pointsInCycle.size() );
                              dijkstraData->setPointElement( pointsInCycle.back(), 0, NOT_ON_BOUNDARY );
                              pointsInCycle.pop_back();
                              break;
                           }
                        }
                     }

                     lastVertex = newVertex;
                  }
                  while ( lastVertex != v );

                  // If the cycle is too small, then we aren't interested, unflag it as boundary
                  if ( pointsInCycle.size() <= 10 )
                  {
                     std::vector<MeshIndex>::iterator iter    = pointsInCycle.begin();
                     std::vector<MeshIndex>::iterator iterEnd = pointsInCycle.end();

                     while ( iter != iterEnd )
                     {
                        dijkstraData->setPointElement( *iter, 0, NOT_ON_BOUNDARY );
                        ++iter;
                     }
                  }
                  else
                  {
                     PRINTVBL( lastVertex );
                     PRINTVBL( v );

                     T def[] = { 0, 0, 0 };

                     Tuples<T>::P lineData;
                     lineData = TuplesImpl<T>::getInstance( 3, def, static_cast<MeshIndex>( pointsInCycle.size() ) );

                     std::vector<MeshIndex>::iterator iter    = pointsInCycle.begin();
                     std::vector<MeshIndex>::iterator iterEnd = pointsInCycle.end();

                     T vtx[3];
                     MeshIndex i = 0;

                     while ( iter != iterEnd )
                     {
                        mesh->getVertex( *iter++, vtx );
                        lineData->setPoint( i++, vtx );
                     }

                     DrawablePoints<T>::P drawLine = fact->createPoints( lineData );
                     drawLine->setLineWidth( 2.f );
                     drawLine->setPointSize( 3.f );
                     drawLine->setClosed( false );

                     float rgba[4];
                     rgba[0] = Noise<float>::randu( 0, 1 );
                     rgba[1] = Noise<float>::randu( 0, 1 );
                     rgba[2] = Noise<float>::randu( 0, 1 );
                     rgba[3] = 1.f;

                     drawLine->setMaterial( fact->createColour( rgba, false ) );

                     m_D->m_RandomDrawables.push_back( drawLine );

                  }

               }
            }

            // Now go though and reflag things as boundary if it is non-infinite
            //for ( MeshIndex v = 0; v < mesh->getNumberOfVertices(); ++v )
            MESH_EACHVERTEX( mesh, v )
            {
               if ( dijkstraData->getPointElement( v, 0 ) != NOT_ON_BOUNDARY )
               {
                  dijkstraData->setPointElement( v, 0, 0 );
               }
            }

            MeshDijkstra<T>::P dijk = MeshDijkstra<T>::getInstance( mesh, dijkstraData );

            Tuples<T>::P data = dijk->getPointData( );

            //// Now go though and reflag things as boundary if it is non-infinite
            //for ( MeshIndex v = 0; v < mesh->getNumberOfVertices(); ++v )
            //{
            //   T val = data->getPointElement( v, 0 );
            //   data->setPointElement( v, 0, exp( -val*val / 800 ) );
            //}

            //for ( MeshIndex v = 0; v < mesh->getNumberOfVertices( ); ++v )
            MESH_EACHVERTEX( mesh, v )
            {
               T val = data->getPointElement( v, 0 );

               if ( val != std::numeric_limits<T>::infinity( ) )
                  maxD = std::max<T>( maxD, val );
            }

            OpenGLColourMapper<T>::P mapper = OpenGLColourMapper<T>::getInstance( );
            mapper->setData( data );

            targetMesh->setMaterial( mapper );

            colMaps.push_back( mapper );
         }

         break;
      }

      std::vector< OpenGLColourMapper<T>::P >::iterator beg = colMaps.begin();
      std::vector< OpenGLColourMapper<T>::P >::iterator end = colMaps.end();

      PRINTVBL( maxD );

      while ( beg != end )
      {
         (*beg)->setMax( (3*maxD)/4 );
         (*beg)->setMax( maxD );
         ++beg;
      }
   }

   typename OpenGLDrawableMesh<T>::P deformableMesh = OpenGLDrawableMesh<T>::getInstance( dmf->getDeformableMesh( ) );
   OpenGLKilgardMaterial::P matDeform = OpenGLKilgardMaterial::getInstance( "kilgard::bone", 0.5f );

   deformableMesh->setMaterial( matDeform );
   m_D->m_DeformableMesh = deformableMesh;

   deformableMesh->setDrawAs( AWT::DRAWAS_SOLID );
   //targetMesh->setDrawAs( AWT::DRAWAS_SOLID );

   typename Mesh<T>::P pmesh( *dmf->getDeformableMesh( ) );
   AreaAveragedNormalCalculator<T>::getInstance( )->calculateNormalsAndSet( *pmesh );

   m_D->correspondencesFound( dmf );

   m_D->m_Visible = true;
}

template <class T>
AWT::DrawDeformableMeshFitting<T>::~DrawDeformableMeshFitting( )
{
   m_D->m_Dmf->removeListener( m_D );

   delete m_D;
}

template <class T>
typename AWT::DrawDeformableMeshFitting<T>::P AWT::DrawDeformableMeshFitting<T>::getInstance( typename AWT::DeformableMeshFitting<T>::P dmf, typename DrawableFactory<T>::P fact )
{
   AUTOGETINSTANCE( AWT::DrawDeformableMeshFitting<T>, ( dmf, fact ) );
}

template <class T>
GETNAMEMACRO( AWT::DrawDeformableMeshFitting<T> );

template <class T>
bool AWT::DrawDeformableMeshFitting<T>::isVisible( ) const
{
   return m_D->m_Visible;
}

template <class T>
void AWT::DrawDeformableMeshFitting<T>::setVisible( const bool v )
{
   m_D->m_Visible = true;
   modified( );
}

template <class T>
void AWT::DrawDeformableMeshFitting<T>::getBounds( double* out_Bounds )
{
   for ( int i = 0; i < 3; ++i )
   {
      out_Bounds[2*i+0] =  std::numeric_limits<T>::infinity( );
      out_Bounds[2*i+1] = -std::numeric_limits<T>::infinity( );
   }

   double newBounds[6];

   if ( false && *m_D->m_DeformableMesh != 0 )
      m_D->m_DeformableMesh->getBounds( newBounds );
   else
      for ( int i = 0; i < 6; ++i )
         newBounds[i] = std::numeric_limits<T>::quiet_NaN( );

   TuplesFunctions<T>::expandBounds( newBounds, out_Bounds );

   for ( TargetIndex i = 0; i < m_D->m_TargetMeshes.size( ); ++i )
   {
      DrawableMesh<T>* mesh = *m_D->m_TargetMeshes[i];

      if ( mesh != 0 )
         mesh->getBounds( newBounds );
      else
         for ( int i = 0; i < 6; ++i )
            newBounds[i] = std::numeric_limits<T>::quiet_NaN( );

      TuplesFunctions<T>::expandBounds( newBounds, out_Bounds );
   }
}

template <class T>
void AWT::DrawDeformableMeshFitting<T>::draw( AWT::DrawContext::P context, const bool transparentPass )
{
   {
      std::vector< DrawableMesh<T>::P >::iterator it    = m_D->m_TargetMeshes.begin( );
      std::vector< DrawableMesh<T>::P >::iterator itEnd = m_D->m_TargetMeshes.end( );

      while ( it != itEnd )
      {
         if ( (*it)->isVisible( ) )
         {
            (*it)->draw( context, transparentPass );
         }

         ++it;
      }
   }

   {
      std::vector< DrawableMesh<T>::P >::iterator it    = m_D->m_EdgeMeshes.begin( );
      std::vector< DrawableMesh<T>::P >::iterator itEnd = m_D->m_EdgeMeshes.end( );

      while ( it != itEnd )
      {
         if ( (*it)->isVisible( ) )
         {
            (*it)->draw( context, transparentPass );
         }

         ++it;
      }
   }

   {
      std::vector< OpenGLDrawableCorrespondences<T>::P >::iterator it    = m_D->m_Correspondencess.begin( );
      std::vector< OpenGLDrawableCorrespondences<T>::P >::iterator itEnd = m_D->m_Correspondencess.end( );

      while ( it != itEnd )
      {
         if ( (*it)->isVisible( ) )
         {
            (*it)->draw( context, transparentPass );
         }

         ++it;
      }
   }

   {
      std::vector< Drawable::P >::iterator it    = m_D->m_RandomDrawables.begin( );
      std::vector< Drawable::P >::iterator itEnd = m_D->m_RandomDrawables.end( );

      while ( it != itEnd )
      {
         if ( (*it)->isVisible( ) )
         {
            (*it)->draw( context, transparentPass );
         }

         ++it;
      }
   }
   
   if ( false && *m_D->m_DeformableMesh != 0 && m_D->m_DeformableMesh->isVisible( ) )
   {
      m_D->m_DeformableMesh->draw( context, transparentPass );
   }

}

template <class T>
void AWT::DrawDeformableMeshFitting<T>::correspondencesFound( typename AWT::DeformableMeshFitting<T>::P dmf )
{
   if ( *dmf != *m_D->m_Dmf )
      return;

   std::vector<OpenGLDrawableCorrespondences<T>::P> newCorrespondences;

   for ( TargetIndex i = 0; i < dmf->getNumberOfTargetMeshes( ); ++i )
   {
      newCorrespondences.push_back( OpenGLDrawableCorrespondences<T>::getInstance( dmf->getCorrespondences( i ) ) );
   }

   m_D->m_Correspondencess = newCorrespondences;
   modified( );
}

template <class T>
void AWT::DrawDeformableMeshFitting<T>::setParent( DrawableAssembly* assm )
{
   m_D->m_Parent = assm;
   modified( );
}

template <class T>
AWT::DrawableAssembly* AWT::DrawDeformableMeshFitting<T>::getParent( )
{
   return m_D->m_Parent;
}

template class AWT::DrawDeformableMeshFitting<double>;
template class AWT::DrawDeformableMeshFitting<float>;
