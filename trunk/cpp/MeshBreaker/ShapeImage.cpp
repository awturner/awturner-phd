#include "ShapeImage.h"

#include <fstream>

#include "FlattenMesh.h"
#include "Mesh/TuplesImpl.h"

template <class T>
struct AWT::ShapeImage<T>::D
{
   MeshIndex xy_to_index( MeshIndex x, MeshIndex y ) { return x*ngrid + y;   }
   MeshIndex index_to_x( MeshIndex index )           { return index / ngrid; }
   MeshIndex index_to_y( MeshIndex index )           { return index % ngrid; }

   typename Tuples<T>::P buildInitialMap( )
   {
      Tuples<T>::P map = TuplesImpl<T>::getInstance( 2, ngrid*ngrid );

      // Populate the map with initial (identity) values
      for ( MeshIndex a = 0; a < ngrid; ++a )
      {
         T value = static_cast<T>( a ) / static_cast<T>( ngrid );

         for ( MeshIndex b = 0; b < ngrid; ++b )
         {
            map->setPointElement( xy_to_index( a, b ), 0, value );
            map->setPointElement( xy_to_index( b, a ), 1, value );
         }
      }

      return map;
   }

   void process( typename FlattenMeshPair<T>::P flat )
   {
      // We need an image which maps from shape image coordinates to flattened mesh coordinates
      // It is this image which will be manipulated in order to improve the sampling of the mesh
      Tuples<T>::P map = buildInitialMap( );

      // We will store the gradient of movement in here
      Tuples<T>::P gradient = TuplesImpl<T>::getInstance( 3, ngrid*ngrid );
      Tuples<T>::P gradient3 = TuplesImpl<T>::getInstance( 3, ngrid*ngrid );

      MeshIndex is[] = { ngrid-1, 0, 1 };

      T thisMapPos[2];
      T thisVertex[3];
      T otherMapPos[2];
      T otherVertex[3];

      const T sigma2 = 10;

      T num[3];
      T den;

      Tuples<T>::P oldMesh = TuplesImpl<T>::getInstance( 3, ngrid*ngrid );
      for ( MeshIndex v = 0; v < ngrid*ngrid; ++v )
      {
         T flatCoords[2], meshCoords[3];

         // Project from flat to mesh
         map->getPoint( v, flatCoords );
         flat->mapFlattenedToMesh( flatCoords, meshCoords );

         oldMesh->setPoint( v, meshCoords );
      }

      for ( MeshIndex it = 0; true; ++it )
      //while ( true )
      {
         DEBUGLINE;
         for ( MeshIndex v = 0; v < ngrid*ngrid; ++v )
         {
            FOREACHAXIS( ax )
               gradient->setPointElement( v, ax, 0 );
         }

         // So... my idea is that I can manipulate the map to improve the spread by moving each vertex
         // away from the weighted mean of its neighbours

         DEBUGLINE;
         for ( MeshIndex v = 0; v < ngrid*ngrid; ++v )
         {
            MeshIndex gx = index_to_x( v );
            MeshIndex gy = index_to_y( v );

            // Get the location of "this" vertex in the mesh
            map->getPoint( v, thisMapPos );
            flat->mapFlattenedToMesh( thisMapPos, thisVertex );

            FOREACHAXIS( ax ) num[ax] = 0;
            den = 0;
            for ( MeshIndex b = 0; b < 3; ++b )
            {
               for ( MeshIndex a = 0; a < 3; ++a )
               {
                  // Do we want to count "self" in the local mode calculation?
                  // Don't do anything for "self"
                  // if ( a == b ) continue;

                  // Get the location of the other vertex in the mesh
                  map->getPoint( xy_to_index( (gx+is[a])%ngrid, (gy+is[b])%ngrid ), otherMapPos );
                  flat->mapFlattenedToMesh( otherMapPos, otherVertex );

                  // Now, calculate the gaussian weight between them
                  T delta2 = 0;
                  FOREACHAXIS( ax )
                     delta2 += ( otherVertex[ax] - thisVertex[ax] ) * ( otherVertex[ax] - thisVertex[ax] );

                  T weight = exp( -delta2 / sigma2 );

                  FOREACHAXIS( ax ) num[ax] += weight*otherVertex[ax];
                  den += weight;
               }
            }

            if ( den != 0 )
            {
               // num is now the mean mesh coordinate of the local sampling neighbourhood
               // It is not necessarily on the mesh, so we need to find the closest point
               // on the mesh to this mean and find its parametrized coordinates
               FOREACHAXIS( ax ) num[ax] = ( num[ax] / den ) - thisVertex[ax];
            }
            else
            {
               FOREACHAXIS( ax ) num[ax] = 0;
            }

            gradient->setPoint( v, num );
         }
         DEBUGLINE;

         // Now, need to work out how much of the gradient can be added to preserve the diffeomorphism
         T dt = static_cast<T>( 0.1 );
         
#if 0
         T minDetJ = -1;
         T detThresh = static_cast<T>( 0.1 );

         const T spacing = static_cast<T>( 1 ) / static_cast<T>( ngrid );

         while ( false && minDetJ < detThresh && dt > 0 )
         {
            PRINTVBL( dt );
            PRINTVBL( minDetJ );

            // Calculate the determinant of the jacobian at each point and make sure it is sufficiently large
            minDetJ = std::numeric_limits<T>::infinity( );

            MeshIndex v;
            for ( v = 0; v < ngrid*ngrid && minDetJ > detThresh; ++v )
            {
               const MeshIndex gx = index_to_x( v );
               const MeshIndex gy = index_to_y( v );

               const MeshIndex gxp = ( gx + 1 ) % ngrid;
               const MeshIndex gxm = ( gx + ngrid - 1 ) % ngrid;

               const MeshIndex gyp = ( gy + 1 ) % ngrid;
               const MeshIndex gym = ( gy + ngrid - 1 ) % ngrid;

               const T vxp[] = { 
                  map->getPointElement( xy_to_index( gxp, gy ), 0 ) - dt*gradient->getPointElement( xy_to_index( gxp, gy ), 0 ),
                  map->getPointElement( xy_to_index( gxp, gy ), 1 ) - dt*gradient->getPointElement( xy_to_index( gxp, gy ), 1 ),
               };
               const T vxm[] = { 
                  map->getPointElement( xy_to_index( gxm, gy ), 0 ) - dt*gradient->getPointElement( xy_to_index( gxm, gy ), 0 ),
                  map->getPointElement( xy_to_index( gxm, gy ), 1 ) - dt*gradient->getPointElement( xy_to_index( gxm, gy ), 1 ),
               };

               const T vyp[] = { 
                  map->getPointElement( xy_to_index( gx, gyp ), 0 ) - dt*gradient->getPointElement( xy_to_index( gx, gyp ), 0 ), 
                  map->getPointElement( xy_to_index( gx, gyp ), 1 ) - dt*gradient->getPointElement( xy_to_index( gx, gyp ), 1 ), 
               };
               const T vym[] = { 
                  map->getPointElement( xy_to_index( gx, gym ), 0 ) - dt*gradient->getPointElement( xy_to_index( gx, gym ), 0 ), 
                  map->getPointElement( xy_to_index( gx, gym ), 1 ) - dt*gradient->getPointElement( xy_to_index( gx, gym ), 1 ), 
               };

               const T dvx_by_dx = ( vxp[0] - vxm[0] ) / ( 2*spacing );
               const T dvy_by_dx = ( vxp[1] - vxm[1] ) / ( 2*spacing );

               const T dvx_by_dy = ( vyp[0] - vym[0] ) / ( 2*spacing );
               const T dvy_by_dy = ( vyp[1] - vym[1] ) / ( 2*spacing );

               const T detJ = dvx_by_dx*dvy_by_dy - dvy_by_dx*dvx_by_dy;

               minDetJ = std::min( minDetJ, detJ );
            }

            if ( v == ngrid*ngrid )
            {
               for ( MeshIndex v = 0; v < ngrid*ngrid; ++v )
               {
                  map->setPointElement( v, 0, map->getPointElement( v, 0 ) - dt*gradient->getPointElement( v, 0 ) );
                  map->setPointElement( v, 1, map->getPointElement( v, 1 ) - dt*gradient->getPointElement( v, 1 ) );
               }
               break;
            }
            else
            {
               dt /= 2;
            }
         }
#endif

         DEBUGLINE;
         if ( it % 1 == 0 )
         {
            std::ofstream fs( "remap_script.m" );
            fs << "close all; clear; clc;" << std::endl;
            fs << "remap = [" << std::endl;

            Tuples<T>::P newMesh = TuplesImpl<T>::getInstance( 3, ngrid*ngrid );
            Tuples<T>::P newParam = TuplesImpl<T>::getInstance( 2, ngrid*ngrid );

            for ( MeshIndex v = 0; v < ngrid*ngrid; ++v )
            {
               T flatCoords[2], meshCoords[3], flatMove[2], projCoords[3];

               // Project from flat to mesh
               map->getPoint( v, flatCoords );
               flat->mapFlattenedToMesh( flatCoords, meshCoords );

               if ( it == 0 )
               {
                  oldMesh->setPoint( v, meshCoords );
               }

               // Add on the gradient move
               FOREACHAXIS( ax )
                  meshCoords[ax] -= static_cast<T>( 0.25 )*gradient->getPointElement( v, ax );

               // Project back into the flat
               flat->mapMeshToFlattened( meshCoords, flatMove );

               // This moved position is the new parametrization
               newParam->setPoint( v, flatMove );

               // And back onto the mesh
               flat->mapFlattenedToMesh( flatMove, projCoords );

               newMesh->setPoint( v, projCoords );

               flatMove[0] -= flatCoords[0];
               flatMove[1] -= flatCoords[1];

               fs << flatCoords[0] << " " << flatCoords[1] << " " << flatMove[0] << " " << flatMove[1] << std::endl;
            }
            fs << "]';" << std::endl;
            //fs << "quiver( remap(1,:), remap(2,:), remap(3,:), remap(4,:), 0 ); axis equal tight; hold on" << std::endl;

            map = newParam;

            fs << "oldmesh = [" << std::endl;
            T projCoords[3];
            for ( MeshIndex v = 0; v < ngrid*ngrid; ++v )
            {
               oldMesh->getPoint( v, projCoords );
               fs << projCoords[0] << " " << projCoords[1] << " " << projCoords[2] << std::endl;
            }
            fs << "]';" << std::endl;

            fs << "newmesh = [" << std::endl;
            for ( MeshIndex v = 0; v < ngrid*ngrid; ++v )
            {
               T projCoords[3];

               newMesh->getPoint( v, projCoords );
               fs << projCoords[0] << " " << projCoords[1] << " " << projCoords[2] << std::endl;
            }
            fs << "]';" << std::endl;

            fs << "figure; hold on;" << std::endl;
            //fs << "for i = 0:32:(1024-32); plot3x( oldmesh(:,i+[1:32 1]), 'k:' ); end" << std::endl;
            //fs << "for i = 0:31; plot3x( oldmesh(:,i+[1:32:1024 1]), 'k:' ); end" << std::endl;
            fs << "for i = 0:32:(1024-32); plot3x( newmesh(:,i+[1:32 1]), 'b' ); end" << std::endl;
            fs << "for i = 0:31; plot3x( newmesh(:,i+[1:32:1024 1]), 'b' ); end" << std::endl;
            fs << "axis equal tight; hold off" << std::endl;
            fs.close( );

            if ( it % 20 == 19 )
               PAUSE;
         }
         DEBUGLINE;
      }
   }

   unsigned int ngrid;
};

template <class T>
AWT::ShapeImage<T>::ShapeImage( typename FlattenMeshPair<T>::P flat, unsigned int ngrid )
{
   m_D = new D;

   m_D->ngrid = ngrid;

   m_D->process( flat );
}

template <class T>
AWT::ShapeImage<T>::~ShapeImage( )
{
   delete m_D;
}

template <class T>
typename AWT::ShapeImage<T>::P AWT::ShapeImage<T>::getInstance( typename AWT::FlattenMeshPair<T>::P flat, unsigned int ngrid )
{
   AUTOGETINSTANCE( AWT::ShapeImage<T>, ( flat, ngrid ) );
}

template <class T>
GETNAMEMACRO( AWT::ShapeImage<T> );

template <class T>
AWT::MeshIndex AWT::ShapeImage<T>::mapMeshToFlattened( const T* meshCoords, T* flatCoords )
{
   return 0;
}

template <class T>
AWT::MeshIndex AWT::ShapeImage<T>::mapFlattenedToMesh( const T* flatCoords, T* meshCoords )
{
   return 0;
}

template class AWT::ShapeImage<double>;
template class AWT::ShapeImage<float>;