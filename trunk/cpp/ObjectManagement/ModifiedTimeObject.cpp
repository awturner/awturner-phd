#include "ModifiedTimeObject.h"

#include "Useful/PrintMacros.h"

AWT::ModifiedTime AWT::ModifiedTimeObject::lastMtime = 0;

AWT::ModifiedTimeObject::ModifiedTimeObject( )
: mtime( ++AWT::ModifiedTimeObject::lastMtime )
{
}

AWT::ModifiedTime AWT::ModifiedTimeObject::getModifiedTime( ) const
{
   return mtime;
}

void AWT::ModifiedTimeObject::modified( )
{
   // This means that no two objects can have the same modified time
   mtime = ++lastMtime;
}