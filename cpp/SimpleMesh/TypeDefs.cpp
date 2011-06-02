#include "TypeDefs.h"

#include <limits>
#include "vnl/algo/vnl_svd.h"

using namespace AWT::SimpleMesh;

const Index  AWT::SimpleMesh::INVALID_INDEX      = std::numeric_limits<Index>::max( );
const double AWT::SimpleMesh::FAR_FAR_AWAY       = std::numeric_limits<double>::infinity();
const double AWT::SimpleMesh::INVALID_COORDINATE = std::numeric_limits<double>::quiet_NaN();

AWT::SimpleMesh::PointIndexWeights::PointIndexWeights( )
{
   p.fill( INVALID_COORDINATE );
   i = INVALID_INDEX;
   w.fill( INVALID_COORDINATE );
}

AWT::SimpleMesh::LameckerMeasures::LameckerMeasures( )
{
   modelScale = 1;
}

std::ostream& operator<<( std::ostream& os, const AWT::SimpleMesh::Correspondence& self )
{
   os << "Correspondence[" << std::endl;
   os << "\tmodelPoint=("<<self.modelPoint.p<<");" << std::endl;
   os << "\tmodelNormal=("<<self.modelNormal[0]<<","<<self.modelNormal[1]<<","<<self.modelNormal[2]<<","<<self.modelNormal[3]<<");" << std::endl;
   os << "\tfragPoint=("<<self.fragPoint.p<<");" << std::endl;
   os << "\twhichFragment="<<(1*self.whichFragment)<<";" << std::endl;
   os << "]";

   return os;
}

void AWT::SimpleMesh::calculateTransformation( const AWT::SimpleMesh::Vector& v, AWT::SimpleMesh::Transformation& mat )
{
   Vector q(4);
   q.set( v.data_block() );
   q.normalize();

   mat(0,0) = q(0) * q(0) + q(1) * q(1) - q(2) * q(2) - q(3) * q(3);
   mat(0,1) = 2 * q(1) * q(2) - 2 * q(0) * q(3);
   mat(0,2) = 2 * q(1) * q(3) + 2 * q(0) * q(2);

   mat(1,0) = 2 * q(1) * q(2) + 2 * q(0) * q(3);
   mat(1,1) = q(0) * q(0) - q(1) * q(1) + q(2) * q(2) - q(3) * q(3);
   mat(1,2) = 2 * q(2) * q(3) - 2 * q(0) * q(1);       

   mat(2,0) = 2 * q(1) * q(3) - 2 * q(0) * q(2);
   mat(2,1) = 2 * q(2) * q(3) + 2 * q(0) * q(1);
   mat(2,2) = q(0) * q(0) - q(1) * q(1) - q(2) * q(2) + q(3) * q(3);

   mat(0,3) = v(4);
   mat(1,3) = v(5);
   mat(2,3) = v(6);

   mat.set_row(3, 0.0);
   mat(3,3) = 1;
}

AWT::SimpleMesh::Vector AWT::SimpleMesh::matrixToPose(const AWT::SimpleMesh::Transformation& R)
{
   Transformation Q;
   Q(1,1) = R(0,0)-R(1,1)-R(2,2);
   Q(1,2) = R(0,1)+R(1,0);
   Q(1,3) = R(0,2)+R(2,0);
   Q(1,0) = R(2,1)-R(1,2);

   Q(2,1) = R(0,1)+R(1,0);
   Q(2,2) = R(1,1)-R(0,0)-R(2,2);
   Q(2,3) = R(1,2)+R(2,1);
   Q(2,0) = R(0,2)-R(2,0);

   Q(3,1) = R(0,2)+R(2,0);
   Q(3,2) = R(1,2)+R(2,1);
   Q(3,3) = R(2,2)-R(0,0)-R(1,1);
   Q(3,0) = R(1,0)-R(0,1);

   Q(0,1) = R(2,1)-R(1,2);
   Q(0,2) = R(0,2)-R(2,0);
   Q(0,3) = R(1,0)-R(0,1);
   Q(0,0) = R(0,0)+R(1,1)+R(2,2);

   vnl_svd<double> svd(Q);
   Point bestQuat = svd.U().get_column(0);

   Vector pose(7);
   for ( Index i = 0; i < 4; ++i )
      pose(i) = bestQuat(i);

   pose(4) = R(0,3);
   pose(5) = R(1,3);
   pose(6) = R(2,3);

   return pose;
}

AWT::SimpleMesh::Transformation AWT::SimpleMesh::poseToMatrix(const AWT::SimpleMesh::Vector& pose)
{
   Transformation trans;
   calculateTransformation(pose, trans);
   return trans;
}