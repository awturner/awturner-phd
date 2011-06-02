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
#include "MatlabEngine.h"

#include "engine.h"

struct AWT::MatlabEngine::D
{
   static Engine* ms_Engine;
   static unsigned int ms_count;
};

Engine* AWT::MatlabEngine::D::ms_Engine = 0;
unsigned int AWT::MatlabEngine::D::ms_count  = 0;

AWT::MatlabEngine::MatlabEngine()
{
   m_D = new D;

   if (D::ms_Engine == 0)
   {
      D::ms_Engine = engOpen("\0");

      if (!D::ms_Engine)
      {
         throw "Could not start MATLAB Engine!";
      }
   }

   ++D::ms_count;
}

AWT::MatlabEngine::~MatlabEngine()
{
   if (--D::ms_count == 0)
   {
      engClose(D::ms_Engine);
      D::ms_Engine = 0;
   }

   delete m_D;
}

AWT::MatlabEngine::P AWT::MatlabEngine::getInstance()
{
   AUTOGETINSTANCE(AWT::MatlabEngine, ());
}

GETNAMEMACRO(AWT::MatlabEngine);

Engine* AWT::MatlabEngine::getEngine()
{
   return D::ms_Engine;
}

int AWT::MatlabEngine::eval(const char* cmd)
{
   return engEvalString(D::ms_Engine, cmd);
}

AWT::MatlabArray::P AWT::MatlabEngine::getVariable(const char* name)
{
   return MatlabArray::getInstance(this, name);
}

int AWT::MatlabEngine::putVariable(const char* name, AWT::MatlabArray::P var)
{
   return engPutVariable(D::ms_Engine, name, var->getArray());
}