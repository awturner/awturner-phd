/*
* ################################################################################
* ### MIT License
* ################################################################################
* 
* Copyright (c) 2006-2011 Andy Turner
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
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