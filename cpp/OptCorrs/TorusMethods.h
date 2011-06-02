#ifndef __TORUSMETHODS_H__
#define __TORUSMETHODS_H__

#include "Useful/XmlRpcDefines.h"

namespace AWT
{
   namespace AlignParametric
   {
      namespace Torus
      {
         void remoteMethod( initialize );
         void remoteMethod( addMesh );
         void remoteMethod( refine );

         void addMethods( RemoteMethods& methods );
      }
   }
}

#endif // __TORUSMETHODS_H__