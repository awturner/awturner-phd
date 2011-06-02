
#include "SimpleMesh/Connectivity.h"
#include "SimpleMesh/FacesInRadius.h"
#include "SimpleMesh/FacesIntersectPlane.h"
#include "SimpleMesh/Mesh.h"
#include "SimpleMesh/MeshFunctions.h"
#include "SimpleMesh/MeshIO.h"
#include "SimpleMesh/Search.h"
#include "SimpleMesh/UniformSampler.h"
#include "SimpleMesh/VerticesBetweenPlanes.h"
#include "SimpleMesh/VerticesInCylinder.h"
#include "SimpleMesh/VerticesInRadius.h"

#include "Useful/ArrayVectorFunctions.h"
#include "Useful/Noise.h"

#include <set>
#include <fstream>
#include <algorithm>

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_matlab_filewrite.h"

typedef AWT::Noise<AWT::SimpleMesh::Coordinate> random;

using namespace AWT::SimpleMesh;

typedef std::vector<IndexLine> IndexLines;

struct IntersectionPoint
{
   Point p;
   Index vs[2];
};
typedef std::vector<IntersectionPoint> IntersectionPoints;

struct FPCSTarget
{
   Coordinate length;
   Coordinate ratio;
   Coordinate dihedral;
};

struct FPCSTargets
{
   FPCSTarget targets[2];
   Coordinate sinAngle;
};

const double PI = atan(1.0) * 4;
const double DEG_TO_RAD = PI / 180.0;

double calculateSinAngle( const Point line1[2], const Point line2[2] )
{
   const Point delta[] = { (line1[1] - line1[0]).normalize(), (line2[1] - line2[0]).normalize() };

   Point crossVec;
   AWT::cross<Coordinate>( delta[0].data_block(), delta[1].data_block(), crossVec.data_block() );
   crossVec[3] = 0;

   return crossVec.magnitude();
}

void findExtrema( Points& points, const Point& axis, Point extrema[2] )
{
   Coordinate extremes[] = { FAR_FAR_AWAY, -FAR_FAR_AWAY };
   Point extremePnt[2];

   for (Index col = 0; col < points.cols(); ++col)
   {
      Point mu = points.get_column(col);
      Coordinate c = dot( axis, mu, 4 );

      if (c < extremes[0])
      {
         extremes[0] = c;
         extrema[0] = mu;
      }

      if (c > extremes[1])
      {
         extremes[1] = c;
         extrema[1] = mu;
      }
   }
}

void calculateFPCSTargets( Points& pts, Points& nmls, FPCSTargets& tgts )
{
   Point delta[] = { pts.get_column(1)-pts.get_column(0), pts.get_column(3)-pts.get_column(2) };
   tgts.targets[0].length = delta[0].magnitude();
   tgts.targets[1].length = delta[1].magnitude();

   // Calculate where these lines cross
   // Assume that they are truly planar
   Point a = pts.get_column(0);
   Point b = pts.get_column(1) - a;
   Point c = pts.get_column(2) - a;
   Point d = pts.get_column(3) - a;

   Coordinate lambda = ( dot(c,b,3) + dot(d,b,3) ) / ( 2 * dot(b,b,3) );
   Point x = a + lambda * b;

   tgts.targets[0].ratio = ( x - pts.get_column(0) ).magnitude() / ( pts.get_column(1) - pts.get_column(0) ).magnitude();
   tgts.targets[1].ratio = ( x - pts.get_column(2) ).magnitude() / ( pts.get_column(3) - pts.get_column(2) ).magnitude();

   // Store the cos of the dihedral angle
   tgts.targets[0].dihedral = inner_product(nmls.get_column(1), nmls.get_column(0));
   tgts.targets[1].dihedral = inner_product(nmls.get_column(3), nmls.get_column(2));

   // Also store the angle between the lines as a discriminator
   delta[0].normalize();
   delta[1].normalize();

   Point crossVec;
   AWT::cross<Coordinate>( delta[0].data_block(), delta[1].data_block(), crossVec.data_block() );
   crossVec[3] = 0;

   tgts.sinAngle = crossVec.magnitude();

}

Mesh::P sampleMesh( Mesh::P model, const Index nsamples, Points& normals )
{
   Mesh::P ret = Mesh::getInstance(nsamples,0);
   UniformSampler::P sampler = UniformSampler::getInstance( model );

   normals.set_size(4,nsamples);

   Matrix& vs = ret->getVertices();
   for ( Index i = 0; i < nsamples; ++i )
   {
      PointIndexWeights piw = sampler->sample();
      vs.set_column(i,piw.p);

      normals.set_column(i, calculateFaceNormal(model, piw.i, true));
   }

   return ret;
}

void pickCoplanarPoints( Search::P searcher, Points& pts, Points& nmls )
{
   Mesh::P mesh = searcher->getMesh();

   // Calculate the mesh's centroid
   Point centroid = calculateCentroid( mesh );

   // Pick a random normal
   Point plane;
   for (Index ax = 0; ax < 3; ++ax)
      plane(ax) = random::randu(-1,1);
   
   plane(3) = -dot( plane, centroid, 3 );

   FacesIntersectPlane::P fip = FacesIntersectPlane::getInstance();
   fip->setTestPlane(plane);

   searcher->search(fip);

   typedef std::vector<IndexLine> IndexLines;
   IndexLines& faces = fip->getFaces();

   Index col;
   Matrix p[2] = { Matrix(4, faces.size()), Matrix(4, faces.size()) };

   Matrix covar(4,4);
   Point sumx;
   Coordinate sumw;

   Points mus(4, faces.size());

   covar.fill(0);
   sumx.fill(0);
   sumw = 0;

   col = 0;
   for (IndexLines::iterator it = faces.begin(), en = faces.end(); it != en; ++it, ++col)
   {
      Point mu = (it->p[0]+it->p[1]) * 0.5;
      mus.set_column(col, mu);

      mu(3) = 0;

      Coordinate len = (it->p[0]-it->p[1]).magnitude();

      covar += outer_product(mu, mu) * len;
      sumx  += mu * len;
      sumw  += len;
   }

   // Just pick three points
   pts.set_size(4,4);
   pts.set_column( 0, mus.get_column(rand()%mus.cols()) );
   pts.set_column( 1, mus.get_column(rand()%mus.cols()) );

   pts.set_column( 2, pts.get_column(0) );
   pts.set_column( 3, mus.get_column(rand()%mus.cols()) );
}

Transformation absoluteOrientation( const Points& ps, const Points& qs )
{
   Matrix covar(4,4, 0);
   Point  sum_p; sum_p.fill(0);
   Point  sum_q; sum_q.fill(0);
   Coordinate var_p(0);
   Coordinate var_q(0);

   const Index N = ps.cols();
   const Coordinate NN = static_cast<Coordinate>(N);

   for ( Index i = 0; i < N; ++i )
   {
      Point pc = ps.get_column(i);
      Point qc = qs.get_column(i);

      pc(3) = qc(3) = 0;

      covar += outer_product<Coordinate>( pc, qc );

      sum_p += pc;
      sum_q += qc;

      var_p += inner_product<Coordinate>(pc, pc);
      var_q += inner_product<Coordinate>(qc, qc);
   }

   // Correct these quantities
   covar -= outer_product<Coordinate>(sum_p, sum_q) / static_cast<Coordinate>(N);
   var_p -= inner_product<Coordinate>(sum_p, sum_p) / static_cast<Coordinate>(N);
   var_q -= inner_product<Coordinate>(sum_q, sum_q) / static_cast<Coordinate>(N);

   // Calculate the scale
   const Coordinate sc = sqrt( var_p / var_q );

   // Calculate the (scaled) rotation
   vnl_svd<Coordinate> svd( covar );
   Transformation R = sc * svd.V() * svd.U().transpose(); R(3,3) = 1;

   // Calculate the translation
   Point t = -( R*sum_p - sum_q ) / static_cast<Coordinate>(N);
   t(3) = 1;
   R.set_column(3,t);

   return R;
}

int main(int argc, char** argv)
{
   const Index nmodelSamples = 10000;
   const Index nmeshSamples = 50;
   const double delta = 3;
   const double dtheta = 3*DEG_TO_RAD;

   random::timeSeed();

   DEBUGMACRO( "Loading mesh from " << argv[1] );
   Mesh::P mesh = MeshIO::loadMesh( std::ifstream(argv[1]) );

   DEBUGMACRO( "Loading model from " << argv[2] );
   ShapeModel::P shapeModel = MeshIO::loadModel( std::ifstream(argv[2]) );

   DEBUGMACRO( "Constructing searchers..." );
   Search::P faceSearcher = Search::getInstance(mesh, true);

   Points quad, nmls;
   FPCSTargets targets;

   do
   {
      DEBUGMACRO( "Picking coplanar points..." );
      pickCoplanarPoints( faceSearcher, quad, nmls );
      calculateFPCSTargets( quad, nmls, targets );
   }
   while (targets.sinAngle < 0.707);

   PRINTVBL( quad );
DEBUGLINE;   
   // Construct a vertex searcher for the model mesh
   // Create an instance of the shape model
   Vector ws(shapeModel->getNumberOfModes());
DEBUGLINE;
   ws.fill(0);
DEBUGLINE;
   ws[0] = 1;
DEBUGLINE;
PRINTVBL(shapeModel->getNumberOfModes());
   Mesh::P shapeModelInst = shapeModel->getMesh(ws);
DEBUGLINE;
   // This is used to check the support
   Search::P modelVertexSearcher = Search::getInstance(shapeModelInst, false);
DEBUGLINE;
   // Now draw samples from it and prepare to search it
   Points normals;
   Mesh::P   modelSampleMesh     = sampleMesh(shapeModelInst, nmodelSamples, normals);
   Search::P modelSampleSearcher = Search::getInstance(modelSampleMesh, false);
DEBUGLINE;
   VerticesInRadius::P vir = VerticesInRadius::getInstance( );
   vir->setInnerRadius( targets.targets[0].length - delta/2 );
   vir->setOuterRadius( targets.targets[0].length + delta/2 );
DEBUGLINE;
   VerticesInCylinder::P vic = VerticesInCylinder::getInstance( );
DEBUGLINE;
   // Calculate the cylindrical bounds
   double rs[] = { FAR_FAR_AWAY, -FAR_FAR_AWAY };
   double zs[] = { FAR_FAR_AWAY, -FAR_FAR_AWAY };

   const double theta = asin(targets.sinAngle);

   const double dr[] = { targets.targets[1].length + delta/2, targets.targets[1].length - delta/2 };
   const double dt[] = { theta + dtheta / 2, theta - dtheta / 2 };
DEBUGLINE;
   for ( Index rr = 0; rr < 2; ++rr )
   {
      for ( Index zz = 0; zz < 2; ++zz )
      {
         for ( Index dti = 0; dti < 2; ++dti )
         {
            for ( Index dri = 0; dri < 2; ++dri )
            {
               rs[0] = std::min(rs[0], dr[dri] * sin(dt[dti]));
               rs[1] = std::max(rs[1], dr[dri] * sin(dt[dti]));

               zs[0] = std::min(zs[0], dr[dri] * cos(dt[dti]));
               zs[1] = std::max(zs[1], dr[dri] * cos(dt[dti]));
            }
         }
      }
   }
DEBUGLINE;
   vic->setRadius( rs[1], rs[0] );
   vic->setEnds( zs[1], zs[0] );

   for ( Index v = 0; v < modelSampleMesh->nv; ++v )
   {
DEBUGLINE;
      const Point basePoint = modelSampleMesh->getVertex(v);
      DEBUGMACRO( v << "\t" << basePoint );

      vir->reset();
      vir->setTestPoint(basePoint);

      modelSampleSearcher->search(vir);

      std::vector<Index>& vertices = vir->getVertices();
      if (!vertices.empty())
      {
         Index boxChecks = 0;
         Index objectChecks = 0;
         std::vector<Index>::iterator it = vertices.begin();
         std::vector<Index>::iterator en = vertices.end();

         vic->setCentre(basePoint);

         while (it != en)
         {
            Point axis = shapeModelInst->getVertex(*it) - basePoint;
            axis.normalize();

            vic->setAxis(axis);

            vic->reset();
            modelSampleSearcher->search(vic);

            boxChecks += vic->getNumberOfBoxChecks();
            objectChecks += vic->getNumberOfChecks();

            //DEBUGMACRO( v << "\t" << (*it) << "\t" << vic->getVertices().size() );
            
            ++it;
         }
         //DEBUGMACRO( v << "\t" << vertices.size() );

         DEBUGMACRO( (0.0+objectChecks) / vertices.size() << "\t" << (0.0+boxChecks) / vertices.size() );
      }
   }

}