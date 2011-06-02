#include "VanillaICPStateMachine.h"

#include "Mesh/TuplesImpl.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/AreaAveragedNormalCalculator.h"

#include "MeshSearching/FacesNearestPointSearch.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/TuplesFunctions.h"
#include "Mesh/MeshConnectivity.h"

#include "MeshSearching/VerticesNearestVertexSearch.h"

#include "DeformableMesh/AddTuples.h"

using namespace Mimlab::Matrices;

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_svd.h"

#include "vnl/vnl_matrix_fixed.h"

#include <limits>
#include <queue>

#include "LMDeformableICP.h"
#include "LMICP_AllMovable.h"
#include "vnl/algo/vnl_levenberg_marquardt.h"

template <class T>
struct AWT::VanillaICPStateMachine<T>::D
{
   class StateClass
   {
   protected:
      StateClass( const std::string desc, const bool finished = false, const bool err = false )
         : m_Desc( desc ), m_Finished( finished ), m_Err( err )
      {
      }

   public:
      virtual std::string getDescription( ) const
      {
         return m_Desc;
      }

      virtual bool        isFinished( ) const
      {
         return m_Finished;
      }

      virtual bool        isError( ) const
      {
         return m_Err;
      }

      virtual StateClass* step( D* self )
      {
         return this;
      }

   protected:
      const std::string m_Desc;
      const bool m_Finished, m_Err;
   };

   class State_Initialize : public StateClass
   {
   public:
      State_Initialize( ) : StateClass( "Initializing" ) {}
   
   public:
      virtual StateClass* step( D* self )
      {
         if ( self->m_Fragments.size( ) == 0 )
         {
            self->m_LastError = "Fragment mesh not set";
            return self->m_Error;
         }

         if ( *self->m_DeformableMesh == 0 )
         {
            self->m_LastError = "Intact mesh not set";
            return self->m_Error;
         }

         for ( unsigned int ifrag = 0; ifrag < self->m_FragsToMove; ++ifrag )
         {
            // Create an empty list of vertices for each fragment
            self->m_PickedVerts.push_back( TuplesImpl<MeshIndex>::getInstance( 1, self->m_Fragments[ifrag]->getNumberOfVertices( ) ) );

            self->m_EdgeVerts.push_back( TuplesImpl<MeshIndex>::getInstance( 1, self->m_Fragments[ifrag]->getNumberOfVertices( ) ) );

            // Create an identity transform for each fragment
            vnl_matrix_fixed<T,4,4> mat;
            mat.set_identity( );
            self->m_Transformations.push_back( mat );
         }

         self->m_MaxParallelDistance = self->m_MaxPerpendicularDistance = self->m_Rms = std::numeric_limits<T>::infinity( );

         return self->m_ChoosePoints;
      }
   };

   class State_ChoosePoints : public StateClass
   {
   public:
      State_ChoosePoints( ) : StateClass( "Choosing Points" ) {}

   public:
      virtual StateClass* step( D* self )
      {
         for ( unsigned int ifrag = 0; ifrag < self->m_FragsToMove; ++ifrag )
         {
            self->m_PickedVerts[ifrag]->clear( );
            self->m_EdgeVerts[ifrag]->clear( );

            MeshConnectivity<T>::P conn = MeshConnectivity<T>::getInstance( self->m_Fragments[ifrag] );

            MeshIndex vv[1];
            MESH_EACHVERTEX( self->m_Fragments[ifrag], v )
            {
               vv[0] = v;
               self->m_PickedVerts[ifrag]->setPoint( v, vv );

               if ( conn->isVertexOnBoundary( v ) )
                  self->m_EdgeVerts[ifrag]->addPoint( vv );
            }
         }

         return self->m_ChooseCorrespondences;
      }
   };

   class State_ChooseCorrespondences : public StateClass
   {
   public:
      State_ChooseCorrespondences( ) : StateClass( "Choosing correspondences" ) {}

   public:
      virtual StateClass* step( D* self )
      {
         // Create a copy of the deformable mesh that we can whittle down
         Mesh<T>::P whittle = MeshImpl<T>::getInstance( self->m_DeformableMesh->getNumberOfVertices( ), self->m_DeformableMesh->getNumberOfFaces( ) );

         self->m_IsFaceUsed.clear( );
         MESH_EACHFACE( self->m_DeformableMesh, f )
            self->m_IsFaceUsed.push_back( false );

         whittle->setVertices( self->m_DeformableMesh->getVertices( ) );
         
         const MeshIndex nmodes = self->m_DeformableMesh->getNumberOfActiveModes( );
         AreaAveragedNormalCalculator<T>::getInstance( )->calculateNormalsAndSet( self->m_DeformableMesh );

         T minArea = std::numeric_limits<T>::infinity( );
         for ( unsigned int ifrag = 0; ifrag < self->m_Fragments.size( ); ++ifrag )
         {
            const T thisArea = MeshFunctions<T>::getSurfaceArea( self->m_Fragments[ifrag] );
            PRINTVBL( thisArea );

            minArea = std::min<T>( minArea, thisArea );
         }
         const T minRegionArea = minArea / 2;

         // This is used to temporarily hold the jacobian at the correspondence point
         vnl_matrix<T> jac( 3, nmodes );

         // Make sure that we've got the storage allocated
         {
            while ( self->m_TemporaryDeformableModels.size( ) < self->m_Fragments.size( ) )
               self->m_TemporaryDeformableModels.push_back( AddTuples<T>::P( 0 ) );

            while ( self->m_TemporaryNormals.size( ) < self->m_Fragments.size( ) )
               self->m_TemporaryNormals.push_back( Tuples<T>::P( 0 ) );
         }

         MeshConnectivity<T>::P allConn = MeshConnectivity<T>::getInstance( self->m_DeformableMesh );

         for ( unsigned int ifrag = 0; ifrag < self->m_FragsToMove; ++ifrag )
         {
            // Create the "whittled" mesh of unused faces
            {
               // Overwrite the existing faces
               Tuples<MeshIndex>::P newFaces = TuplesImpl<MeshIndex>::getInstance( 3, self->m_DeformableMesh->getNumberOfFaces( ) );
               whittle->setFaces( newFaces );

               MeshIndex vs[3];
               MESH_EACHFACE( self->m_DeformableMesh, f )
               {
                  if ( !self->m_IsFaceUsed[f] )
                  {
                     self->m_DeformableMesh->getFaceIndices( f, vs );
                     newFaces->addPoint( vs );
                  }
               }

               self->m_RemainingMesh->setFaces( newFaces );
               AreaAveragedNormalCalculator<T>::getInstance( )->calculateNormalsAndSet( self->m_RemainingMesh );
            }

            if ( ifrag == self->m_FragsToMove )
               break;

            PRINTVBL( ifrag );

            self->m_PntFragment[ifrag]->clear( );
            self->m_NmlFragment[ifrag]->clear( );

            // Calculate normals if we don't have them
            if ( !self->m_Fragments[ifrag]->hasVertexNormals( ) )
               AreaAveragedNormalCalculator<T>::getInstance( )->calculateNormalsAndSet( self->m_Fragments[ifrag] );
      
            // Convenience variable
            const Tuples<T>::P vtxNormals = self->m_Fragments[ifrag]->getVertexNormals( );

            FacesNearestPointSearch<T>::P searcher = FacesNearestPointSearch<T>::getInstance( );
            
            // Keep a record so that we can draw which faces have been used
            MeshConnectivity<T>::P conn = MeshConnectivity<T>::getInstance( whittle );

            const MeshIndex npts = self->m_PickedVerts[ifrag]->getNumberOfPoints( );

            // Create a vector of tuples for the temporary deformable mesh
            // This will be the deformable mesh at the found correspondences
            std::vector< Tuples<T>::P > tempDeformable;
            for ( unsigned int m = 0; m < nmodes; ++m )
               tempDeformable.push_back( TuplesImpl<T>::getInstance( 3, npts ) );

            Tuples<T>::P tempNormals = TuplesImpl<T>::getInstance( 3, npts );

            MeshIndex p = 0;

            try
            {
               whittle->prepareToSearchFaces( true );

               PRINTVBL( self->m_EdgeVerts[ifrag]->getNumberOfPoints() );
               for ( p = 0; p < npts; ++p )
               {
                  T vtx[3], nml[3];
                  T nearest[3];

                  const MeshIndex vi = self->m_PickedVerts[ifrag]->getPointElement( p, 0 );
                  
                  self->m_Fragments[ifrag]->getVertex( vi, vtx );
                  vtxNormals->getPoint( vi, nml );

                  searcher->reset( );
                  searcher->setTestPoint( vtx );
                  searcher->setTestNormal( nml );

                  // Ditch correspondences outside 45 degree cone
                  searcher->setCompatibilityCosine( 0.707 );

                  // Look in the whittle mesh (faces which haven't already been "used")
                  whittle->search( searcher );

                  const int np = searcher->getNearestPoint( nearest );
                  if ( np != -1 && np > conn->getMesh( )->getNumberOfFaces( ) )
                  {
                     PRINTVBL( np );
                     PRINTVBL( conn->getMesh( )->getNumberOfFaces( ) );
                     AWTEXCEPTIONTHROW( "WTF?" );
                  }

                  if ( np != -1 && !conn->isFaceOnBoundary( np ) )
                  {
                     

                     self->m_PntFragment[ifrag]->addPoint( vtx );
                     self->m_NmlFragment[ifrag]->addPoint( nml );

                     // Work out which face this point is on
                     MeshIndex vis[3];
                     
                     whittle->getFaceIndices( np, vis );
                     
                     MeshIndex parentFace = allConn->getFaceIndex( vis[0], vis[1], vis[2] );

                     // Get the local variation at this point
                     self->m_DeformableMesh->getJacobian( parentFace, nearest, jac );
                     T vtx[3];
                     for ( unsigned int m = 0; m < nmodes; ++m )
                     {
                        vtx[0] = jac[0][m];
                        vtx[1] = jac[1][m];
                        vtx[2] = jac[2][m];

                        tempDeformable[m]->addPoint( vtx );
                     }
                     
                     // Get the normal of the face
                     self->m_DeformableMesh->getFaceNormals( )->getPoint( parentFace, vtx );
                     tempNormals->addPoint( vtx );

                     // Mark this face and the surrounding faces as in use
                     // (so they won't be matched by subsequent fragments)
                     {
                        self->m_IsFaceUsed[ np ] = true;

                        MeshIndex fs[10];
                        MeshIndex nadj = conn->getAdjacentFaces( np, fs );

                        for ( MeshIndex i = 0; i < nadj; ++i )
                           self->m_IsFaceUsed[ fs[i] ] = true;
                     }
                  }
               }
            }
            catch ( std::exception ex )
            {
               PRINTVBL( p );
               PRINTVBL( ex.what() );
               throw ex;
            }
            catch ( ... )
            {
               PRINTVBL( p );
               throw "??";
            }

            // Store the temporary deformable model
            self->m_TemporaryDeformableModels[ifrag] = AddTuples<T>::getInstance( 3 );
            for ( unsigned int m = 0; m < nmodes; ++m )
            {
               self->m_TemporaryDeformableModels[ifrag]->insert( tempDeformable[m], self->m_ModeWeights(m) );
            }

            self->m_TemporaryNormals[ifrag] = tempNormals;

            self->m_Rms = TuplesFunctions<T>::calculateRmsOrtho( 
               self->m_PntFragment[ifrag], self->m_TemporaryDeformableModels[ifrag], 
               self->m_TemporaryNormals[ifrag], 1, 1,
               self->m_MaxParallelDistance, self->m_MaxPerpendicularDistance );

            //PRINTVBL( self->m_DeformableMesh->getNumberOfFaces( ) );
            //PRINTVBL( self->m_IsFaceUsed.size( ) );
            //PAUSE;

            std::vector<MeshIndex> bigRegions;
            MESH_EACHFACE( self->m_DeformableMesh, f )
            {
               //PRINTVBL( f );
               if ( self->m_IsFaceUsed[f] == false )
               {
                  //DEBUGLINE;
                  std::vector<MeshIndex> thisRegion;

                  std::queue<MeshIndex> q;
                  q.push( f );

                  double regionArea = 0;

                  while ( !q.empty() )
                  {
                     MeshIndex ff = q.front( );
                     q.pop( );

                     if ( !self->m_IsFaceUsed[ff] )
                     {
                        regionArea += MeshFunctions<double>::getFaceArea( self->m_DeformableMesh, ff );

                        self->m_IsFaceUsed[ff] = true;
                        thisRegion.push_back( ff );

                        MeshIndex fs[10];
                        const MeshIndex nadj = allConn->getAdjacentFaces( ff, fs );

                        for ( MeshIndex i = 0; i < nadj; ++i )
                           q.push( fs[i] );
                     }
                  }

                  //PRINTVBL( regionArea );
                  if ( regionArea >= minRegionArea )
                  {
                     typedef std::vector<MeshIndex>::iterator It;
                     for ( It it = thisRegion.begin(), en = thisRegion.end(); it != en; ++it )
                        bigRegions.push_back( *it );
                  }
                  //DEBUGLINE;
               }
            }

            MESH_EACHFACE( self->m_DeformableMesh, f )
            {
               self->m_IsFaceUsed[f] = true;
            }

            typedef std::vector<MeshIndex>::iterator It;
            for ( It it = bigRegions.begin(), en = bigRegions.end(); it != en; ++it )
               self->m_IsFaceUsed[ *it ] = false;

         }

         //DEBUGLINE;
         //return self->m_FragmentMatching;

         return self->m_RigidAlignment;
      }
   };

   class State_Alignment : public StateClass
   {
   public:
      State_Alignment( ) : StateClass( "Aligning to model" ) {}

   public:
      static T linearKernel( const T v )
      {
         return v;
      }

      virtual StateClass* step( D* self )
      {
         const unsigned int nmodes = self->m_DeformableMesh->getNumberOfActiveModes( );
         const unsigned int nfrags = self->m_FragsToMove;

         unsigned int nparams = 7*nfrags + nmodes;
         unsigned int nresids = 0;
         for ( unsigned int ifrag = 0; ifrag < nfrags; ++ifrag )
            nresids += self->m_PntFragment[ifrag]->getNumberOfPoints( );

         LMMultiDeformableICP::P lmmulti = LMMultiDeformableICP::getInstance( nparams, nresids, &linearKernel );

         vnl_vector<double> params( nparams );
         params.fill( 0 );

         // Pack the poses into the parameter vector
         for ( unsigned int ifrag = 0; ifrag < nfrags; ++ifrag )
         {
            double centroid[3];
            MeshFunctions<double>::calculateFaceCentroid( self->m_Fragments[ifrag], centroid );

            lmmulti->addPair( self->m_PntFragment[ifrag], self->m_NmlFragment[ifrag],
               self->m_TemporaryDeformableModels[ifrag], self->m_TemporaryNormals[ifrag], centroid );

            params( 7*ifrag ) = 1;
         }

         // Pack the model parameters into the last slots in the vector
         for ( unsigned int m = 0; m < nmodes; ++m )
            params( 7*nfrags + m ) = self->m_ModeWeights[m];;

         LMMultiDeformableICP* vlmmulti = *lmmulti;
         vnl_levenberg_marquardt lm( *vlmmulti );

         lm.set_max_function_evals( 50 );

         lm.minimize_without_gradient( params );

         DEBUGLINE;
         PRINTVBL( params );

         // Unpack the model parameters
         self->m_ModeWeights = vectorslice( params, 7*nfrags, nmodes );

         T newRms = self->m_Rms;
         for ( unsigned int ifrag = 0; ifrag < nfrags; ++ifrag )
         {
            double centroid[3];
            MeshFunctions<double>::calculateFaceCentroid( self->m_Fragments[ifrag], centroid );

            // Unpack the pose
            vnl_vector<T> pose = vectorslice( params, 7*ifrag, 7 );

            vnl_matrix_fixed<T,4,4> Tinc = LMDeformableICP::makeTransformation( pose, centroid );

            self->m_DeformableMesh->setModeWeights( self->m_ModeWeights.data_block( ) );

            // Transform the mesh and the sample points
            MeshFunctions<T>::transformMesh( self->m_Fragments[ifrag], Tinc );
            TuplesFunctions<T>::transformPoints( self->m_PntFragment[ifrag], Tinc );
            TuplesFunctions<T>::transformPoints( self->m_NmlFragment[ifrag], Tinc, true );

            // Increment the transformation matrix
            self->m_Transformations[ifrag] = Tinc * self->m_Transformations[ifrag];

            PRINTVBL2( ifrag, self->m_Transformations[ifrag] );

            // Evaluate the RSS between the point sets
            newRms = TuplesFunctions<T>::calculateRmsOrtho( self->m_PntFragment[ifrag], 
               self->m_TemporaryDeformableModels[ifrag], self->m_TemporaryNormals[ifrag], 1, 1,
               self->m_MaxParallelDistance, self->m_MaxPerpendicularDistance );

            DEBUGMACRO( self->m_Rms << " -> " << newRms );
            DEBUGMACRO( "\t" << "max parallel      = " << self->m_MaxParallelDistance );
            DEBUGMACRO( "\t" << "max perpendicular = " << self->m_MaxPerpendicularDistance );
         }

         PRINTVBL( abs( newRms - self->m_Rms ) );

         if ( abs( newRms - self->m_Rms ) < 1e-3 /*&& newRms < self->m_Rms*/ )
         {
            PRINTVBL( nmodes );

            if ( self->m_FragsToMove != self->m_Fragments.size( ) )
            {
               ++self->m_FragsToMove;
               self->m_Rms = newRms;
               return self->m_ChooseCorrespondences;
            }
            else if ( nmodes == 1 )
            {
               self->m_DeformableMesh->activateAllModes( );

               PRINTVBL( self->m_DeformableMesh->getNumberOfActiveModes( ) );

               // If the number of modes has increased
               if ( self->m_DeformableMesh->getNumberOfActiveModes( ) != 1 )
               {
                  // Increase the length of the mode weight vector
                  vnl_vector<double> oldWeights = self->m_ModeWeights;
                  self->m_ModeWeights.set_size( self->m_DeformableMesh->getNumberOfActiveModes( ) );
                  self->m_ModeWeights.fill( 0 );

                  for ( unsigned int i = 0; i < oldWeights.size( ); ++i )
                     self->m_ModeWeights(i) = oldWeights(i);

                  self->m_Rms = newRms;
                  return self->m_ChooseCorrespondences;
               }
            }

            return self->m_Finished;
         }
         else
         {
            self->m_Rms = newRms;
            return self->m_ChooseCorrespondences;
         }
      }
   };
   
   class State_FragmentMatching : public StateClass
   {
   public:
      State_FragmentMatching( ) : StateClass( "Fragment Matching", false ) {}

      virtual StateClass* step( D* self )
      {
         // Get the list of vertices on the edges
         std::vector< Mesh<double>::P > edgeMeshes;
         vnl_vector<double> params( 7*( self->m_Fragments.size( ) - 1 ) );
         params.fill( 0 );

         unsigned int nresids = 0;

         // The first frag is fixed
         DEBUGLINE;
         for ( MeshIndex ifrag = 1; ifrag < self->m_Fragments.size( ); ++ifrag )
            params( 7*(ifrag-1) ) = 1;
         DEBUGLINE;

         // Create Meshes for each of the boundary sets
         for ( MeshIndex ifrag = 0; ifrag < self->m_Fragments.size( ); ++ifrag )
         {
            MeshConnectivity<double>::P conn = MeshConnectivity<double>::getInstance( self->m_Fragments[ifrag] );

            Tuples<double>::P verts = TuplesImpl<double>::getInstance( 3, self->m_Fragments[ifrag]->getNumberOfVertices( ) );

            double vtx[3];

            MESH_EACHVERTEX( self->m_Fragments[ifrag], v )
            {
               if ( conn->isVertexOnBoundary( v ) )
               {
                  self->m_Fragments[ifrag]->getVertex( v, vtx );
                  verts->addPoint( vtx );

                  ++nresids;
               }
            }

            // No vertices, no faces
            Mesh<double>::P mesh = MeshImpl<double>::getInstance( 0, 0 );
            mesh->setVertices( verts );
            edgeMeshes.push_back( mesh );
         }

         VerticesNearestVertexSearch<double>::P searcher = VerticesNearestVertexSearch<double>::getInstance( );

         LMICP_AllMovable::P lmicp = LMICP_AllMovable::getInstance( params.size( ), nresids );

         for ( MeshIndex ifrag = 0; ifrag < self->m_Fragments.size( ); ++ifrag )
            lmicp->addPoints( edgeMeshes[ifrag]->getVertices( ) );

         for ( MeshIndex jfrag = 0; jfrag < self->m_Fragments.size( ); ++jfrag )
         {
            edgeMeshes[jfrag]->prepareToSearchVertices( true );
         }

         // Search for the correspondences
         double vtxTest[3], vtxNear[3];
         for ( MeshIndex ifrag = 0; ifrag < self->m_Fragments.size( ); ++ifrag )
         {
            MeshIndex bestJ;
            MeshIndex bestP;
            double bestDistance = std::numeric_limits<double>::infinity( );

            MESH_EACHVERTEX( edgeMeshes[ifrag], v )
            {
               edgeMeshes[ifrag]->getVertex( v, vtxTest );

               for ( MeshIndex jfrag = 0; jfrag < self->m_Fragments.size( ); ++jfrag )
               {
                  if ( ifrag == jfrag ) continue;

                  searcher->reset( );
                  searcher->setTestPoint( vtxTest );
                  

                  edgeMeshes[jfrag]->search( searcher );

                  int np = searcher->getNearestPoint( vtxNear );

                  if ( np != -1 )
                  {
                     double dist = deltaNorm( vtxTest, vtxNear, 3 );
                     if ( dist < bestDistance )
                     {
                        bestJ = jfrag;
                        bestP = np;
                        bestDistance = dist;
                     }
                  }
               }

               lmicp->addCorrespondence( ifrag, v, bestJ, bestP );
            }
         }

         LMICP_AllMovable* plmicp = *lmicp;
         vnl_levenberg_marquardt lm( *plmicp );

         lm.set_max_function_evals( 50 );

         lm.minimize_without_gradient( params );

         PRINTVBL( params );
         vnl_vector<double> resids( lmicp->get_number_of_residuals( ) );

         BLANKLINE;
         BLANKLINE;
         lmicp->debugOn( );
         DEBUGMACRO( "========" );
         lmicp->f( params, resids );
         lmicp->debugOff( );

         for ( MeshIndex ifrag = 0; ifrag < self->m_Fragments.size( ); ++ifrag )
         {
            double centroid[3];
            MeshFunctions<T>::calculateVertexCentroid( edgeMeshes[ifrag], centroid );

            // Unpack the pose
            vnl_vector<T> pose;
            
            if ( ifrag == 0 )
            {
               pose = vnl_vector<T>( 7 );
               pose.fill( 0 );
               pose(0) = 1;
            }
            else
            {
               pose = vectorslice( params, 7*(ifrag-1), 7 );
            }

            vnl_matrix_fixed<T,4,4> Tinc = LMDeformableICP::makeTransformation( pose, centroid );

            PRINTVBL( Tinc );

            // Transform the mesh and the sample points
            MeshFunctions<T>::transformMesh( self->m_Fragments[ifrag], Tinc );
            TuplesFunctions<T>::transformPoints( self->m_PntFragment[ifrag], Tinc );
            TuplesFunctions<T>::transformPoints( self->m_NmlFragment[ifrag], Tinc, true );

            // Increment the transformation matrix
            self->m_Transformations[ifrag] = Tinc * self->m_Transformations[ifrag];

            // Evaluate the RSS between the point sets
            T newRms = TuplesFunctions<T>::calculateRmsOrtho( self->m_PntFragment[ifrag], 
               self->m_TemporaryDeformableModels[ifrag], self->m_TemporaryNormals[ifrag], 1, 1,
               self->m_MaxParallelDistance, self->m_MaxPerpendicularDistance );

            DEBUGMACRO( self->m_Rms << " -> " << newRms );
            DEBUGMACRO( "\t" << "max parallel      = " << self->m_MaxParallelDistance );
            DEBUGMACRO( "\t" << "max perpendicular = " << self->m_MaxPerpendicularDistance );
         }
         //PAUSE;

         return self->m_FragmentMatching;
      }
   };

   class State_Finished : public StateClass
   {
   public:
      State_Finished( ) : StateClass( "Finished!", true ) {}

   public:
      virtual StateClass* step( D* self )
      {
         self->m_Transformation = vnl_svd<double>( self->m_Transformations[1] ).inverse( ) * self->m_Transformations[0];
         PRINTVBL( self->m_Transformation );
         PRINTVBL( self->m_ModeWeights );
         // Get all of the faces which haven't been used in nearest point calculations
      
         // From the finished state, we don't want to go anywhere
         return this;
      }
   };

   class State_Error : public StateClass
   {
   public:
      State_Error( ) : StateClass( "Errrror!", true, true ) {}

   public:
      virtual StateClass* step( D* self )
      {
         // From the error state, we don't want to go anywhere
         return this;
      }
   };

   class State_Undefined : public StateClass
   {
   public:
      State_Undefined( ) : StateClass( "State Undefined", true, true ) {}

   public:
      virtual StateClass* step( D* self )
      {
         // This is an error state
         return this;
      }
   };

   D( )
   {
      m_Initialize            = new State_Initialize( );
      m_ChoosePoints          = new State_ChoosePoints( );
      m_ChooseCorrespondences = new State_ChooseCorrespondences( );
      m_RigidAlignment        = new State_Alignment( );
      m_FragmentMatching      = new State_FragmentMatching( );
      m_Finished              = new State_Finished( );
      m_Error                 = new State_Error( );
      m_Undefined             = new State_Undefined( );

      m_NextState = m_CurrentState = m_Initialize;
      m_LastError = "";
   }

   virtual ~D( )
   {
      delete m_Initialize;
      delete m_ChoosePoints;
      delete m_ChooseCorrespondences;
      delete m_RigidAlignment;
      delete m_FragmentMatching;
      delete m_Finished;
      delete m_Error;
      delete m_Undefined;
   }

   bool step( )
   {
      m_CurrentState = m_NextState;

      BLANKLINE;
      DEBUGMACRO( "> " << m_CurrentState->getDescription( ) );
      BLANKLINE;

      m_NextState    = m_CurrentState->step( this );
      
      BLANKLINE;
      DEBUGMACRO( "< " << m_CurrentState->getDescription( ) );
      BLANKLINE;

      return !m_NextState->isError( );
   }

   StateClass* getCurrentState( )
   {
      return m_CurrentState;
   }

   std::string getLastError( )
   {
      return m_LastError;
   }

   void addFragmentMesh( typename Mesh<T>::P mesh )
   {
      m_Fragments.push_back( mesh );

      m_PntFragment.push_back( TuplesImpl<T>::getInstance( 3, mesh->getNumberOfVertices( ) ) );
      m_NmlFragment.push_back( TuplesImpl<T>::getInstance( 3, mesh->getNumberOfVertices( ) ) );

      m_FragsToMove = m_Fragments.size( );
   }

   void setDeformableMesh( typename DeformableMesh<T>::P mesh )
   {
      m_DeformableMesh = mesh;

      if ( !mesh->hasVertexNormals( ) )
         AreaAveragedNormalCalculator<T>::getInstance( )->calculateNormalsAndSet( mesh );

      m_ModeWeights = vnl_vector<T>( mesh->getNumberOfActiveModes( ) );
      m_ModeWeights[0] = 1;

      mesh->setActiveModes( 1 );

      m_RemainingMesh = MeshImpl<T>::getInstance( 1, 1 );
      m_RemainingMesh->setVertices( m_DeformableMesh->getVertices( ) );
      m_RemainingMesh->setFaces( m_DeformableMesh->getFaces( ) );
   }

   State_Initialize*            m_Initialize;
   State_ChoosePoints*          m_ChoosePoints;
   State_Alignment*             m_RigidAlignment;
   State_ChooseCorrespondences* m_ChooseCorrespondences;
   State_FragmentMatching*      m_FragmentMatching;
   State_Finished*              m_Finished;
   State_Error*                 m_Error;
   State_Undefined*             m_Undefined;

   StateClass*                  m_CurrentState;
   StateClass*                  m_NextState;

   std::string                  m_LastError;

   StateMachineStack::P         m_Stack;

   bool                         m_IsError;

   typename DeformableMesh<T>::P  m_DeformableMesh;
   typename Mesh<T>::P  m_RemainingMesh;

   std::vector< typename Mesh<T>::P > m_Fragments;
   
   std::vector< typename Tuples<MeshIndex>::P > m_PickedVerts;
   std::vector< typename Tuples<MeshIndex>::P > m_EdgeVerts;

   std::vector< typename Tuples<T>::P > m_PntFragment;
   std::vector< typename Tuples<T>::P > m_NmlFragment;

   std::vector< typename AddTuples<T>::P > m_TemporaryDeformableModels;
   std::vector< typename Tuples<T>::P >    m_TemporaryNormals;

   std::vector< vnl_matrix_fixed<T,4,4> > m_Transformations;

   T                              m_Rms;
   T                              m_MaxParallelDistance;
   T                              m_MaxPerpendicularDistance;

   unsigned int m_FragsToMove;

   vnl_matrix_fixed<T,4,4>        m_Transformation;
   vnl_vector<T>                  m_ModeWeights;

   std::vector<bool> m_IsFaceUsed;
};

template <class T>
AWT::VanillaICPStateMachine<T>::VanillaICPStateMachine( AWT::StateMachineStack::P stack )
{
   m_D = new D;

   m_D->m_Stack = stack;
}

template <class T>
AWT::VanillaICPStateMachine<T>::~VanillaICPStateMachine( )
{
   delete m_D;
}

template <class T>
typename AWT::VanillaICPStateMachine<T>::P AWT::VanillaICPStateMachine<T>::getInstance( AWT::StateMachineStack::P stack )
{
   AUTOGETINSTANCE( AWT::VanillaICPStateMachine<T>, ( stack ) );
}

template <class T>
GETNAMEMACRO( AWT::VanillaICPStateMachine<T> );

template <class T>
AWT::StateMachineStack* AWT::VanillaICPStateMachine<T>::getStack( )
{
   return *m_D->m_Stack;
}

template <class T>
bool AWT::VanillaICPStateMachine<T>::step( )
{
   return m_D->step( );
}

template <class T>
bool AWT::VanillaICPStateMachine<T>::isRunning( ) const
{
   return !m_D->getCurrentState( )->isFinished( );
}

template <class T>
bool AWT::VanillaICPStateMachine<T>::isError( ) const
{
   return m_D->getCurrentState( )->isError( );
}

template <class T>
std::string AWT::VanillaICPStateMachine<T>::getLastError( ) const
{
   return m_D->getLastError( );
}

template <class T>
std::string AWT::VanillaICPStateMachine<T>::getStateName( ) const
{
   return m_D->getCurrentState( )->getDescription( );
}

template <class T>
void AWT::VanillaICPStateMachine<T>::addFragment( typename AWT::Mesh<T>::P frag )
{
   m_D->addFragmentMesh( frag );
}

template <class T>
typename AWT::Mesh<T>::P AWT::VanillaICPStateMachine<T>::getFragment( MeshIndex i )
{
   return m_D->m_Fragments[i];
}

template <class T>
void AWT::VanillaICPStateMachine<T>::setDeformableMesh( typename AWT::DeformableMesh<T>::P mesh )
{
   m_D->setDeformableMesh( mesh );
}

template <class T>
typename AWT::DeformableMesh<T>::P AWT::VanillaICPStateMachine<T>::getDeformableMesh( )
{
   return m_D->m_DeformableMesh;
}

template <class T>
T AWT::VanillaICPStateMachine<T>::getRms( ) const
{
   return m_D->m_Rms;
}

template <class T>
vnl_matrix_fixed<T,4,4> AWT::VanillaICPStateMachine<T>::getTransformation( ) const
{
   return m_D->m_Transformation;
}

template <class T>
vnl_vector<T> AWT::VanillaICPStateMachine<T>::getModeWeights( ) const
{
   return m_D->m_ModeWeights;
}

template <class T>
typename AWT::Tuples<T>::P AWT::VanillaICPStateMachine<T>::getFragmentPoints( MeshIndex i )
{
   return m_D->m_PntFragment[i];
}

template <class T>
typename AWT::Tuples<T>::P AWT::VanillaICPStateMachine<T>::getIntactPoints( MeshIndex i )
{
   return m_D->m_TemporaryDeformableModels[i];
}

template <class T>
T AWT::VanillaICPStateMachine<T>::getMaxParallelDistance( ) const
{
   return m_D->m_MaxParallelDistance;
}

template <class T>
T AWT::VanillaICPStateMachine<T>::getMaxPerpendicularDistance( ) const
{
   return m_D->m_MaxPerpendicularDistance;
}

template <class T>
typename AWT::Mesh<T>::P AWT::VanillaICPStateMachine<T>::getRemainingMesh( )
{
   return m_D->m_RemainingMesh;
}

template class AWT::VanillaICPStateMachine<double>;