#include "RandomMeshSlicer.h"

#include "Mesh/Mesh.h"


#include "MeshSlicer/MeshSlicedPair.h"
#include "MeshSlicer/MeshSlicer.h"
#include "MeshSlicer/PlanarCut.h"
#include "MeshSlicer/NotCuttingFunction.h"
#include "MeshSlicer/AndCuttingFunction.h"
#include "Mesh/Mesh.h"
#include "Mesh/VTKMeshLoader.h"
#include "OpenGLDrawables/OpenGLDrawableMesh.h"
#include "Mesh/MeshFunctions.h"
#include "DrawableFramework/DrawColour.h"
#include "Useful/ArrayVectorFunctions.h"
#include "Mesh/AreaAveragedNormalCalculator.h"

#include "Useful/Noise.h"
#include "Useful/PrintMacros.h"

#include <vector>

template <class T>
struct AWT::RandomMeshSlicer<T>::D
{
   void addMesh( typename AWT::Mesh<T>::P useMesh )
   {
      if ( useMesh->getNumberOfVertices( ) == 0 )
         return;

      typename NormalCalculator<T>::P nc = AreaAveragedNormalCalculator<T>::getInstance( );
      nc->calculateNormalsAndSet( useMesh );

      typename Mesh<T>::P autoPtr;
      autoPtr = useMesh;

      m_Fragments.push_back( autoPtr );
   }

   void recursivelySlice( typename AWT::Mesh<T>::P mesh, typename AWT::CuttingFunction<T>::P* cutFuncs, const int depth, const int maxdepth )
   {
      // Doing it like this ensures there isn't a "no mans land" between fragments as
      // there would be if you used "Not" function.

      MeshSlicer<T>::P meshSlicer = MeshSlicer<T>::getInstance( );
      meshSlicer->setCuttingFunction( cutFuncs[depth] );

      MeshSlicedPair<T>::P msp = meshSlicer->sliceMesh( mesh );

      if ( depth == maxdepth-1 )
      {
         addMesh( msp->getIncluded( ) );
         addMesh( msp->getExcluded( ) );
      }
      else
      {
         recursivelySlice( msp->getIncluded( ), cutFuncs, depth+1, maxdepth );
         recursivelySlice( msp->getExcluded( ), cutFuncs, depth+1, maxdepth );
      }
   }

   std::vector<typename Mesh<T>::P> m_Fragments;
};

template <class T>
AWT::RandomMeshSlicer<T>::RandomMeshSlicer( typename AWT::Mesh<T>::P mesh, const unsigned int nplanes )
{
   m_D = new D;

   // Calculate the mesh's standard deviation
   T meshStDev = sqrt( AWT::MeshFunctions<T>::calculateVariance( mesh ) );

   CuttingFunction<T>::P* cutFuncs = new CuttingFunction<T>::P[nplanes];

   for ( unsigned int pl = 0; pl < nplanes; ++pl )
   {
      T plane[4];

      // Pick a random direction
      for ( int ax = 0; ax < 3; ++ax )
         plane[ax] = AWT::Noise<T>::randu( -1, 1 );

      normalize( plane, 3 );

      // Pick a random distance from the normal
      plane[3] = AWT::Noise<T>::randu( -1, 1 ) * meshStDev;

      PlanarCut<T>::P planeCut = PlanarCut<T>::getInstance( );
      planeCut->setPlane( plane );

      cutFuncs[pl] = planeCut;
   }

   m_D->recursivelySlice( mesh, cutFuncs, 0, nplanes );

   delete[] cutFuncs;
}

template <class T>
AWT::RandomMeshSlicer<T>::~RandomMeshSlicer( )
{
   delete m_D;
}

template <class T>
typename AWT::RandomMeshSlicer<T>::P AWT::RandomMeshSlicer<T>::getInstance( typename AWT::Mesh<T>::P mesh, unsigned int nplanes )
{
   AUTOGETINSTANCE( AWT::RandomMeshSlicer<T>, ( mesh, nplanes ) );
}

template <class T>
GETNAMEMACRO( AWT::RandomMeshSlicer<T> );

template <class T>
unsigned int AWT::RandomMeshSlicer<T>::getNumberOfFragments( ) const
{
   return static_cast<unsigned int>( m_D->m_Fragments.size( ) );
}

template <class T>
typename AWT::Mesh<T>::P AWT::RandomMeshSlicer<T>::getFragment( const unsigned int f )
{
   return *m_D->m_Fragments[f];
}

template class AWT::RandomMeshSlicer<double>;
template class AWT::RandomMeshSlicer<float>;