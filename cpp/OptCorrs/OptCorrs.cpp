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
namespace AWT
{
   typedef unsigned int MeshIndex;
}

#include "XmlRpc.h"

#include "CommonMethods.h"
#include "TorusMethods.h"
#include "FixedMethods.h"
#include "ParticleMethods.h"
#include "CoulombicMethods.h"

#include "Useful/PrintMacros.h"
#include "Useful/Exception.h"
#include "Useful/XmlRpcDefines.h"

using namespace AWT;
using namespace AWT::AlignParametric;
using namespace XmlRpc;

// The server
XmlRpcServer s;

///////////////////////////////////////////////

int main(int argc, char** argv)
{
   if (argc != 2)
   {
      std::cerr << "No port number specified." << std::endl;
      exit(1);
   }

   int port = atoi(argv[1]);

   if (port < 1024 || port >= 65536)
   {
      std::cerr << "Port number out of range." << std::endl;
      exit(1);
   }

   typedef std::vector<RemoteMethod> RemoteMethods;
   RemoteMethods methods;

   CommonMethods::addMethods(methods);
   Torus::addMethods(methods);
   Particles::addMethods(methods);
   Fixed::addMethods(methods);
   CoulombicParticles::addMethods(methods);

   addMethodsToServer(s, methods);

   s.enableIntrospection(true);

   // Create the server socket on the specified port
   XmlRpcValue::setDoubleFormat("%.8g");
   s.bindAndListen(port);

   std::cerr << "XMLRPC Server listening on port " << port << "." << std::endl;

   // Wait for requests and process indefinitely (Ctrl-C to exit)
   while (1)
   {
      s.work(-1.0);
   }

   return 0;
}