#ifndef __XMLRPCDEFINES_H__
#define __XMLRPCDEFINES_H__

#define RPC_METHOD_NAME( server, MethodName, RPName ) class MethodName : public XmlRpcServerMethod \
{ \
public: \
   MethodName( XmlRpcServer* server) : XmlRpcServerMethod(#RPName, server) { std::cout << #RPName << std::endl; } \
   void execute(XmlRpcValue& params, XmlRpcValue& result)

#define RPC_METHOD( server, MethodName ) RPC_METHOD_NAME( server, MethodName, MethodName )
#define RPC_X( server, MethodName ) } __inst_##MethodName(&server);

#include "vnl/vnl_vector.h"
#include "Useful/Exception.h"
#include "XmlRpc.h"
#include <vector>

using namespace XmlRpc;

namespace AWT
{
   template <class T>
   void copyVectorIntoXmlRpcValue( const vnl_vector<T>& vec, XmlRpcValue& val )
   {
      val.setSize( vec.size() );
      for ( unsigned int i = 0; i < vec.size(); ++i )
         val[i] = vec[i];
   }

   template <class T>
   void copyXmlRpcValueIntoVector( XmlRpcValue& val, vnl_vector<T>& vec, const unsigned int length )
   {
      if ( val.size() != length )
      {
         PRINTVBL( val.size() );
         PRINTVBL( length );
         throw XmlRpcException( "Argument is the wrong length" );
      }

      vec.set_size( val.size() );
      for ( Index i = 0; i < (Index)val.size(); ++i )
      {
         switch ( val[i].getType() )
         {
         case XmlRpcValue::TypeInt:
            vec[i] = (double)( (int)val[i] );
            break;
         default:
            vec[i] = (double)val[i];
         }
      }
   }

   class RemoteMethod : public XmlRpcServerMethod
   {
   public:
      RemoteMethod( const std::string& name, void (*_callback)( XmlRpcValue&, XmlRpcValue& ) )
         : XmlRpcServerMethod(name, 0), callback( _callback )
      {}

      virtual void execute( XmlRpcValue& params, XmlRpcValue& result )
      { 
         try
         {
            this->callback( params, result );
         }
         catch ( AWT::Exception ex )
         {
            throw XmlRpcException( std::string( ex.what() ) );
         }
         catch ( const char* s )
         {
            throw XmlRpcException( std::string( s ) );
         }
      }

   protected:
      void (*callback)( XmlRpcValue& params, XmlRpcValue& result );
   };

   typedef std::vector<RemoteMethod> RemoteMethods;
}

#define remoteMethod( Name ) Name( XmlRpcValue& params, XmlRpcValue& result )

#endif // __XMLRPCDEFINES_H__