#ifndef __X_REFERENCECOUNTEDOBJECT_H__
#define __X_REFERENCECOUNTEDOBJECT_H__

#include <map>
#include <string>

#include "Useful/PrintMacros.h"

#define STANDARDGETINSTANCE( Class, Args ) { Class* ret = new Class##Args; ret->countMe( ); return ret; }
#define GETNAMEMACRO( Class ) std::string Class::getClassName( ) const { return #Class; }

namespace AWT
{
   typedef unsigned int ReferenceCount;

   class ReferenceCountedObject
   {
   public:
      ReferenceCount getReferenceCount( ) const;

      void keep( const void* obj = 0 );
      void release( const void* obj = 0);

      void monitor( );
      void unmonitor( );

      virtual std::string getClassName( ) const = 0;

      void countMe( );

      ReferenceCount getObjectCount( std::string& str );

      static void record( );

      static void compare( );

      static void reportOne( const std::string key );

      static void report( const bool hideZeros = true );

      static bool debug;

      template <class RefTy>
      static void swapRefCountedObjects( const void* /*obj*/, RefTy*& holder, RefTy* objNew, const bool = false )
      {
         if ( debug ) DEBUGLINE;

         if ( holder != objNew )
         {
            RefTy* objOld = holder;
            
            if ( debug ) DEBUGLINE;

            if ( objNew != 0 )
            {
               objNew->keep( );
            }
            if ( debug ) DEBUGLINE;
            
            holder = objNew;
            
            if ( debug ) DEBUGLINE;
            
            if ( objOld != 0 )
            {
               if ( debug ) DEBUGLINE;
               objOld->release( );
            }
            if ( debug ) DEBUGLINE;
            
         }
         if ( debug )
            DEBUGLINE;
      }
         
   protected:
      static void report( const std::string name, const ReferenceCount n );

      void uncountMe( );

      ReferenceCountedObject( );
      virtual ~ReferenceCountedObject( );

   private:
      ReferenceCount m_ReferenceCount;
      bool m_Monitored;
      
      static std::map<std::string,ReferenceCount> m_InstanceMap;
      static std::map<std::string,ReferenceCount> m_InstanceMapRecord;
   };
}


#endif // __X_REFERENCECOUNTEDOBJECT_H__
