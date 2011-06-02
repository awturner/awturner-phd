#ifndef __PRIMITIVE_ASSEMBLY_H__
#define __PRIMITIVE_ASSEMBLY_H__

#include "GeometricPrimitive.h"

#include "Useful/Iterator.h"
#include <vector>

namespace AWT
{
   template <class T>
   class Iterator;

   namespace GeometricPrimitives
   {
      template <class T, class PartType = GeometricPrimitive<T> >
      class Assembly : public GeometricPrimitive<T>
      {
      protected:
         Assembly( );
         virtual ~Assembly( );

      public:
         Assembly<T,PartType>* getInstance( );
         void addPart( PartType* primitive );
         void removePart( PartType* primitive );

         PartType*    getPart( unsigned int idx ) const;
         unsigned int getNumberOfParts() const;

         Iterator<PartType*>* iterator( ) const;

         ModifiedTime getModifiedTime( ) const;

         virtual void refresh( );

      protected:
         std::vector<PartType*> parts;
      };
   }
}

namespace AWT
{
   namespace GeometricPrimitives
   {
      template <class T, class PartType>
      Assembly<T,PartType>* Assembly<T,PartType>::getInstance()
      {
         return new Assembly();
      }

      template <class T, class PartType>
      Assembly<T,PartType>::Assembly( )
      {
      }

      template <class T, class PartType>
      Assembly<T,PartType>::~Assembly( )
      {
         for ( unsigned int i = 0; i < parts.size(); ++i )
            parts[i]->unregisterUse();
      }

      template <class T, class PartType>
      void Assembly<T,PartType>::refresh( )
      {
         for ( unsigned int i = 0; i < parts.size(); ++i )
            parts[i]->refresh();
      }

      template <class T, class PartType>
      ModifiedTime Assembly<T,PartType>::getModifiedTime() const
      {
         ModifiedTime ret = this->mtime;

         for ( unsigned int i = 0; i < parts.size(); ++i )
            ret = std::max<ModifiedTime>( ret, parts[i]->getModifiedTime() );

         return ret;
      }

      template <class T, class PartType>
      PartType* Assembly<T,PartType>::getPart( unsigned int idx ) const
      {
         return parts[idx];
      }

      template <class T, class PartType>
      unsigned int Assembly<T,PartType>::getNumberOfParts() const
      {
         return static_cast<unsigned int>( parts.size() );
      }

      template <class T, class PartType>
      void Assembly<T,PartType>::addPart( PartType* primitive )
      {
         primitive->registerUse();
         parts.push_back( primitive );
         modified();
      }

      template <class T, class PartType>
      void Assembly<T,PartType>::removePart( PartType* primitive )
      {
         size_t i;
         for ( i = 0; i < parts.size(); ++i )
         {
            if ( parts[i] == primitive )
            {
               parts.erase( parts.begin()+i, parts.begin()+i+1 );
               primitive->unregisterUse();
               break;
            }
         }
         modified();
      }

      template <class T, class PartType>
      Iterator<PartType*>* Assembly<T,PartType>::iterator( ) const
      {
         return VectorIterator<PartType*>::getInstance( parts );
      }
   }
}

#endif // __PRIMITIVE_ASSEMBLY_H__