#ifndef __SAVEABLESTATEMACHINE_H__
#define __SAVEABLESTATEMACHINE_H__

#include "BasicStateMachine.h"

namespace AWT
{
   class SaveableStateMachine : public BasicStateMachine
   {
   public:
      typedef ManagedAutoPointer<SaveableStateMachine> P;

      virtual std::string getMagic( ) const = 0;

      virtual bool load( const std::string& filename ) = 0;

      virtual void save( const std::string& filename ) = 0;
   };
}

#endif // __SAVEABLESTATEMACHINE_H__