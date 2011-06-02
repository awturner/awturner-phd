#include "CommandManager.h"

AWT::CommandManager::~CommandManager( )
{
   clearUndoRedo( );
}

bool AWT::CommandManager::canUndo( )
{
   return !undoStack.empty( );
}

bool AWT::CommandManager::canRedo( )
{
   return !redoStack.empty( );
}

bool AWT::CommandManager::undo( )
{
   if ( canUndo( ) )
   {
      bool ret = undoStack.back( )->undo( );
      if ( ret )
      {
         redoStack.push_back( undoStack.back( ) );
         undoStack.pop_back( );
      }
      return ret;
   }
   else
   {
      return false;
   }
}

bool AWT::CommandManager::redo( )
{
   if ( canRedo( ) )
   {
      bool ret = redoStack.back( )->execute( );
      if ( ret )
      {
         undoStack.push_back( redoStack.back( ) );
         redoStack.pop_back( );
      }
      return ret;
   }
   else
   {
      return false;
   }
}

bool AWT::CommandManager::execute( Command* cmd )
{
   if ( dynamic_cast<UndoableCommand*>( cmd ) != 0 )
   {
      bool ret = cmd->execute( );
      if ( ret )
         undoStack.push_back( dynamic_cast<UndoableCommand*>( cmd ) );

      return ret;
   }
   else
   {
      clearUndoRedo( );
      return cmd->execute( );
   }
}

void AWT::CommandManager::clearUndoRedo( )
{  
   while ( !undoStack.empty( ) )
   {
      delete undoStack.back( );
      undoStack.pop_back( );
   }

   while ( !redoStack.empty( ) )
   {
      delete redoStack.back( );
      redoStack.pop_back( );
   }
}
