#ifndef __COULOMBICMETHODS_H__
#define __COULOMBICMETHODS_H__

#include "Useful/XmlRpcDefines.h"

namespace AWT
{
   namespace AlignParametric
   {
      namespace CoulombicParticles
      {
         void remoteMethod( initialize );
         void remoteMethod( addMesh );
         void remoteMethod( refine );
         void remoteMethod( configureRefinement );

         void addMethods( RemoteMethods& methods );
      }
   }
}

#endif // __COULOMBICMETHODS_H__