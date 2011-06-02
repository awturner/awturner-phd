#ifndef __COMMANDMANAGER_H__
#define __COMMANDMANAGER_H__

#include "Command.h"
#include <vector>

namespace AWT
{
   class CommandManager
   {
   public:
      ~CommandManager( );

      bool canUndo( );
      bool canRedo( );

      bool undo( );
      bool redo( );

      bool execute( Command* cmd );

   protected:
      void clearUndoRedo( );

      std::vector<UndoableCommand*> undoStack;
      std::vector<UndoableCommand*> redoStack;
   };
}

#endif // __COMMANDMANAGER_H__