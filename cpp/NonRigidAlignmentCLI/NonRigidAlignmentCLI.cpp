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
#include "NonRigidAlignment/NonRigidAlignmentStateMachine.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <signal.h>

#include "Mesh/Mesh.h"
#include "NonRigidAlignment/NonRigidAlignment.h"

using namespace AWT;

NonRigidAlignmentStateMachine<double>::P nrasm;

void handleSignal(int sig)
{
   if (*nrasm != 0)
   {
      DEBUGMACRO("Calling external stop method!");
      DEBUGMACRO("(Pressing Ctrl-C again will quit the program normally)");
      nrasm->externalStop();
   }
}

int main(int argc, char** argv)
{
   DEBUGLINE;
   if (argc < 3)
   {
      //std::cerr << "usage: NonRigidAlignmentCLI <source mesh> <target mesh> <output filename> <output deformed mesh filename>" << std::endl;
      std::cerr << "usage: NonRigidAlignmentCLI <input lua file> <output root>" << std::endl;
      exit(1);
   }

   signal(SIGINT, handleSignal);

   StateMachineStack::P sms = StateMachineStack::getInstance();
   nrasm = NonRigidAlignmentStateMachine<double>::getInstance(sms, false);

   DEBUGLINE;
   if (!nrasm->load(argv[1]))
   {
      DEBUGMACRO("Could not load " << argv[1]);
      std::cerr << nrasm->getLastError() << std::endl;
      DEBUGLINEANDEXIT(1);
   }
   DEBUGLINE;

   if (argc == 4)
   {
      DEBUGMACRO("Only loading and saving; no processing!");
      nrasm->loadOnly();
   }

   unsigned int el = 0;

   DEBUGMACRO("Starting main loop...");
   while (nrasm->isRunning() && !nrasm->isError())
   {
      nrasm->step();

      if (false && nrasm->aboutToRefine())
      {
         if (el < 4)
         {
            std::stringstream defOutput; defOutput << argv[2] << ".level" << el << ".deformed.ply";
            std::stringstream tgtOutput; tgtOutput << argv[2] << ".level" << el << ".target.ply";

            nrasm->saveDeformedModel(defOutput.str().c_str());
            nrasm->saveTargetModel(tgtOutput.str().c_str());

            ++el;
         }
      }
   }

   if (!nrasm->isError())
   {
      //std::stringstream luaOutput; luaOutput << argv[2] << ".lua";
      //nrasm->save(luaOutput.str());

      //std::stringstream defOutput; defOutput << argv[2] << ".deformed.ply";
      //nrasm->saveDeformedModel(defOutput.str().c_str());

      std::stringstream prjOutput; prjOutput << argv[2] << ".projected.ply";
      nrasm->saveProjected(prjOutput.str().c_str());

      //std::stringstream tgtOutput; tgtOutput << argv[2] << ".target.ply";
      //nrasm->saveTargetModel(tgtOutput.str().c_str());

      std::cerr << "Success." << std::endl;
   }
   else
   {
      std::cerr << "An error was encountered:" << std::endl;  
      std::cerr << nrasm->getLastError() << std::endl;
   }
}