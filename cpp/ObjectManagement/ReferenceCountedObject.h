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
#ifndef __X_REFERENCECOUNTEDOBJECT_H__
#define __X_REFERENCECOUNTEDOBJECT_H__

#include <map>
#include <string>

#include "Useful/PrintMacros.h"

#define STANDARDGETINSTANCE(Class, Args) { Class* ret = new Class##Args; ret->countMe(); return ret; }
#define GETNAMEMACRO(Class) std::string Class::getClassName() const { return #Class; }

namespace AWT
{
   typedef unsigned int ReferenceCount;

   class ReferenceCountedObject
   {
   public:
      ReferenceCount getReferenceCount() const;

      void keep(const void* obj = 0);
      void release(const void* obj = 0);

      void monitor();
      void unmonitor();

      virtual std::string getClassName() const = 0;

      void countMe();

      ReferenceCount getObjectCount(std::string& str);

      static void record();

      static void compare();

      static void reportOne(const std::string key);

      static void report(const bool hideZeros = true);

      static bool debug;

      template <class RefTy>
      static void swapRefCountedObjects(const void* /*obj*/, RefTy*& holder, RefTy* objNew, const bool = false)
      {
         if (debug) DEBUGLINE;

         if (holder != objNew)
         {
            RefTy* objOld = holder;
            
            if (debug) DEBUGLINE;

            if (objNew != 0)
            {
               objNew->keep();
            }
            if (debug) DEBUGLINE;
            
            holder = objNew;
            
            if (debug) DEBUGLINE;
            
            if (objOld != 0)
            {
               if (debug) DEBUGLINE;
               objOld->release();
            }
            if (debug) DEBUGLINE;
            
         }
         if (debug)
            DEBUGLINE;
      }
         
   protected:
      static void report(const std::string name, const ReferenceCount n);

      void uncountMe();

      ReferenceCountedObject();
      virtual ~ReferenceCountedObject();

   private:
      ReferenceCount m_ReferenceCount;
      bool m_Monitored;
      
      static std::map<std::string,ReferenceCount> m_InstanceMap;
      static std::map<std::string,ReferenceCount> m_InstanceMapRecord;
   };
}


#endif // __X_REFERENCECOUNTEDOBJECT_H__
