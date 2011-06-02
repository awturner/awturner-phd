#ifndef __MANAGEDOBJECTSET_H__
#define __MANAGEDOBJECTSET_H__

#include <vector>
#include "Useful/PrintMacros.h"

namespace AWT
{
   template <class Ty>
   class ManagedObjectSet : public virtual ManagedObject
   {
   protected:
      ManagedObjectSet( )
      {
      }

      virtual ~ManagedObjectSet( )
      {
         removeAll( );
      }

   public:
      static ManagedObjectSet<Ty>* getInstance( )
      {
         STANDARDGETINSTANCE( ManagedObjectSet, ( ) );
      }

      void add( ManagedAutoPointer<Ty> obj )
      {
         m_Listeners.push_back( obj );

         modified( );
      }

      void remove( ManagedAutoPointer<Ty> obj )
      {
         std::vector<Ty*>::iterator it = m_Listeners.begin( );
         for ( ; it != m_Listeners.end( ); ++it )
         {
            if ( *it == *obj )
            {
               m_Listeners.erase( it );
               return;
            }
         }

         modified( );
      }

      void removeAll( )
      {
         DEBUGMACRO( "removeAll - this isn't deleting anything. fix?" );
         
         m_Listeners.clear( );

         modified( );
      }

   protected:
      std::vector<typename Ty::P> m_Listeners;
   };

   template <class ListenerType, class Actor>
   class CompositeListener : public ManagedObjectSet<ListenerType>
   {
   public:
      void execute( typename Actor::P actor, void (ListenerType::*funcPtr)( typename Actor::P ) )
      {
         std::vector<ListenerType>::iterator it = m_Listeners.begin( );
         for ( ; it != m_Listeners.end( ); ++it )
         {
            ((*it).*funcPtr)( actor );
         }
      }
   };
}

#endif // __MANAGEDOBJECTSET_H__