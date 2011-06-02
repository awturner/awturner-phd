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
   if ( argc != 2 )
   {
      std::cerr << "No port number specified." << std::endl;
      exit(1);
   }

   int port = atoi(argv[1]);

   if ( port < 1024 || port >= 65536 )
   {
      std::cerr << "Port number out of range." << std::endl;
      exit(1);
   }

   typedef std::vector<RemoteMethod> RemoteMethods;
   RemoteMethods methods;

   CommonMethods::addMethods( methods );
   Torus::addMethods( methods );
   Particles::addMethods( methods );
   Fixed::addMethods( methods );
   CoulombicParticles::addMethods( methods );

   addMethodsToServer( s, methods );

   s.enableIntrospection(true);

   // Create the server socket on the specified port
   XmlRpcValue::setDoubleFormat( "%.8g" );
   s.bindAndListen(port);

   std::cerr << "XMLRPC Server listening on port " << port << "." << std::endl;

   // Wait for requests and process indefinitely (Ctrl-C to exit)
   while ( 1 )
   {
      s.work(-1.0);
   }

   return 0;
}