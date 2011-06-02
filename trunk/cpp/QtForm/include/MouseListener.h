#ifndef __MOUSELISTENER_H__
#define __MOUSELISTENER_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   class MouseListener : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MouseListener> P;

      virtual void mouseEntered( ) = 0;
      virtual void mouseExited( ) = 0;

      virtual void mouseChanged( const double* eye, const double* normal, const int button ) = 0;
   };
}

#endif // __MOUSELISTENER_H__