#ifndef __TRANSFORMATION_H__
#define __TRANSFORMATION_H__

#include "Drawable.h"
#include "Mesh/Pose.h"

namespace AWT
{
   template <class T>
   class Transformation : public Drawable
   {
   public:
      typedef ManagedAutoPointer<Transformation<T>> P;

   public:
      virtual void setDrawable( Drawable::P d ) = 0;

      virtual Drawable::P getDrawable( ) = 0;

      virtual void setPose( typename Pose<T>::P pose ) = 0;

      virtual typename Pose<T>::P getPose( ) = 0;
   };
}

#endif // __TRANSFORMATION_H__