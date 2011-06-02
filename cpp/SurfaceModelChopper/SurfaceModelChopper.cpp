
#include "DeformableMesh/DeformableMesh.h"
#include "DeformableMesh/DeformableMeshLoader.h"

#include "Useful/PrintMacros.h"
#include "Useful/VNLTypeDefs.h"

#include <vector>
#include <fstream>
#include <string>

using namespace AWT;

int main( int argc, char** argv )
{
   std::ifstream                 conditionStream( argv[1] );
   std::vector<std::string>      conditionNames;
   std::vector<std::vector<int>> conditions;
   std::vector<DoublePoint>      planes;
   std::string                   modelName;

   bool validateOnly = ( argc > 2 ) && !strcmp("validate",argv[2]);
   
   char keyword[80];
   while ( !conditionStream.eof( ) )
   {
      conditionStream >> keyword;

      if ( strcmp( "model", keyword ) == 0 )
      {
         conditionStream >> modelName;
      }
      else if ( strcmp( "plane", keyword ) == 0 )
      {
         DoublePoint plane;

         conditionStream >> plane[0] >> plane[1] >> plane[2] >> plane[3];

         PRINTVEC( plane, 4 );

         plane /= sqrt( plane[0]*plane[0] + plane[1]*plane[1] + plane[2]*plane[2] );

         PRINTVEC( plane, 4 );

         planes.push_back( plane );
      }
      else if ( strcmp( "condition", keyword ) == 0 )
      {
         std::string name;
         int nplanes;
         int pl;
         std::vector<int> condPlanes;

         conditionStream >> name >> nplanes;

         for ( int i = 0; i < nplanes; ++i )
         {
            conditionStream >> pl;
            condPlanes.push_back( pl );
         }

         conditions.push_back( condPlanes );
         conditionNames.push_back( name );
      }
      else
      {
         DEBUGMACRO( "Unknown keyword!" );
         PRINTVBL( keyword );
         DEBUGLINEANDEXIT( 1 );
      }
   }

   PRINTVBL( modelName );
   std::cerr << "Planes:" << std::endl;
   for ( size_t i = 0; i < planes.size( ); ++i )
   {
      std::cerr << "  " << i << ": " << planes[i][0] << "x + " << planes[i][1] << "y + " << planes[i][2] << "z + " << planes[i][3] << "w = 0" << std::endl;
   }
   std::cerr << "Conditions:" << std::endl;
   for ( size_t i = 0; i < conditions.size( ); ++i )
   {
      std::cerr << "  " << conditionNames[i] << ":";
      for ( size_t j = 0; j < conditions[i].size( ); ++j )
      {
         std::cerr << " " << conditions[i][j];
      }
      std::cerr << std::endl;
   }

   if ( validateOnly )
   {
      DEBUGLINEANDEXIT( 0 );
   }

   DeformableMesh<double>::P model = DeformableMeshLoader<double>::load( modelName.c_str() );
   
   std::cerr << "Loaded model " << modelName << " successfully!" << std::endl;
   PRINTVBL( model->getNumberOfFaces( ) );

   MeshIndex vs[3];

   for ( size_t c = 0; c < conditions.size( ); ++c )
   {
      std::vector<int> condPlanes = conditions[c];

      DoublePoint points[3];
      std::ofstream os( conditionNames[c].c_str( ) );
      int foundCount = 0;
      int rejected = 0;

      //for ( int f = 0; f < model->getNumberOfFaces( ); ++f )
      MESH_EACHFACE( model, f )
      {
         model->getFaceIndices( f, vs );
         
         for ( int v = 0; v < 3; ++v )
         {
            model->getVertex( vs[v], points[v].data_block() );
            points[v][3] = 1;
         }

         size_t cc;

         for ( cc = 0; cc < condPlanes.size( ); ++cc )
         {
            DoublePoint plane = planes[ abs(condPlanes[cc])-1 ];

            int v;
            for ( v = 0; v < 3; ++v )
            {
               double dot = 0;
               for ( int i = 0; i < 4; ++i )
                  dot += plane[i]*points[v][i];

               if ( dot > 0 )
                  break;
            }

            if ( (v == 3) ^ (condPlanes[cc] > 0) )
               break;
         }

         if ( cc == condPlanes.size( ) )
         {
            os << vs[0] << " " << vs[1] << " " << vs[2] << std::endl;
            ++foundCount;
         }
         else
         {
            ++rejected;
         }
      }

      os.close( );
      std::cerr << conditionNames[c] << " " << foundCount << " " << rejected << std::endl;
   }
}