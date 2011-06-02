#ifndef __OPENGLDRAWABLEIMPL_H__
#define __OPENGLDRAWABLEIMPL_H__

#include "DrawableFramework/DrawableImpl.h"

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   class OpenGLDrawableImpl : public DrawableImpl
   {
   public:
      typedef ManagedAutoPointer<OpenGLDrawableImpl> P;

   };
}

#endif // __OPENGLDRAWABLEIMPL_H__