#include "DeformableMeshFitting/DeformableMeshFittingStateMachine.h"
#include "Mesh/MeshSimilarity.h"

#include <iostream>

using namespace AWT;

int main( int argc, char** argv )
{
   for ( int i = 1; i < argc; ++i )
   {
      StateMachineStack::P sms = StateMachineStack::getInstance( );

      DeformableMeshFittingStateMachine<double>::P sm = DeformableMeshFittingStateMachine<double>::getInstance( sms );

      PRINTVBL( argv[i] );
      sm->load( argv[i] );

      try
      {
         MeshSimilarity<double>::P meshSim = MeshSimilarity<double>::getInstance( sm->getDeformableMeshFitting( )->typename Mesh<T>::P meshA, typename Mesh<T>::P meshB, const T aodThresh, const unsigned int subdivs, const bool inCorr = false );

         std::cout << argv[i] << "\t" << meshSim->getMeanDistance( ) << "\t" << meshSim->getRmsDistance( ) << "\t"
            << meshSim->getMaximumDistance( ) << "\t" << meshSim->getAreaOfDeviation( ) << std::endl;
      }
      catch ( AWT::Exception ex )
      {
         PRINTVBL( ex );
      }
      catch ( ... )
      {
         DEBUGMACRO( "An exception was caught" );
      }
   }
}