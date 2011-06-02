#include "ParticlesAndMesh.h"

#include "MeshSearching/MeshApproxGeodesic.h"

#include "Maths/Sequences.h"
#include "MeshSearching/FacesNearestPointSearch.h"
#include "MeshSearching/VerticesNearestVerticesSearch.h"
#include "Mesh/VerticesTreeData.h"
#include "Mesh/TuplesImpl.h"
#include "Mesh/MeshImpl.h"

#include "vnl/vnl_sparse_matrix.h"
#include "vnl/vnl_matlab_filewrite.h"

#include "Useful/ArrayVectorFunctions.h"

template <class T>
struct AWT::ParticlesAndMesh<T>::D
{
   struct Direction { T x; T y; T z; };
   typedef std::pair< T, Direction > DistanceDirection;
   typedef std::map< MeshIndex, DistanceDirection > DistanceDirectionMap;

   typedef std::map< MeshIndex, std::vector<MeshIndex> > VertOnFaceMap;

   struct DistanceDirectionWeight
   {
      T         distance;
      Direction direction;
      T         weight;
   };

   typedef std::map< MeshIndex, DistanceDirectionWeight > AdjRow;
   typedef std::map< MeshIndex, AdjRow >                  AdjMatrix;

   struct Adjacency
   {
      void prepare( const MeshIndex N )
      {
         ddwMatrix.clear( );

         for ( MeshIndex r = 0; r < N; ++r )
            ddwMatrix[r] = AdjRow();

         weightsColumnSum.set_size( N );
         weightsColumnSum.fill( 0 );

         weightsRowSum.set_size( N );
         weightsRowSum.fill( 0 );
      }

      void insert( const MeshIndex row, const MeshIndex col, DistanceDirectionWeight ddw )
      {
         ddwMatrix[row][col] = ddw;

         weightsColumnSum( col ) += ddw.weight;
         weightsRowSum( row )    += ddw.weight;
      }

      AdjMatrix     ddwMatrix;

      vnl_vector<T> weightsColumnSum;
      vnl_vector<T> weightsRowSum;
   };

   static T truncatedDeltaGaussian( const T vtxA[3], const T vtxB[3], const T sigma, const T cutoff )
   {
      const T SQRT_TWOPI = sqrt( 8*atan(1.0) );
      const T dns = deltaNormSquared<T>( vtxA, vtxB, 3 );

      return truncatedGaussian( dns, sigma, cutoff );
   }

   static T truncatedGaussian( const T dns, const T sigma, const T cutoff )
   {
      const T SQRT_TWOPI = sqrt( 8*atan(1.0) );

      // Truncate to make exactly zero over most space
      if ( dns > cutoff*cutoff )
         return 0;

      T div = 1;
      for ( unsigned char i = 0; i < 3; ++i )
         div *= SQRT_TWOPI*sigma;

      return exp( -dns / (2*sigma*sigma ) ) / div;
   }
   
   void setSigma( const MeshIndex i, const T v, const AWT::ValueRange<T>& sigmaRange )
   {
      sigma->setPointElement( i, 0, sigmaRange.clamp( v ) );
   }

   T getSigma( const MeshIndex i ) const
   {
      return sigma->getPointElement( i, 0 );
   }

   // Using a Euclidean distance
   void getNearbyParticleDistancesE( const MeshIndex particle, const T maxDistance, DistanceDirectionMap& ddm, bool calculateDirections = false )
   {
      T vtx[3];
      T vtxOther[3];
      Direction di = { 0, 0, 0 };

      VerticesNearestVerticesSearch<T>::P searcher = VerticesNearestVerticesSearch<T>::getInstance( );
      searcher->reset( );

      particles->getPoint( particle, vtx );
      searcher->setTestPoint( vtx );
      searcher->setRadius( maxDistance );

      particlesMesh->prepareToSearchVertices( false );
      particlesMesh->search( searcher );

      ddm.clear( );

      for ( MeshIndex i = 0, imax = searcher->getNumberOfPointsFound( ); i < imax; ++i )
      {
         MeshIndex v = searcher->getPointFoundIndex( i );

         particles->getPoint( v, vtxOther );

         FOREACHAXIS( ax )
            vtxOther[ax] -= vtx[ax];

         const T len = normalize<T>( vtxOther, 3 );
         if ( calculateDirections )
         {
            di.x = vtxOther[0];
            di.y = vtxOther[1];
            di.z = vtxOther[2];
         }
         else
         {
            di.x = di.y = di.z = 0;
         }

         ddm[v] = DistanceDirection( len, di );
      }
   }

   // Using a geodesic distance
   void getNearbyParticleDistancesG( const MeshIndex particle, const T maxDistance, DistanceDirectionMap& ddm, bool calculateDirections = false )
   {
      T vtx[3];

      particles->getPoint( particle, vtx );
      geo->setOrigin( vtx, vertexFaceIndex[particle], maxDistance );

      ddm.clear( );

      T dir[] = { 0, 0, 0 };
      Direction di = { 0, 0, 0 };

      const MeshIndex nfaces = geo->getNumberOfFoundCells( );
      for ( MeshIndex fi = 0; fi < nfaces; ++fi )
      {
         // This cell is inside the radius
         const MeshIndex f = geo->getFoundCell( fi );

         // Find all the points which were associated to it on the last reprojection
         for ( std::vector<MeshIndex>::iterator it = vertsOnFace[f].begin(), en = vertsOnFace[f].end(); it != en; ++it )
         {
            particles->getPoint( *it, vtx );

            const T len = geo->distanceTo( vtx, f );

            // We may not actually need directions, e.g. for optimizing sigma
            if ( calculateDirections )
            {
               geo->directionTo( vtx, f, dir, false );

               di.x = dir[0];
               di.y = dir[1];
               di.z = dir[2];
            }

            ddm[*it] = DistanceDirection( len, di );
         }
      }
   }

   void buildAdjacency( Adjacency& adj )
   {
      const MeshIndex N = particles->getNumberOfPoints( );

      adj.prepare( N );

      for ( MeshIndex k = 0; k < N; ++k )
      {
         //PRINTVBL( k );
         // Get the current estimate
         const T sigmaK = getSigma( k );
         //PRINTVBL( sigmaK );

         // Rebuild the DistanceDirectionMap map with directions
         DistanceDirectionMap ddm;
         getNearbyParticleDistances( k, 3*sigmaK, ddm, true );

         // Go through each of the nearby points found
         for ( DistanceDirectionMap::iterator it = ddm.begin(), en = ddm.end(); it != en; ++it )
         {
            const MeshIndex         j  = it->first;
            const DistanceDirection dd = it->second;

            if ( j != k )
            {
               const T wt = D::truncatedGaussian( dd.first*dd.first, sigmaK, 3*sigmaK );

               DistanceDirectionWeight ddw = { dd.first, dd.second, wt };

               adj.insert( j, k, ddw );
            }
         }
      }
   }

   void getNearbyParticleDistances( const MeshIndex particle, const T maxDistance, DistanceDirectionMap& ddm, bool calculateDirections = false )
   {
#if CATES_USEGEODESIC
      getNearbyParticleDistancesG( particle, maxDistance, ddm, calculateDirections );
#else
      getNearbyParticleDistancesE( particle, maxDistance, ddm, calculateDirections );
#endif
   }

   void optimizeSigma( const AWT::ValueRange<T>& sigmaRange, const bool debug = false )
   {
      const MeshIndex N = particles->getNumberOfPoints( );

      DistanceDirectionMap ddm;

      ValueRange<T> tmpSigmaRange = { sigmaRange.upper, sigmaRange.lower };
      for ( MeshIndex k = 0; k < N; ++k )
      {
         //PRINTVBL( k );

         T sigmaK = sigmaRange.clamp( getSigma( k ) );
         T oldSigma = 0;

         // Use Newton's method to iteratively update the value of sigma
         for ( MeshIndex iters = 0; iters < maxSigmaIters; ++iters )
         {
            // Get the particle distances, truncated at 3*current sigma value
            // Only need to do this if sigmaK has increased
            {
               if ( 3*sigmaK > oldSigma )
               {
                  // Make the range a little bigger (1.5x) so that if sigma increases, we don't need to recalc each time
                  getNearbyParticleDistances( k, oldSigma = 4.5*sigmaK, ddm, false );
               }
            }
            
            T num = 0;
            T den = 0;

            // Go through each of the nearby points found
            for ( DistanceDirectionMap::iterator it = ddm.begin(), en = ddm.end(); it != en; ++it )
            {
               const MeshIndex         j  = it->first;
               const DistanceDirection dd = it->second;

               if ( j != k )
               {
                  const T wt = D::truncatedGaussian( dd.first*dd.first, sigmaK, 3*sigmaK );

                  num += wt * dd.first*dd.first;
                  den += wt;
               }
            }

            if ( den != 0 )
            {
               // Calculate the updated value
               sigmaK = sqrt( ( num ) / ( 3 * den ) );

               // If sigma has gone out of the allowed range, then stop
               if ( sigmaK < oldSigma && sigmaK <= sigmaRange.lower )
                  break;

               if ( sigmaK > oldSigma && sigmaK >= sigmaRange.upper )
                  break;

               if ( abs(sigmaK-oldSigma) < 1e-3 )
                  break;
            }
            else
            {
               sigmaK = 2*sigmaK;

               if ( sigmaK > sigmaRange.upper )
                  break;
            }
         }

         setSigma( k, sigmaK, sigmaRange );

         //PRINTVBL( sigmaK );
         //BLANKLINE;

         tmpSigmaRange.lower = std::min( tmpSigmaRange.lower, sigmaK );
         tmpSigmaRange.upper = std::max( tmpSigmaRange.upper, sigmaK );

         DEBUGMACRO( "**\t" << getSigma(k) );
      }

      actualSigmaRange.lower = std::max( sigmaRange.lower, tmpSigmaRange.lower );
      actualSigmaRange.upper = std::min( sigmaRange.upper, tmpSigmaRange.upper );

      PRINTVBL( actualSigmaRange.lower );
      PRINTVBL( actualSigmaRange.upper );

      /*
      if ( debug )
      {
         vnl_matrix<T> weights;
         {
            Adjacency adj;
            buildAdjacency( adj );

            AdjMatrix& adjm = adj.ddwMatrix;
            const Idx M = adjm.size();

            weights.set_size( M, M );
            weights.fill( 0 );

            for ( Idx i = 0; i < M; ++i )
            {
               for ( D::AdjRow::iterator it = adjm[i].begin(); it != adjm[i].end(); ++it )
               {
                  weights( i, it->first ) = it->second.weight;
               }
            }
         }

         ValueRange<T> tmpSigmaRange = { 0.01, 10 };

         const unsigned int steps = 50;

         vnl_matrix<T> sigmas( N, steps+1 );
         vnl_vector<T> thesig( steps+1 );
         vnl_vector<T> bestsig( N );

         for ( MeshIndex i = 0; i < steps+1; ++i )
         {
            thesig(i) = logspace( tmpSigmaRange.lower, tmpSigmaRange.upper, steps, i );
         }

         for ( MeshIndex k = 0; k < N; ++k )
         {
            PRINTVBL( k );
            const T oldSigmaK = sigmaRange.clamp( getSigma( k ) );
            bestsig( k ) = oldSigmaK;

            for ( MeshIndex i = 0; i < steps+1; ++i )
            {
               setSigma( k, thesig(i), tmpSigmaRange );

               Adjacency adj;
               buildAdjacency( adj );

               sigmas( k, i ) = calculateEntropy( adj );
            }

            setSigma( k, oldSigmaK, sigmaRange );
         }

         {
            vnl_matlab_filewrite mfw( "pam.mat" );
            mfw.write( weights,  "pam_weights" );
            mfw.write( sigmas,  "pam_sigmas" );
            mfw.write( thesig,  "pam_thesig" );
            mfw.write( bestsig, "pam_bestSigma" );
            DEBUGMACRO( __FILE__ << "\t" << "pam.mat" );
         }
      }
      */

   }

   void associateToFace( MeshIndex vertex, MeshIndex face )
   {
      const MeshIndex undefined = std::numeric_limits<MeshIndex>::max();

      // Is there currently a list of vertices on this face?
      if ( vertsOnFace.find( face ) == vertsOnFace.end() )
         vertsOnFace[ face ] = std::vector<MeshIndex>();

      // Is the vertex currently associated to a face?
      std::map<MeshIndex,MeshIndex>::iterator fn = vertexFaceIndex.find( vertex );
      if ( fn != vertexFaceIndex.end() && fn->second != undefined )
      {
         // Don't actually need to do anything
         if ( fn->second == face )
            return;

         // Need to get rid of this face from the list of vertices on this face
         std::vector<MeshIndex>::iterator it = vertsOnFace[face].begin();
         while ( it != vertsOnFace[face].end() && *it != vertex )
            ++it;

         if ( it != vertsOnFace[face].end() )
         {
            vertsOnFace[face].erase( it, it+1 );
            vertexFaceIndex[vertex] = undefined;
         }
      }

      if ( vertsOnFace.find( face ) == vertsOnFace.end() )
      {
         vertsOnFace[ face ] = std::vector<MeshIndex>();
      }

      vertexFaceIndex[vertex] = face;
      vertsOnFace[face].push_back( vertex );
   }

   // Ensure that the particles are on the surface of the mesh
   void projectParticlesOntoSurface( )
   {
      // Used to find nearest points
      FacesNearestPointSearch<T>::P searcher = FacesNearestPointSearch<T>::getInstance( );
      
      vertsOnFace.clear( );
      vertexFaceIndex.clear( );

      for ( MeshIndex v = 0, vmax = particles->getNumberOfPoints( ); v < vmax; ++v )
         projectParticleOntoSurface( v, searcher );

   }

   void projectParticleOntoSurface( const MeshIndex i )
   {
      FacesNearestPointSearch<T>::P searcher = FacesNearestPointSearch<T>::getInstance( );
      projectParticleOntoSurface( i, searcher );
   }

   void projectParticleOntoSurface( const MeshIndex v, typename FacesNearestPointSearch<T>::P searcher )
   {
      T vtx[3];

      particles->getPoint( v, vtx );
      searcher->reset( );
      searcher->setTestPoint( vtx );

      mesh->search( searcher );

      int np = searcher->getNearestPoint( vtx );
      
      associateToFace( v, np );

      particles->setPoint( v, vtx );
   
   }

   T calculateEntropy( Adjacency& adj )
   {
      const MeshIndex N = particles->getNumberOfPoints( );
      T logSum = 0;

      for ( MeshIndex i = 0; i < N; ++i )
      {
         logSum += log( adj.weightsColumnSum( i ) );
      }

      const T ret = ( logSum - N*log(static_cast<T>(N)) ) / -static_cast<T>(N);
      
      return ret;
   }

   MeshIndex maxSigmaIters;

   ValueRange<T> actualSigmaRange;

   typename Mesh<T>::P   mesh;
   typename Tuples<T>::P particles;
   typename Tuples<T>::P sigma;

   typename Mesh<T>::P   particlesMesh;

   typename MeshApproxGeodesic<T>::P geo;

   std::map<MeshIndex,MeshIndex> vertexFaceIndex;
   VertOnFaceMap vertsOnFace;
};

template <class T>
AWT::ParticlesAndMesh<T>::ParticlesAndMesh( typename AWT::Mesh<T>::P mesh, typename AWT::Tuples<T>::P particles )
{
   m_D = new D;

   m_D->maxSigmaIters = 20;

   m_D->mesh      = mesh;
   m_D->particles = particles;

   // Construct a mesh around the particles for easier searching
   m_D->particlesMesh = MeshImpl<T>::getInstance( 0, 0 );
   m_D->particlesMesh->setVertices( particles );

   // Construct the geodesic distance calculator
   m_D->geo       = MeshApproxGeodesic<T>::getInstance( m_D->mesh );

   m_D->sigma   = TuplesImpl<T>::getInstance( 1, particles->getNumberOfPoints( ) );

   const ValueRange<T> sigmaRange = { 0, 1e10 };

   const T zero[] = { 0, 0, 0 };
   for ( MeshIndex v = 0, vmax = particles->getNumberOfPoints( ); v < vmax; ++v )
   {
      m_D->setSigma( v, 100, sigmaRange );
   }

   // This projects the points onto the surface
   m_D->projectParticlesOntoSurface( );
}

template <class T>
AWT::ParticlesAndMesh<T>::~ParticlesAndMesh( )
{
   delete m_D;
}

template <class T>
typename AWT::ParticlesAndMesh<T>::P AWT::ParticlesAndMesh<T>::getInstance( typename AWT::Mesh<T>::P mesh, typename AWT::Tuples<T>::P particles )
{
   AUTOGETINSTANCE( AWT::ParticlesAndMesh<T>, ( mesh, particles ) );
}

template <class T>
GETNAMEMACRO( AWT::ParticlesAndMesh<T> );

template <class T>
typename AWT::Mesh<T>::P AWT::ParticlesAndMesh<T>::getMesh( )
{
   return m_D->mesh;
}

template <class T>
typename AWT::Tuples<T>::P AWT::ParticlesAndMesh<T>::getParticles( )
{
   return m_D->particles;
}

template <class T>
typename AWT::Tuples<T>::P AWT::ParticlesAndMesh<T>::getSigma( )
{
   return m_D->sigma;
}

template <class T>
void AWT::ParticlesAndMesh<T>::setSigmaRange( const AWT::ValueRange<T>& range )
{
   for ( MeshIndex i = 0; i < m_D->sigma->getNumberOfPoints( ); ++i )
   {
      m_D->sigma->setPointElement( i, 0, range.clamp( m_D->sigma->getPointElement( i, 0 ) ) );
   }
}

template <class T>
T AWT::ParticlesAndMesh<T>::getMinSigma( ) const
{
   return m_D->actualSigmaRange.lower;
}

template <class T>
void AWT::ParticlesAndMesh<T>::updateParticles( const typename AWT::Tuples<T>::P delta, const T dt, typename AWT::Tuples<T>::P actualDelta )
{
   const MeshIndex N = m_D->particles->getNumberOfPoints( );

   T vtx[3];
   T dlt[3];

   // Used to find nearest points
   FacesNearestPointSearch<T>::P searcher = FacesNearestPointSearch<T>::getInstance( );
   m_D->mesh->prepareToSearchFaces( );

   Tuples<T>::P oldPos = TuplesImpl<T>::getInstance( 3, N );
   for ( MeshIndex v = 0; v < N; ++v )
   {
      m_D->particles->getPoint( v, vtx );
      oldPos->setPoint( v, vtx );
   }

   for ( MeshIndex v = 0; v < N; ++v )
   {
      m_D->particles->getPoint( v, vtx );

      delta->getPoint( v, dlt );

      for ( MeshIndex ax = 0; ax < 3; ++ax )
         vtx[ax] += dt*dlt[ax];

      m_D->particles->setPoint( v, vtx );
   }

   m_D->projectParticlesOntoSurface( );

   // Return what the actual moves were
   for ( MeshIndex v = 0; v < N; ++v )
   {
      m_D->particles->getPoint( v, vtx );
      oldPos->getPoint( v, dlt );

      FOREACHAXIS( ax )
         dlt[ax] -= vtx[ax];

      actualDelta->setPoint( v, dlt );
   }
}

template <class T>
T AWT::ParticlesAndMesh<T>::calculateInternalForceUpdate( const AWT::ValueRange<T>& sigmaRange, typename AWT::Tuples<T>::P delta, T& maxTimestep )
{
   const MeshIndex N = m_D->particles->getNumberOfPoints( );

   m_D->particlesMesh->prepareToSearchVertices( true );

   // Update the optimal sigmas
   m_D->optimizeSigma( sigmaRange );

   PRINTVBL( m_D->actualSigmaRange.lower );
   PRINTVBL( m_D->actualSigmaRange.upper );

   D::Adjacency adj;
   m_D->buildAdjacency( adj );
   
   // First, calculate the value of the entropy right now
   const T ret = m_D->calculateEntropy( adj );

   T vtxDelta[3];
   T vtxDelta2[3];
   T vtxDelta_combined[3];
   T num[3];
   T denom;

   T rmsDelta_first  = 0;
   T rmsDelta_second = 0;
   T rmsDelta_combined = 0;

   for ( MeshIndex k = 0; k < N; ++k )
   {
      const T sigmaK = m_D->getSigma( k );
      const D::AdjRow& thisRow = adj.ddwMatrix[k];

      T thisInvMaxTimestep = 0;
      T thisInvMaxTimestep2 = 0;

      // Reset the accumulators
      vtxDelta[0] = vtxDelta[1] = vtxDelta[2] = 0;

      num[0] = num[1] = num[2] = 0;
      denom = 0;

      for ( D::AdjRow::const_iterator it = thisRow.begin(), en = thisRow.end(); it != en; ++it )
      {
         const MeshIndex j                     = it->first;
         const D::DistanceDirectionWeight& ddw = it->second;

         num[0] += ddw.weight * ddw.distance * ddw.direction.x;
         num[1] += ddw.weight * ddw.distance * ddw.direction.y;
         num[2] += ddw.weight * ddw.distance * ddw.direction.z;

         denom += ddw.weight;
      }

      // Right now, vtxDelta is just the numerator; need to divide it down
      if ( denom != 0 )
      {
         for ( MeshIndex ax = 0; ax < 3; ++ax )
            vtxDelta[ax] += num[ax] / denom / (sigmaK * sigmaK);

         // Update the calc for the min timestep
         thisInvMaxTimestep += 1 / ( sigmaK * sigmaK );
      }
      else
      {
         for ( MeshIndex ax = 0; ax < 3; ++ax )
            vtxDelta[ax] += 0;
      }

      vtxDelta2[0] = vtxDelta2[1] = vtxDelta2[2] = 0;
      for ( MeshIndex i = 0; i < N; ++i )
      {
         D::AdjRow::iterator it = adj.ddwMatrix[i].find( k );
         if ( it != adj.ddwMatrix[i].end( ) )
         {
            const D::DistanceDirectionWeight& ddw = it->second;

            if ( ddw.weight != 0 )
            {
               const T sigmaI = m_D->getSigma( i );

               num[0] = ddw.weight * ddw.distance * ddw.direction.x;
               num[1] = ddw.weight * ddw.distance * ddw.direction.y;
               num[2] = ddw.weight * ddw.distance * ddw.direction.z;
               
               denom = adj.weightsColumnSum( k );

               for ( MeshIndex ax = 0; ax < 3; ++ax )
                  vtxDelta2[ax] -= num[ax] / denom / (sigmaI * sigmaI);

               thisInvMaxTimestep2 += 1 / ( sigmaI * sigmaI );
            }
         }
      }

      for ( MeshIndex ax = 0; ax < 3; ++ax )
         vtxDelta_combined[ax] = vtxDelta[ax] + vtxDelta2[ax];

      rmsDelta_combined += dot( vtxDelta_combined, vtxDelta_combined, 3 );

      delta->setPoint( k, vtxDelta_combined );

      //thisInvMaxTimestep += thisInvMaxTimestep2;
      maxTimestep = std::min<T>( maxTimestep, 1 / ( thisInvMaxTimestep ) );
      

      rmsDelta_first  += dot( vtxDelta, vtxDelta, 3 );
      rmsDelta_second += dot( vtxDelta2, vtxDelta2, 3 );

      //T cosTheta = dot( vtxDelta_combined, vtxDelta, 3 ) / sqrt( dot( vtxDelta, vtxDelta, 3 ) ) / sqrt( dot( vtxDelta_combined, vtxDelta_combined, 3 ) );
      //PRINTVBL( acos( cosTheta ) * 45.0 / ( atan(1.0) ) );

   }

   rmsDelta_first  = sqrt( rmsDelta_first / N );
   rmsDelta_second = sqrt( rmsDelta_second / N );
   rmsDelta_combined = sqrt( rmsDelta_combined / N );

   BLANKLINE;
   DEBUGMACRO( rmsDelta_first << "\t" << rmsDelta_second << "\t" << rmsDelta_combined );
   PRINTVBL( rmsDelta_second / rmsDelta_first );
   BLANKLINE;

   PRINTVBL( maxTimestep );

   return ret;
}

#include "Useful/Noise.h"

template <class T>
void AWT::ParticlesAndMesh<T>::splitParticle( const MeshIndex i )
{
   const MeshIndex N = m_D->particles->getNumberOfPoints( );

   T vtx[3], sigma;
   
   // Duplicate the particles' sigma
   sigma = m_D->getSigma( i );
   m_D->sigma->addPoint( &sigma );

   // Duplicate the particle position with a slight perturbation
   m_D->particles->getPoint( i, vtx );

   for ( MeshIndex ax = 0; ax < 3; ++ax )
      vtx[ax] += sigma * Noise<T>::randn( );

   m_D->particles->addPoint( vtx );

   // Make sure that this new particle is on the surface
   m_D->projectParticleOntoSurface( N );
}

template <class T>
void AWT::ParticlesAndMesh<T>::splitParticles( )
{
   const MeshIndex N = m_D->particles->getNumberOfPoints( );

   Tuples<T>::P newParticles = m_D->particles;
   Tuples<T>::P newSigmas    = m_D->sigma;

   T vtx[3], sigma;
   for ( MeshIndex i = 0; i < N; ++i )
   {
      // Duplicate the particles' sigma
      sigma = m_D->getSigma( i );
      newSigmas->addPoint( &sigma );

      // Duplicate the particle position with a slight perturbation
      m_D->particles->getPoint( i, vtx );

      for ( MeshIndex ax = 0; ax < 3; ++ax )
         vtx[ax] += sigma * Noise<T>::randn( );

      newParticles->addPoint( vtx );
   }

   m_D->projectParticlesOntoSurface( );
}

template <class T>
void AWT::ParticlesAndMesh<T>::debugSigmas( const ValueRange<T>& sigmaRange )
{
   m_D->optimizeSigma( sigmaRange, true );
}

template class AWT::ParticlesAndMesh<double>;