/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#include "CommandManager.h"

AWT::CommandManager::~CommandManager()
{
   clearUndoRedo();
}

bool AWT::CommandManager::canUndo()
{
   return !undoStack.empty();
}

bool AWT::CommandManager::canRedo()
{
   return !redoStack.empty();
}

bool AWT::CommandManager::undo()
{
   if (canUndo())
   {
      bool ret = undoStack.back()->undo();
      if (ret)
      {
         redoStack.push_back(undoStack.back());
         undoStack.pop_back();
      }
      return ret;
   }
   else
   {
      return false;
   }
}

bool AWT::CommandManager::redo()
{
   if (canRedo())
   {
      bool ret = redoStack.back()->execute();
      if (ret)
      {
         undoStack.push_back(redoStack.back());
         redoStack.pop_back();
      }
      return ret;
   }
   else
   {
      return false;
   }
}

bool AWT::CommandManager::execute(Command* cmd)
{
   if (dynamic_cast<UndoableCommand*>(cmd) != 0)
   {
      bool ret = cmd->execute();
      if (ret)
         undoStack.push_back(dynamic_cast<UndoableCommand*>(cmd));

      return ret;
   }
   else
   {
      clearUndoRedo();
      return cmd->execute();
   }
}

void AWT::CommandManager::clearUndoRedo()
{  
   while (!undoStack.empty())
   {
      delete undoStack.back();
      undoStack.pop_back();
   }

   while (!redoStack.empty())
   {
      delete redoStack.back();
      redoStack.pop_back();
   }
}
