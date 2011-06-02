#include "FaceQueries.h"

#include "ObjectEKDTree/OEKDTreeData.h"
#include "FacesTreeData.h"

#include "Useful/ArrayVectorFunctions.h"

template <class T>
void AWT::FaceQueries<T>::getCentroid( const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index, T* out_Centroid )
{
   const FacesTreeData<T>* data = reinterpret_cast<const FacesTreeData<T>*>( in_Data.getDataConst( ) );

   T point0[3];
   T point1[3];
   T point2[3];

   data->getVertex( in_Index, 0, point0 );
   data->getVertex( in_Index, 1, point1 );
   data->getVertex( in_Index, 2, point2 );

   for ( int i = 0; i < 3; ++i )
      out_Centroid[i] = ( point0[i] + point1[i] + point2[i] ) / 3;
}

template <class T>
void AWT::FaceQueries<T>::getNormal( const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index, T* out_Normal )
{
   const FacesTreeData<T>* data = reinterpret_cast<const FacesTreeData<T>*>( in_Data.getDataConst( ) );

   T point0[3];
   T point1[3];
   T point2[3];

   data->getVertex( in_Index, 0, point0 );
   data->getVertex( in_Index, 1, point1 );
   data->getVertex( in_Index, 2, point2 );

   for ( int i = 0; i < 3; i++ )
   {
      point1[i] = point1[i] - point0[i];
      point2[i] = point2[i] - point0[i];
   }

   cross( &point1[0], &point2[0], &out_Normal[0] );
   normalize<T>( out_Normal, 3 );
}

template <class T>
void AWT::FaceQueries<T>::getNearestPointOnTriangle( const T* in_TestPoint, const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index, T* out_Nearest )
{
   const FacesTreeData<T>* data = reinterpret_cast<const FacesTreeData<T>*>( in_Data.getDataConst( ) );

   T point0[3];
   T point1[3];
   T point2[3];

   data->getVertex( in_Index, 0, point0 );
   data->getVertex( in_Index, 1, point1 );
   data->getVertex( in_Index, 2, point2 );

   getNearestPointOnTriangle( in_TestPoint, point0, point1, point2, out_Nearest );
}

template <class T>
void AWT::FaceQueries<T>::getNearestPointOnTriangle( const T* in_TestPoint, const T* _point0, const T* _point1, const T* _point2, T* out_Nearest )
{
   T point1[3];
   T point2[3];

   for ( int i = 0; i < 3; i++ )
   {
      point1[i] = _point1[i] - _point0[i];
      point2[i] = _point2[i] - _point0[i];
   }

   T p[3] = {
      in_TestPoint[0] - _point0[0], 
      in_TestPoint[1] - _point0[1],
      in_TestPoint[2] - _point0[2],
   };

   T nml[3];
   cross( &point1[0], &point2[0], &nml[0] );
   const T len = normalize<T>( nml, 3 );

   // Check if this cell is degenerate
   if ( len == 0 || nml[0] != nml[0]  ||  nml[1] != nml[1]  ||  nml[2] != nml[2] )
   {
      // This is a cop-out - must fix this at some point!
      out_Nearest[0] = out_Nearest[1] = out_Nearest[2] = std::numeric_limits<T>::infinity( );
      return;
   }

   const T nml_dot_p = dot<T>( &nml[0], &p[0], 3 );
   const T np[3] = {
      p[0] - nml_dot_p * nml[0],
      p[1] - nml_dot_p * nml[1],
      p[2] - nml_dot_p * nml[2]
   };

   const T vec0[3] = { point1[0], point1[1], point1[2] };
   const T vec1[3] = { point2[0], point2[1], point2[2] };
   const T vec2[3] = { point2[0]-point1[0], point2[1]-point1[1], point2[2]-point1[2] };

   const T nsqvec[3] = {
      dot<T>( &vec0[0], &vec0[0],3 ),
      dot<T>( &vec1[0], &vec1[0],3 ),
      dot<T>( &vec2[0], &vec2[0],3 )
   };

   const T lams[3] = {
      dot<T>( &vec0[0], &np[0],3 ),
      dot<T>( &vec1[0], &np[0],3 ),
      dot<T>( &vec2[0], &np[0],3 ) - dot<T>( &vec2[0], &vec0[0],3 )
   };

   int resultCase;

   // The first 3 cases are where the nearest point is a vertex
   if ( lams[0] < 0 && lams[1] < 0 )
   {
      out_Nearest[0] = 0;
      out_Nearest[1] = 0;
      out_Nearest[2] = 0;
      resultCase = 1;
   }
   else if ( lams[0] > nsqvec[0] && lams[2] < 0 )
   {
      out_Nearest[0] = point1[0];
      out_Nearest[1] = point1[1];
      out_Nearest[2] = point1[2];
      resultCase = 2;
   }
   else if ( lams[1] > nsqvec[1] && lams[2] > nsqvec[2] )
   {
      out_Nearest[0] = point2[0];
      out_Nearest[1] = point2[1];
      out_Nearest[2] = point2[2];
      resultCase = 3;
   }
   else
   {
      // This is the determinant of [ vec0 vec1 nml ]
      const T det =   vec0[0] * ( vec1[1]*nml[2] - nml[1]*vec1[2] )
              - vec0[1] * ( vec1[0]*nml[2] - nml[0]*vec1[2] )
              + vec0[2] * ( vec1[0]*nml[1] - nml[0]*vec1[1] );

      // This is the adjoint of [ vec0 vec1 nml ]
      const T mat[3][3] = {
         {  (vec1[1]*nml [2]-nml [1]*vec1[2]), -(vec1[0]*nml [2]-nml [0]*vec1[2]),  (vec1[0]*nml [1]-nml [0]*vec1[1]) },
         { -(vec0[1]*nml [2]-nml [1]*vec0[2]),  (vec0[0]*nml [2]-nml [0]*vec0[2]), -(vec0[0]*nml [1]-nml [0]*vec0[1]) },
         {  (vec0[1]*vec1[2]-vec1[1]*vec0[2]), -(vec0[0]*vec1[2]-vec1[0]*vec0[2]),  (vec0[0]*vec1[1]-vec1[0]*vec0[1]) }
      };

      const T proj[3] = {
         ( mat[0][0]*np[0] + mat[0][1]*np[1] + mat[0][2]*np[2] ) / det,
         ( mat[1][0]*np[0] + mat[1][1]*np[1] + mat[1][2]*np[2] ) / det,
         ( mat[2][0]*np[0] + mat[2][1]*np[1] + mat[2][2]*np[2] ) / det,
      };

      if ( lams[0] >= 0 && lams[0] <= nsqvec[0] && proj[1] < 0 )
      {
         T mult = lams[0] / nsqvec[0];

         out_Nearest[0] = 0 + mult * vec0[0];
         out_Nearest[1] = 0 + mult * vec0[1];
         out_Nearest[2] = 0 + mult * vec0[2];
         resultCase = 4;
      }
      else if ( lams[1] >= 0 && lams[1] <= nsqvec[1] && proj[0] < 0 )
      {
         T mult = lams[1] / nsqvec[1];

         out_Nearest[0] = 0 + mult * vec1[0];
         out_Nearest[1] = 0 + mult * vec1[1];
         out_Nearest[2] = 0 + mult * vec1[2];
         resultCase = 5;
      }
      else if ( (proj[0]+proj[1]) > 1 )
      {
         T mult = lams[2] / nsqvec[2];

         out_Nearest[0] = point1[0] + mult * vec2[0];
         out_Nearest[1] = point1[1] + mult * vec2[1];
         out_Nearest[2] = point1[2] + mult * vec2[2];
         resultCase = 6;
      }
      else
      {
         out_Nearest[0] = np[0];
         out_Nearest[1] = np[1];
         out_Nearest[2] = np[2];
         resultCase = 0;
      }
   }

   out_Nearest[0] += _point0[0];
   out_Nearest[1] += _point0[1];
   out_Nearest[2] += _point0[2];
}

template <class T>
bool AWT::FaceQueries<T>::rayIntersectsTriangle( const typename OEKDTree::OEKDTreeData<T,3>::P in_Data, const int in_Index, const T* origin, const T* direction, T* intersectionPoint, T* distance )
{
   const FacesTreeData<T>* data = reinterpret_cast<const FacesTreeData<T>*>( in_Data.getDataConst( ) );

   T offset[3];
   T point1[3];
   T point2[3];

   data->getVertex( in_Index, 0, offset );
   data->getVertex( in_Index, 1, point1 );
   data->getVertex( in_Index, 2, point2 );

   for ( int i = 0; i < 3; i++ )
   {
      point1[i] -= offset[i];
      point2[i] -= offset[i];
   }

   T nml[3];
   cross( point1, point2, nml );

   // If ||nml|| == 0, then the case is degenerate
   if ( dot<T>( nml, nml, 3 ) == 0 )
   {
      return false;
   }

   normalize<T>( nml, 3 );

   T denom = dot<T>( nml, direction, 3 );

   // denom = 0 => line is perp to triangle normal so doesn't intersect
   if ( denom == 0 )
      return false;

   T delta[3];
   delta[0] = offset[0] - origin[0];
   delta[1] = offset[1] - origin[1];
   delta[2] = offset[2] - origin[2];

   const T lambda = dot<T>( delta, nml, 3 ) / denom;
   /*if ( lambda < 0 )
      return false;*/

   T np[3];
   np[0] = origin[0] + lambda * direction[0];
   np[1] = origin[1] + lambda * direction[1];
   np[2] = origin[2] + lambda * direction[2];

   // OK: out_IntersectionPoint now lies in the plane of the triangle.
   // the question is, does it lie within the boundary of the triangle...

   // Define these to make the code a bit prettier
   T* vec0 = point1;
   T* vec1 = point2;

   // This is the determinant of [ vec0 vec1 nml ]
   T det =   vec0[0] * ( vec1[1]*nml[2] - nml[1]*vec1[2] )
           - vec0[1] * ( vec1[0]*nml[2] - nml[0]*vec1[2] )
           + vec0[2] * ( vec1[0]*nml[1] - nml[0]*vec1[1] );

   // This is the adjoint of [ vec0 vec1 nml ]
   T mat[3][3] = {
      {  (vec1[1]*nml[2]-nml[1]*vec1[2]),   -(vec1[0]*nml[2]-nml[0]*vec1[2]),    (vec1[0]*nml[1]-nml[0]*vec1[1])   },
      { -(vec0[1]*nml[2]-nml[1]*vec0[2]),    (vec0[0]*nml[2]-nml[0]*vec0[2]),   -(vec0[0]*nml[1]-nml[0]*vec0[1])   },
      {  (vec0[1]*vec1[2]-vec1[1]*vec0[2]), -(vec0[0]*vec1[2]-vec1[0]*vec0[2]),  (vec0[0]*vec1[1]-vec1[0]*vec0[1]) }
   };

   T proj[3];
   
   np[0] -= offset[0];
   np[1] -= offset[1];
   np[2] -= offset[2];

   // This code works, but to make it more efficient, certain things don't need to be calculated
   proj[0] = ( mat[0][0]*np[0] + mat[0][1]*np[1] + mat[0][2]*np[2] ); // / det;
   proj[1] = ( mat[1][0]*np[0] + mat[1][1]*np[1] + mat[1][2]*np[2] ); // / det;
   //proj[2] = ( mat[2][0]*np[0] + mat[2][1]*np[1] + mat[2][2]*np[2] ) / det;

   if ( proj[0] < 0 || proj[0] > det/*1*/ || proj[1] < 0 || proj[1] > det/*1*/ || (proj[0] + proj[1]) > det/*1*/ )
      return false;

   double nearestPoint[3];

   nearestPoint[0] = origin[0] + lambda * direction[0];
   nearestPoint[1] = origin[1] + lambda * direction[1];
   nearestPoint[2] = origin[2] + lambda * direction[2];

   if ( distance != 0 )
      *distance = abs( lambda );

   return true;
}

template class AWT::FaceQueries<double>;
template class AWT::FaceQueries<float>;