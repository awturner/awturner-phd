#ifndef __SIMPLEMESHTREEDATA_H__
#define __SIMPLEMESHTREEDATA_H__

#include "ObjectEKDTree/OEKDTreeData.h"
#include "Mesh.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class TreeData : public OEKDTree::OEKDTreeData<double,3>
      {
      protected:
         TreeData( Mesh::P mesh, const bool _useFaces );
         virtual ~TreeData( );

      public:
         typedef ManagedAutoPointer< TreeData > P;

         static P getInstance( Mesh::P mesh, const bool _useFaces );

      public:
         std::string getClassName( ) const;

         virtual OEKDTree::ObjectIndex getNumberOfObjects( ) const;

         virtual double getCoordinate( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;
         virtual void getPosition( const OEKDTree::ObjectIndex objectIndex, double* pos ) const;

         virtual double getMinimumBound( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;
         virtual double getMaximumBound( OEKDTree::ObjectIndex objectIndex, OEKDTree::AxisIndex axis ) const;

         virtual Mesh::P getMesh( );

         const bool useFaces;

      protected:
         struct D;
         D* m_D;
      };
   }
}

#endif // __SIMPLEMESHTREEDATA_H__