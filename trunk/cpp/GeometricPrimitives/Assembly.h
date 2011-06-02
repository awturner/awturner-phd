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
         Assembly();
         virtual ~Assembly();

      public:
         Assembly<T,PartType>* getInstance();
         void addPart(PartType* primitive);
         void removePart(PartType* primitive);

         PartType*    getPart(unsigned int idx) const;
         unsigned int getNumberOfParts() const;

         Iterator<PartType*>* iterator() const;

         ModifiedTime getModifiedTime() const;

         virtual void refresh();

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
      Assembly<T,PartType>::Assembly()
      {
      }

      template <class T, class PartType>
      Assembly<T,PartType>::~Assembly()
      {
         for (unsigned int i = 0; i < parts.size(); ++i)
            parts[i]->unregisterUse();
      }

      template <class T, class PartType>
      void Assembly<T,PartType>::refresh()
      {
         for (unsigned int i = 0; i < parts.size(); ++i)
            parts[i]->refresh();
      }

      template <class T, class PartType>
      ModifiedTime Assembly<T,PartType>::getModifiedTime() const
      {
         ModifiedTime ret = this->mtime;

         for (unsigned int i = 0; i < parts.size(); ++i)
            ret = std::max<ModifiedTime>(ret, parts[i]->getModifiedTime());

         return ret;
      }

      template <class T, class PartType>
      PartType* Assembly<T,PartType>::getPart(unsigned int idx) const
      {
         return parts[idx];
      }

      template <class T, class PartType>
      unsigned int Assembly<T,PartType>::getNumberOfParts() const
      {
         return static_cast<unsigned int>(parts.size());
      }

      template <class T, class PartType>
      void Assembly<T,PartType>::addPart(PartType* primitive)
      {
         primitive->registerUse();
         parts.push_back(primitive);
         modified();
      }

      template <class T, class PartType>
      void Assembly<T,PartType>::removePart(PartType* primitive)
      {
         size_t i;
         for (i = 0; i < parts.size(); ++i)
         {
            if (parts[i] == primitive)
            {
               parts.erase(parts.begin()+i, parts.begin()+i+1);
               primitive->unregisterUse();
               break;
            }
         }
         modified();
      }

      template <class T, class PartType>
      Iterator<PartType*>* Assembly<T,PartType>::iterator() const
      {
         return VectorIterator<PartType*>::getInstance(parts);
      }
   }
}

#endif // __PRIMITIVE_ASSEMBLY_H__