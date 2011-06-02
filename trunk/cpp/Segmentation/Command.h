#ifndef __COMMAND_H__
#define __COMMAND_H__

namespace AWT
{
   class Command
   {
   public:
      virtual bool execute( ) = 0;
   };

   class UndoableCommand : public Command
   {
   public:
      virtual bool undo( )    = 0;
   };
}

#endif __COMMAND_H__