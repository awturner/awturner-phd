#include "StateMachineStack.h"

#include <vector>

struct AWT::StateMachineStack::D
{
   std::vector<BasicStateMachine::P> stack;
};

AWT::StateMachineStack::StateMachineStack( )
{
   m_D = new D;
}

AWT::StateMachineStack::~StateMachineStack( )
{
   delete m_D;
}

AWT::StateMachineStack::P AWT::StateMachineStack::getInstance( )
{
   AUTOGETINSTANCE( AWT::StateMachineStack, ( ) );
}

GETNAMEMACRO( AWT::StateMachineStack );

AWT::StateMachineStack* AWT::StateMachineStack::getStack( )
{
   return this;
}

void AWT::StateMachineStack::push( AWT::BasicStateMachine::P m )
{
   m_D->stack.push_back( m );
}

AWT::BasicStateMachine::P AWT::StateMachineStack::getTop( ) const
{
   if ( m_D->stack.empty( ) )
      return AWT::BasicStateMachine::P( );

   return m_D->stack.back( );
}

AWT::BasicStateMachine::P AWT::StateMachineStack::pop( )
{
   BasicStateMachine::P ret = m_D->stack.back( );

   m_D->stack.pop_back( );

   return ret;
}

bool AWT::StateMachineStack::step( )
{
   //PRINTVBL( getTop( )->getName( ) );
   bool ret = getTop( )->step( );

   if ( !isRunning( ) && !isError( ) && m_D->stack.size( ) > 1 )
      pop( );

   return ret;
}

bool AWT::StateMachineStack::isRunning( ) const
{
   return getTop( )->isRunning( );
}

bool AWT::StateMachineStack::isError( ) const
{
   return getTop( )->isError( );
}

std::string AWT::StateMachineStack::getLastError( ) const
{
   return getTop( )->getLastError( );
}

std::string AWT::StateMachineStack::getStateName( ) const
{
   return getTop( )->getStateName( );
}

void AWT::StateMachineStack::clear( )
{
   m_D->stack.clear();
}