#include "MeshFunctions.h"

#include "Mesh.h"
#include "MeshImpl.h"
#include "Useful/ArrayVectorFunctions.h"
#include "Useful/Noise.h"
#include "AreaAveragedNormalCalculator.h"
#include "MeshConnectivity.h"
#include "TuplesFunctions.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"

template <class T>
AWT::MeshFunctions<T>::MeshFunctions( )
{
}

template <class T>
AWT::MeshFunctions<T>::~MeshFunctions( )
{
}

template <class T>
void AWT::MeshFunctions<T>::calculateFaceCentroid( const typename Mesh<T>::P mesh, const MeshIndex in_Face, T* out_Centroid )
{
   T vtx[3][3];
   mesh->getFace( in_Face, vtx[0], vtx[1], vtx[2] );

   for ( int i = 0; i < 3; ++i )
      out_Centroid[i] = 0;

   for ( int i = 0; i < 3; ++i )
      for ( int j = 0; j < 3; ++j )
         out_Centroid[i] += vtx[j][i];

   for ( int i = 0; i < 3; ++i )
      out_Centroid[i] /= 3;
}

template <class T>
void AWT::MeshFunctions<T>::calculateVertexCentroid( const typename Mesh<T>::P mesh, T* out_Centroid )
{
   out_Centroid[0] = out_Centroid[1] = out_Centroid[2] = 0;

   T vtx[3];
   
   //for ( AWT::MeshIndex v = 0; v < mesh->getNumberOfVertices( ); ++v )
   MESH_EACHVERTEX( mesh, v )
   {
      mesh->getVertex( v, vtx );

      out_Centroid[0] += vtx[0];
      out_Centroid[1] += vtx[1];
      out_Centroid[2] += vtx[2];
   }

   out_Centroid[0] /= mesh->getNumberOfVertices( );
   out_Centroid[1] /= mesh->getNumberOfVertices( );
   out_Centroid[2] /= mesh->getNumberOfVertices( );
}

template <class T>
void AWT::MeshFunctions<T>::calculateFaceCentroid( const typename Mesh<T>::P mesh, T* out_Centroid )
{
   out_Centroid[0] = out_Centroid[1] = out_Centroid[2] = 0;

   T vtx[3];
   T den = 0;
   
   MESH_EACHFACE( mesh, f )
   {
      T area = getFaceArea( mesh, f );

      calculateFaceCentroid( mesh, f, vtx );

      FOREACHAXIS( ax )
         out_Centroid[ax] += area * vtx[ax];

      den += area;
   }
   
   scale<T>( out_Centroid, 1 / den, 3 );
}

template <class T>
T AWT::MeshFunctions<T>::calculateVariance( const typename Mesh<T>::P mesh )
{
   T sum_xtx = 0;
   T sum_x[] = { 0, 0, 0 };
   T vtx[3];
   
   const MeshIndex nverts = mesh->getNumberOfVertices( );

   for ( AWT::MeshIndex v = 0; v < nverts; ++v )
   {
      mesh->getVertex( v, vtx );

      sum_x[0] += vtx[0];
      sum_x[1] += vtx[1];
      sum_x[2] += vtx[2];

      sum_xtx += dot<T>( vtx, vtx, 3 );
   }

   T ret = ( sum_xtx - dot<T>( sum_x, sum_x, 3 ) / nverts ) / nverts;

   return ret;
}

template <class T>
T AWT::MeshFunctions<T>::getFaceArea( const typename AWT::Mesh<T>::P mesh, const AWT::MeshIndex in_Index )
{
   T point[3][3];
   mesh->getFace( in_Index, &point[0][0], &point[1][0], &point[2][0] );

   // Get the directions of the edges relative to the zero-th point
   for ( MeshIndex j = 0; j < 3; ++j )
   {
      point[1][j] -= point[0][j];
      point[2][j] -= point[0][j];
   }

   T nml[3];
   // Take the cross product of the edges
   nml[0] = point[1][1]*point[2][2] - point[1][2]*point[2][1];
   nml[1] = point[1][2]*point[2][0] - point[1][0]*point[2][2];
   nml[2] = point[1][0]*point[2][1] - point[1][1]*point[2][0];

   T len = sqrt( nml[0]*nml[0] + nml[1]*nml[1] + nml[2]*nml[2] ) / 2;

   return len;
}

template <class T>
T AWT::MeshFunctions<T>::getSurfaceArea( const typename AWT::Mesh<T>::P mesh )
{
   T ret = 0;

   //for ( MeshIndex f = 0; f < mesh->getNumberOfFaces( ); ++f )
   MESH_EACHFACE( mesh, f )
      ret += getFaceArea( mesh, f );

   return ret;
}

/*
template <class T>
void AWT::MeshFunctions<T>::calculateBounds( const typename AWT::Mesh<T>::P mesh, double* out_Bounds )
{
   TuplesFunctions<T>::calculateBounds( mesh->getVertices, out_Bounds );
}

template <class T>
void AWT::MeshFunctions<T>::expandBounds( const typename AWT::Mesh<T>::P mesh, double* out_Bounds )
{
   double bounds[6];
   calculateBounds( mesh, bounds );
   expandBounds( bounds, out_Bounds );
}

template <class T>
void AWT::MeshFunctions<T>::expandBounds( const double* bounds, double* out_Bounds )
{
   for ( int ax = 0; ax < 3; ++ax )
   {
      out_Bounds[2*ax+0] = std::min<double>( out_Bounds[2*ax+0], bounds[2*ax+0] );
      out_Bounds[2*ax+1] = std::max<double>( out_Bounds[2*ax+1], bounds[2*ax+1] );
   }
}
*/

template <class T>
void AWT::MeshFunctions<T>::calculatePrincipalAxes(const typename AWT::Mesh<T>::P mesh, T out_Axes[3][3] )
{
   // Firstly, let's get the centroid
   T mu[3], vtx[3];
   calculateFaceCentroid( mesh, mu );

   vnl_matrix<T> A(3,3);
   A.fill( 0 );

   MESH_EACHVERTEX( mesh, v )
   {
      mesh->getVertex( v, vtx );

      FOREACHAXIS( ax )
         vtx[ax] -= mu[ax];

      FOREACHAXIS( i )
      {
         FOREACHAXIS( j )
         {
               A( i, j ) += vtx[i]*vtx[j];
         }
      }
   }

   vnl_symmetric_eigensystem<T> eig( A );
   
   // Don't know which way the eigenvalues come out...
   if ( eig.get_eigenvalue( 0 ) > eig.get_eigenvalue( 2 ) )
   {
      FOREACHAXIS( i )
      {
         vnl_vector<T> vec = eig.get_eigenvector( i );
         FOREACHAXIS( ax )
            out_Axes[i][ax] = vec( ax );
      }
   }
   else
   {
      FOREACHAXIS( i )
      {
         vnl_vector<T> vec = eig.get_eigenvector( 2-i );
         FOREACHAXIS( ax )
            out_Axes[i][ax] = vec( ax );
      }
   }

   // Calculate the determinant of the axes - we want it to be right-handed
   T det = 0;
   FOREACHAXIS( ax )
   {
      const unsigned int bx = (ax+1)%3;
      const unsigned int cx = (ax+2)%3;

      det += out_Axes[ax][0] * ( out_Axes[bx][1]*out_Axes[cx][2] - out_Axes[bx][2]*out_Axes[cx][1] );
   }
   
   if ( det < 0 )
   {
      // Flip the smallest axis if we have an LH system
      for ( int ax = 0; ax < 3; ++ax )
         out_Axes[2][ax] *= -1;
   }
}

template <class T>
void AWT::MeshFunctions<T>::constructOrthonormalAxes( const T in_Z[3], const T* in_Up, T out_Axes[3][3] )
{
   FOREACHAXIS( ax )
      out_Axes[2][ax] = in_Z[ax];

   if ( in_Up == 0 )
   {
      FOREACHAXIS( ax )
      {
         do
         {
            out_Axes[1][ax] = Noise<T>::randu( -1, 1 );
         }
         while ( abs(out_Axes[1][ax]) == abs(out_Axes[2][ax]) );
      }
   }
   else
   {
      FOREACHAXIS( ax )
         out_Axes[1][ax] = in_Up[ax];
   }

   normalize( out_Axes[2], 3 );
   cross( out_Axes[1], out_Axes[2], out_Axes[0] );
   normalize( out_Axes[0], 3 );
   cross( out_Axes[2], out_Axes[0], out_Axes[1] );
}

template <class T>
void AWT::MeshFunctions<T>::calculatePrincipalAxesGivenZ(const typename AWT::Mesh<T>::P mesh, T in_Z[3], T out_Axes[3][3] )
{
   // Firstly, let's get the centroid
   T mu[3], vtx[3], vtxP[3];
   calculateFaceCentroid( mesh, mu );

   //PRINTVEC( mu, 3 );

   // Let's also construct an arbitrary coordinate system so that we can project down to its x-y plane
   // Need to pick a y-direction
   T arbCoords[3][3];
   constructOrthonormalAxes( in_Z, 0, arbCoords );

   T iarb[3][3], imu[3];
   unprojectParameters( arbCoords, mu, iarb, imu );

   //PRINTVEC( iarb[0], 3 );
   //PRINTVEC( iarb[1], 3 );
   //PRINTVEC( iarb[2], 3 );
   //BLANKLINE;

   vnl_matrix<T> A(3,3);
   A.fill( 0 );

   MESH_EACHVERTEX( mesh, v )
   {
      mesh->getVertex( v, vtx );

      project( vtx, iarb, imu, vtxP );

      vtxP[2] = 0;

      FOREACHAXIS( i )
         FOREACHAXIS( j )
               A( i, j ) += vtxP[i]*vtxP[j];
   }

   vnl_symmetric_eigensystem<T> eig( A );

   //PRINTVEC( arbCoords[0], 3 );
   //PRINTVEC( arbCoords[1], 3 );
   //PRINTVEC( arbCoords[2], 3 );
   //PRINTVEC( mu, 3 );
   //BLANKLINE;

   T origin[] = { 0, 0, 0 };

   FOREACHAXIS( ax )
   {
      vnl_vector<T> vec = eig.get_eigenvector( 2-ax );
      
      {
         FOREACHAXIS( ax )
            vtxP[ax] = vec(ax);
      }

      //PRINTVEC( vtxP, 3 );
      project( vtxP, arbCoords, origin, out_Axes[ax] );
      //PRINTVEC( out_Axes[ax], 3 );
      //BLANKLINE;
   }

   //PRINTVEC( out_Axes[0], 3 );
   //PRINTVEC( out_Axes[1], 3 );
   //PRINTVEC( out_Axes[2], 3 );
   //BLANKLINE;

   if ( det3x3( out_Axes ) < 0 )
   {
      //DEBUGMACRO( "Flipping y" );
      FOREACHAXIS( ax )
         out_Axes[1][ax] *= -1;
   }

   //PRINTVEC( out_Axes[0], 3 );
   //PRINTVEC( out_Axes[1], 3 );
   //PRINTVEC( out_Axes[2], 3 );
   //BLANKLINE;
}

template <class T>
void AWT::MeshFunctions<T>::transformMesh( typename AWT::Mesh<T>::P mesh, vnl_matrix_fixed<T,4,4> t )
{
   vnl_vector_fixed<T,4> vec;
   
   TuplesFunctions<T>::transformPoints( mesh->getVertices( ), t, false );

   if ( mesh->hasVertexNormals( ) )
      TuplesFunctions<T>::transformPoints( mesh->getVertexNormals( ), t, true );

   if ( mesh->hasFaceNormals( ) )
      TuplesFunctions<T>::transformPoints( mesh->getFaceNormals( ), t, true );

   if ( vnl_det<T>( t ) < 0 )
   {
      MeshIndex vs[3];
      MeshIndex tmp;

      MESH_EACHFACE( mesh, f )
      {
         mesh->getFaceIndices( f, vs );

         // Reverse the order of the triple - i.e. swap vs[0] and vs[2]
         tmp   = vs[0];
         vs[0] = vs[2];
         vs[2] = tmp;

         mesh->setFaceIndices( f, vs );
      }
   }
}

template <class T>
void AWT::MeshFunctions<T>::untransformMesh( typename AWT::Mesh<T>::P mesh, const T in_R[3][3], const T in_t[3] )
{
   T iR[3][3], iT[3];

   unprojectParameters( in_R, in_t, iR, iT );
   transformMesh( mesh, iR, iT );
}

template <class T>
void AWT::MeshFunctions<T>::transformMesh( typename AWT::Mesh<T>::P mesh, const T in_R[3][3], const T in_t[3] )
{
   vnl_matrix_fixed<T,4,4> t;
   t.fill( 0 );

   for ( unsigned int r = 0; r < 3; ++r )
   {
      for ( unsigned int c = 0; c < 3; ++c )
         t(r,c) = in_R[r][c];

      t(r,3) = in_t[r];
   }
   t(3,3) = 1;

   transformMesh( mesh, t );
}

template <class T>
typename AWT::Tuples<T>::P AWT::MeshFunctions<T>::calculatePrincipalCurvature( const typename AWT::Mesh<T>::P mesh )
{
   typename Tuples<T>::P ret = TuplesImpl<T>::getInstance( 1, mesh->getNumberOfVertices( ) );

   if ( !mesh->hasVertexNormals( ) )
   {
      AreaAveragedNormalCalculator<T>::getInstance( )->calculateNormalsAndSet( mesh );
   }

   Tuples<T>::P nmls = mesh->getVertexNormals( );
   MeshConnectivity<T>::P conn = MeshConnectivity<T>::getInstance( mesh );

   T vtx[3], vtxAdj[3];
   T nml[3], nmlAdj[3];

   MESH_EACHVERTEX( mesh, v )
   {
      typename Container::Iterator<MeshIndex>::P it = conn->getAdjacentVertices( v );

      mesh->getVertex( v, vtx );
      nmls->getPoint( v, nml );

      T bestCurvature = 0;

      while ( it->hasNext( ) )
      {
         MeshIndex vn = it->next( );

         mesh->getVertex( vn, vtxAdj );
         nmls->getPoint( vn, nmlAdj );

         T num = 0;
         T den = 0;

         FOREACHAXIS( ax )
         {
            vtxAdj[ax] -= vtx[ax];

            num += vtxAdj[ax]*( nmlAdj[ax] - nml[ax] );
            den += vtxAdj[ax]*vtxAdj[ax];
         }

         bestCurvature = std::max<T>( bestCurvature, num/den );
      }

      bestCurvature = log(1/bestCurvature);

      ret->setPoint( v, &bestCurvature );

      //DEBUGMACRO( v << "\t" << bestCurvature );
   }

   return ret;

   typename Tuples<T>::P rett = TuplesImpl<T>::getInstance( 1, mesh->getNumberOfVertices( ) );
   MESH_EACHVERTEX( mesh, v )
   {
      typename Container::Iterator<MeshIndex>::P it = conn->getAdjacentVertices( v );

      T curv = ret->getPointElement( v, 0 );
      T minCurv = curv;

      while ( it->hasNext( ) )
      {
         MeshIndex vadj = it->next( );
         minCurv = std::min<T>( minCurv, ret->getPointElement( vadj, 0 ) );

         //PRINTVBL( ret->getPointElement( vadj, 0 ) );
      }
      //BLANKLINE;
      //PRINTVBL( curv );
      //PRINTVBL( minCurv );
      //PAUSE;

      if ( curv > minCurv )
      {
         rett->setPointElement( v, 0, 0 );
      }
      else
      {
         DEBUGLINE;
         rett->setPointElement( v, 0, 1 );
      }
   }

   return rett;
}

#include <set>
#include <map>

template <class T>
int AWT::MeshFunctions<T>::getEulerCharacteristic(const typename AWT::Mesh<T>::P mesh)
{
   // Need to count the edges, without counting joined edges twice
   typedef std::pair< MeshIndex, MeshIndex > Edge;
   typedef std::set< Edge > EdgeSet;

   EdgeSet edges;

   MeshIndex vs[3];
   MESH_EACHFACE( mesh, f )
   {
      mesh->getFaceIndices( f, vs );

      for ( int ax = 0; ax < 3; ++ax )
      {
         if ( vs[ax] < vs[(ax+1)%3] )
            edges.insert( Edge( vs[ax], vs[(ax+1)%3] ) );
         else
            edges.insert( Edge( vs[(ax+1)%3], vs[ax] ) );
      }
   }

   const int e = static_cast<int>( edges.size( ) );
   const int v = mesh->getNumberOfVertices( );
   const int f = mesh->getNumberOfFaces( );

   PRINTVBL( e );

   // This is the Euler characteristic
   return v - e + f;
}

#include <fstream>

template <class T>
void AWT::MeshFunctions<T>::fixHoles(typename AWT::Mesh<T>::P mesh)
{
   typedef std::pair< MeshIndex, MeshIndex > Edge;
   typedef std::map< MeshIndex, MeshIndex > EdgeMap;

   EdgeMap edgemap;

   MeshConnectivity<T>::P conn = MeshConnectivity<T>::getInstance( mesh );

   //std::ofstream os( "links.csv" );

   MeshIndex cnt = 0;
   MeshIndex vs[3];
   MESH_EACHFACE( mesh, f )
   {
      mesh->getFaceIndices( f, vs );

      for ( unsigned int ax = 0; ax < 3; ++ax )
      {
         MeshIndex fue = conn->getFacesUsingEdge( vs[ax], vs[(ax+1)%3], 0 );

         if ( fue != 2 )
         {
            // Push the /backwards/ edge
            edgemap[ vs[(ax+1)%3] ] = vs[ax];
            DEBUGMACRO( vs[(ax+1)%3] << "\t" << vs[ax]  );

            //os << vs[(ax+1)%3] << "," << vs[ax] << std::endl;

            if ( fue != 1 )
               PAUSE;

            ++cnt;
         }
      }
   }
   PRINTVBL( cnt );
   //os.close( );
   //DEBUGMACRO( "Data written to links.csv" );
   //PAUSE;

   typedef std::vector<MeshIndex> Ring;

   MeshIndex cntNotTriangle = 0, cntTriangle = 0;
   while ( !edgemap.empty() )
   {
      Ring ring;

      const MeshIndex ringstart = edgemap.begin( )->first;
      MeshIndex v = edgemap.begin( )->second;

      ring.push_back( ringstart );

      edgemap.erase( ringstart );
      
      while ( ringstart != v )
      {
         // Push it into this ring
         ring.push_back( v );

         // Move onto the next one
         const MeshIndex nextv = edgemap[ v ];
         edgemap.erase( v );
         v = nextv;
      }

      if ( ring.size( ) != 3 )
         ++cntNotTriangle;
      else
      {
         ++cntTriangle;
         MeshIndex vs[] = { ring[0], ring[1], ring[2] };
         mesh->setFaceIndices( mesh->getNumberOfFaces( ), vs );

         for ( Ring::const_iterator it = ring.begin( ), itend = ring.end( ); it != itend; ++it )
         {
            std::cerr << *it << " ";
         }
         BLANKLINE;
         //PAUSE;
      }

      
   }

   PRINTVBL( cntTriangle );
   PRINTVBL( cntNotTriangle );
   PRINTVBL2( "Open Edges", cnt );
}

template <class T>
T AWT::MeshFunctions<T>::meshToMeshDistance( typename AWT::Mesh<T>::P meshA, typename AWT::Mesh<T>::P meshB, const bool surface )
{
   // Checking to ensure that the meshes have the same connectivity
   if ( meshA->getNumberOfVertices() != meshB->getNumberOfVertices() )
      AWTEXCEPTIONTHROW( "Meshes must have same number of vertices" );

   if ( surface )
   {
      if ( meshA->getNumberOfFaces() != meshB->getNumberOfFaces() )
         AWTEXCEPTIONTHROW( "Meshes must have same number of faces" );

      // Now check all triangles to ensure that they are the same
      MeshIndex faceA[3], faceB[3];
      MESH_EACHFACE( meshA, f )
      {
         meshA->getFaceIndices( f, faceA );
         meshB->getFaceIndices( f, faceB );

         FOREACHAXIS( ax )
            if ( faceA[ax] != faceB[ax] )
               AWTEXCEPTIONTHROW( "Face indices are different" );
      }

      // Right, the meshes have the same connectivity.

      // It's quite east to calculate analytically
      T vtxA[3][3];
      T vtxB[3][3];
      T vtxC[3][3];

      T ret = 0;
      T surfaceArea = 0;
      MESH_EACHFACE( meshA, f )
      {
         meshA->getFace( f, vtxA[0], vtxA[1], vtxA[2] );
         meshB->getFace( f, vtxB[0], vtxB[1], vtxB[2] );

         // Calculate the mean triangle, in order that this
         // measure can be symmetrical
         FOREACHAXIS( i )
            FOREACHAXIS( ax )
               vtxC[i][ax] = ( vtxB[i][ax] + vtxA[i][ax] ) / 2;

         FOREACHAXIS( ax )
         {
            vtxC[2][ax] -= vtxC[0][ax];
            vtxC[1][ax] -= vtxC[0][ax];
         }
         cross<T>( vtxC[1], vtxC[2], vtxC[0] );
         const T faceArea = normalize<T>( vtxC[0], 3 ) / 2;

         surfaceArea += faceArea;

         // Integration is done over triangle [a>=0, b>=0, a+b<=1]
         // using result from Wolfram|Alpha (pretty tedious to do by hand):-
         //     Integrate[ Integrate[ (p + a*q + b*r)^2, {a, 0, 1-b} ], {b, 0, 1} ]

         // This scales the integration so that it's in the object space instead
         const T jacobian = faceArea;

         // The contribution by each axis is independent
         FOREACHAXIS( ax )
         {
            const T p = vtxB[0][ax] - vtxA[0][ax];
            const T q = (vtxB[1][ax]-vtxB[0][ax]) - (vtxA[1][ax]-vtxA[0][ax]);
            const T r = (vtxB[2][ax]-vtxB[0][ax]) - (vtxA[2][ax]-vtxA[0][ax]);

            ret += ( 6*p*p + 4*p*(q+r) + q*q + q*r + r*r ) / 12 * jacobian;
         }
      }

      // Normalize it by the total surface area to get the standard error
      return sqrt( ret / surfaceArea );
   }
   else
   {
      // Just look at vertices
      T vtxA[3];
      T vtxB[3];

      T ret = 0;
      MESH_EACHVERTEX( meshA, v )
      {
         meshA->getVertex( v, vtxA );
         meshB->getVertex( v, vtxB );

         ret += deltaNormSquared<T>( vtxA, vtxB, 3 );
      }

      // Normalize it by the total surface area to get the standard error
      return sqrt( ret / meshA->getNumberOfVertices() );
   }
}

template class AWT::MeshFunctions<double>;
template class AWT::MeshFunctions<float>;