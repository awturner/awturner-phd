#ifndef __CATESMETHODS_H__
#define __CATESMETHODS_H__

#include "Useful/XmlRpcDefines.h"

namespace AWT
{
   namespace AlignParametric
   {
      namespace Particles
      {
         void remoteMethod( initialize );
         void remoteMethod( addMesh );
         void remoteMethod( configureRefinement );

         void addMethods( RemoteMethods& methods );
      }
   }
}

#endif // __CATESMETHODS_H__