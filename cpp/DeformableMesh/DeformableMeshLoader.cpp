#include "DeformableMeshLoader.h"

#include "DeformableMeshImpl.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/TuplesImpl.h"

#include <fstream>
#include <string>

template <class T>
typename AWT::DeformableMesh<T>::P AWT::DeformableMeshLoader<T>::load(const char *filename)
{
   std::ifstream is( filename );

   DeformableMeshImpl<T>::P ret;

   if ( !is.fail( ) )
   {
      std::string line;

      std::getline( is, line );

      if ( !is.fail( ) )
      {
         typename Mesh<T>::P meanShape = VTKMeshLoader<T>::load( line.c_str( ), false );

         if ( *meanShape != 0 )
         {
            ret = DeformableMeshImpl<T>::getInstance( *meanShape );

            T def[] = { 0, 0, 0 };
            T norm = 0;

            while ( true )
            {
               std::getline( is, line );

               if ( !is.fail( ) )
               {
                  Tuples<T>::P tuples = TuplesImpl<T>::getInstance( 3, def, meanShape->getNumberOfVertices( ) );

                  std::ifstream iss( line.c_str( ) );

                  T pos[3];

                  MESH_EACHVERTEX( meanShape, v )
                  {
                     iss >> pos[0] >> pos[1] >> pos[2];

                     tuples->setPoint( v, pos );

                     norm += pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2];
                  }

                  if ( norm > 1e-8 )
                     ret->addMode( tuples );

                  iss.close( );
               }
               else
               {
                  break;
               }
            }
         } 
      }
   }

   is.close( );

   return ret;
}

template class AWT::DeformableMeshLoader<double>;
template class AWT::DeformableMeshLoader<float>;
