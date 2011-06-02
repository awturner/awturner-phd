#ifndef __WRITETOCLIPBOARD_H__
#define __WRITETOCLIPBOARD_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   class WriteToClipboard : public ManagedObject
   {
   protected:
      WriteToClipboard( );
      virtual ~WriteToClipboard( );

   public:
      static void copyToClipboard( const std::string& string );

   protected:

   };
}

#endif // __WRITETOCLIPBOARD_H__