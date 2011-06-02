#ifndef __MODIFIEDTIME_OBJECT_H__
#define __MODIFIEDTIME_OBJECT_H__

namespace AWT
{
   typedef unsigned long long ModifiedTime;

   class Mutex;

   class ModifiedTimeObject
   {
   protected:
      ModifiedTimeObject( );
      
   public:
      virtual ModifiedTime getModifiedTime( ) const;
      void                 modified( );

   private:
      mutable ModifiedTime mtime;
      static ModifiedTime lastMtime;
   };
}




#endif // __MODIFIEDTIME_OBJECT_H__
