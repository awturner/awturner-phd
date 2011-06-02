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
#include "DeformableMeshFitting/DeformableMeshFittingStateMachine.h"

#include <iostream>

#include <signal.h>
#include <string>
#include <new>

#include "vld.h"

#include "StateMachine/StateMachineStack.h"

using namespace AWT;

DeformableMeshFittingStateMachine<double>::P sm;
std::string saveFilename;

void handleInterrupt(int i)
{
   switch (i)
   {
   case SIGINT:
      if (*sm != 0)
      {
         sm->save(saveFilename);
         std::cerr << "Saved to " << saveFilename << std::endl;
      }

      AWT::ReferenceCountedObject::report();

      DEBUGLINEANDEXIT(1);

      break;
   }
}

int main(int argc, char** argv)
{
   try
   {
      if (argc < 3)
      {
         std::cerr << "Usage: DeformableMeshFittingCLI <source filename> <output filename>" << std::endl;
         DEBUGLINEANDEXIT(1);
      }

      StateMachineStack::P sms = StateMachineStack::getInstance();

      for (int i = 1; i < argc; i += 2)
      {
         saveFilename = std::string(argv[i+1]);

         while (*sms->getTop() != 0)
            sms->pop();

         sm = DeformableMeshFittingStateMachine<double>::getInstance(sms);

         sm->load(argv[i+0]);
         
         //signal(SIGINT, handleInterrupt);

         if (i+2 < argc && !strcmp(argv[i+2], "-evalonly"))
         {
            DEBUGLINE;
            ++i;
            sm->evalOnly();
         }

         try
         {
            while (sm->isRunning() && !sm->isError())
            {
               std::cerr << "State: " << sm->getStateName() << std::endl;
               sm->step();
            }
         }
         catch (const std::bad_alloc& x)
         {
            std::cerr << "Out of memory: " << x.what() << std::endl;
         }
         
         if (sm->isError())
            std::cerr << sm->getLastError() << std::endl;

         sm->save(saveFilename);
      }

      //DEBUGLINEANDEXIT(99);
      DEBUGLINE;
   }
   catch (...)
   {
      std::cerr << "An exception was caught" << std::endl;
   }

   //DEBUGLINE;

   //AWT::ReferenceCountedObject::report();

   //DEBUGLINE;
}