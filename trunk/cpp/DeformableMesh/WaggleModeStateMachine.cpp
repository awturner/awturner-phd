#include "WaggleModeStateMachine.h"

#include "StateMachine/SaveableStateMachine.h"

#include "Maths/Constants.h"

using namespace AWT;

template <class T>
AWT::WaggleModeStateMachine<T>::WaggleModeStateMachine( StateMachineStack::P stack )
{
   m_Stack = stack;
   m_Step = 0;
   m_Finished = false;
}
 
template <class T>
typename AWT::WaggleModeStateMachine<T>::P AWT::WaggleModeStateMachine<T>::getInstance( StateMachineStack::P stack )
{
   AUTOGETINSTANCE( AWT::WaggleModeStateMachine<T>, ( stack ) );
}

template <class T>
GETNAMEMACRO( AWT::WaggleModeStateMachine<T> );

template <class T>
void AWT::WaggleModeStateMachine<T>::setMesh( typename DeformableMesh<T>::P mesh )
{
  m_Mesh = mesh;
}

template <class T>
StateMachineStack* AWT::WaggleModeStateMachine<T>::getStack( )
{
   return *m_Stack;
}

template <class T>
bool AWT::WaggleModeStateMachine<T>::step( )
{
   if ( !m_Finished )
   {
      unsigned int stepsPerMode = 60;

      unsigned int whichMode = 1 + ( m_Step / stepsPerMode ) % ( 4 - 1 );
      unsigned int stepNumber = m_Step % stepsPerMode;

      m_Mesh->setModeWeight( whichMode, sin( (stepNumber+static_cast<T>( 0 ) )/stepsPerMode * AWT::Constants<T>::Pi * 2 ) );

      ++m_Step;

      unsigned int nextMode = 1 + ( m_Step / stepsPerMode ) % ( 4 - 1 );

      if ( nextMode != whichMode )
      {
         m_Mesh->setModeWeight( whichMode, static_cast<T>( (whichMode==0)?1:0 ) );
      }

      if ( m_Step >= stepsPerMode && nextMode == 0 )
      {
         m_Finished = true;
      }
   }

   return true;
}

template <class T>
bool AWT::WaggleModeStateMachine<T>::isRunning( ) const
{
   return !m_Finished;
}

template <class T>
bool AWT::WaggleModeStateMachine<T>::isError( ) const
{
   return false;
}

template <class T>
std::string AWT::WaggleModeStateMachine<T>::getLastError( ) const
{
   return "";
}

template <class T>
std::string AWT::WaggleModeStateMachine<T>::getStateName( ) const
{
   return m_Finished ? "Finished" : "Stepping";
}

template <class T>
std::string AWT::WaggleModeStateMachine<T>::getMagic( ) const
{
   return "BLAH";
}

template <class T>
bool AWT::WaggleModeStateMachine<T>::load( const std::string& )
{
   return true;
}

template <class T>
void AWT::WaggleModeStateMachine<T>::save( const std::string& )
{
}

template class AWT::WaggleModeStateMachine<double>;
template class AWT::WaggleModeStateMachine<float>;