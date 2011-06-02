#include "Sampler.h"
#include "Useful/ColouredConsole.h"

#include "MeshFunctions.h"
#include "Useful/Noise.h"

using namespace AWT::SimpleMesh;

AWT::SimpleMesh::Sampler::Sampler( Mesh::P mesh )
{
   this->mesh = mesh;

   cumAreas = new double[ mesh->nf ];
   for ( Index i = 0; i < mesh->nf; ++i )
      cumAreas[i] = 0;
}

AWT::SimpleMesh::Sampler::~Sampler( )
{
   delete [] cumAreas;
}

Mesh::P AWT::SimpleMesh::Sampler::getMesh( )
{
   return mesh;
}

AWT::SimpleMesh::PointIndexWeights AWT::SimpleMesh::Sampler::sample( )
{
   PointIndexWeights ret;

   double v = Noise<double>::randu( 0, 1 );

   // Now, need to work out which face this falls on
   Index lower = 0;
   Index upper = mesh->nf;

   while ( upper - lower > 1 )
   {
      Index mid = ( lower + upper ) / 2;

      if ( cumAreas[mid] >= v )
         upper = mid;
      else
         lower = mid;
   }

   while ( cumAreas[lower] == cumAreas[lower+1] )
      ++lower;

   //PRINTVBL( v );
   //DEBUGMACRO( lower+0 << ": " << cumAreas[lower+0] );
   //DEBUGMACRO( lower+1 << ": " << cumAreas[lower+1] );
   //BLANKLINE;

   ret.i = lower;

   // Now, pick a point on this triangle
   ret.w(0) = Noise<double>::randu( 0, 1 );
   ret.w(1) = Noise<double>::randu( 0, 1 );

   if ( ret.w(0) + ret.w(1) > 1 )
   {
      ret.w(0) = 1-ret.w(0);
      ret.w(1) = 1-ret.w(1);
   }

   Point vs[3];
   getFaceVertices( mesh, ret.i, vs );
   
   ret.p = ( 1 - ret.w(0) - ret.w(1) )*vs[0] + ret.w(0)*vs[1] + ret.w(1)*vs[2];
   
   return ret;
}
