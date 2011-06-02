#ifndef __OPENGLFONT_H__
#define __OPENGLFONT_H__


#include "ObjectManagement/ManagedObject.h"

#include "OpenGLHeaders.h"
#include <QFont>


namespace AWT
{
   class OpenGLFont : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<OpenGLFont> P;

   public:
      virtual void select( ) = 0;
   };

   class QtGLFont : public OpenGLFont
   {
   public:
      typedef ManagedAutoPointer<QtGLFont> P;

   protected:
      QtGLFont( QFont& font )
      {
         m_Font = new QFont( font );
      }

      virtual ~QtGLFont( )
      {
         delete m_Font;
      }

   public:
      static P getInstance( QFont& font )
      {
         AUTOGETINSTANCE( AWT::QtGLFont, ( font ) );
      }

      virtual void select( )
      {
         
      }
   };
}

#endif //  __OPENGLTEXTRENDERER_H__