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
#include "ReferenceCountedObject.h"

#include "Useful/PrintMacros.h"

GETNAMEMACRO(AWT::ReferenceCountedObject);

namespace AWT
{
   // Keep track of objects
   std::map<std::string,ReferenceCount> AWT::ReferenceCountedObject::m_InstanceMap;
   std::map<std::string,ReferenceCount> AWT::ReferenceCountedObject::m_InstanceMapRecord;

   bool ReferenceCountedObject::debug;

   ReferenceCountedObject::ReferenceCountedObject()
   {
      m_ReferenceCount = 1;
      m_Monitored = false;
   }

   void AWT::ReferenceCountedObject::countMe()
   {
      // Look to see if its already in there
      std::map<std::string,ReferenceCount>::iterator it = m_InstanceMap.find(getClassName());
      if (it != m_InstanceMap.end())
         ++m_InstanceMap[getClassName()];
      else
         m_InstanceMap[getClassName()] = 1;

   }

   void AWT::ReferenceCountedObject::uncountMe()
   {
      try
      {
         //PRINTVBL(getClassName());
         if (!m_InstanceMap.empty())
            --m_InstanceMap[getClassName()];
      }
      catch (...)
      {
         //DEBUGLINE;
         //PRINTVBL(getClassName());
      }
   }

   ReferenceCountedObject::~ReferenceCountedObject()
   {
   }

   ReferenceCount ReferenceCountedObject::getReferenceCount() const
   {
      return m_ReferenceCount;
   }

   void ReferenceCountedObject::monitor()
   {
      m_Monitored = true;
   }

   void ReferenceCountedObject::unmonitor()
   {
      m_Monitored = false;
   }

   void ReferenceCountedObject::keep(const void*)
   {
      ++m_ReferenceCount;

      if (m_Monitored)
         DEBUGMACRO("Keep " << this->getClassName());
   }

   void ReferenceCountedObject::release(const void*)
   {
      ReferenceCount cnt = --m_ReferenceCount;

      if (m_Monitored)
         DEBUGMACRO("Release " << this->getClassName());

      if (cnt == 0)
      {
         uncountMe();
         delete this;
      }
   }

   void ReferenceCountedObject::record()
   {
   }

   void ReferenceCountedObject::compare()
   {
   }

   void ReferenceCountedObject::report(const std::string key, const ReferenceCount n)
   {
      std::cerr << n << "\t";

      if (key == "")
         std::cerr << "<unnamed>";
      else
         std::cerr << key;
      std::cerr << std::endl;
   }

   void ReferenceCountedObject::reportOne(const std::string key)
   {
      std::map<std::string,ReferenceCount>::iterator f = m_InstanceMap.find(key);

      if (f != m_InstanceMap.end())
      {
         report(f->first, f->second);
      }
      else
      {
         DEBUGMACRO("No instances of " << key << " found.");
      }
   }

   void ReferenceCountedObject::report(const bool hideZeros)
   {
      std::map<std::string,ReferenceCount>::iterator it = m_InstanceMap.begin();

      ReferenceCount totalObjects = 0;

      for (; it != m_InstanceMap.end(); ++it)
      {
         if (it->second == 0 && hideZeros)
            continue;

         totalObjects += it->second;

         report(it->first, it->second);
      }

      if (!hideZeros || totalObjects != 0)
      {
         std::cerr << totalObjects << " in total." << std::endl;
         std::cerr << "========" << std::endl;
      }
   }

   ReferenceCount ReferenceCountedObject::getObjectCount(std::string& str)
   {
      return m_InstanceMap[str];
   }
}