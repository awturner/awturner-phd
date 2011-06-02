#ifndef __FIXEDMETHODS_H__
#define __FIXEDMETHODS_H__

#include "Useful/XmlRpcDefines.h"

namespace AWT
{
   namespace AlignParametric
   {
      namespace Fixed
      {
         void remoteMethod( initialize );
         void remoteMethod( addMesh );

         void addMethods( RemoteMethods& methods );
      }
   }
}

#endif // __FIXEDMETHODS_H__