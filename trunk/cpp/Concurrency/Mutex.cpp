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
#include "Mutex.h"

#include "Useful/PrintMacros.h"

#include "ObjectManagement/ManagedObject.h"

#ifdef _WIN32

// Windows specific code

#include <windows.h>

struct AWT::Mutex::D
{
   void*        m_Obj;
   HANDLE       m_Mutex;
   unsigned int m_RecurseDepth;
};

AWT::Mutex::Mutex( )
{
   m_D = new D;

   m_D->m_Obj = 0;
   m_D->m_RecurseDepth = 0;

   m_D->m_Mutex = CreateMutex( NULL, FALSE, NULL );
}

AWT::Mutex::~Mutex( )
{
   delete m_D;
}

bool AWT::Mutex::acquireIfNotLocked( void* obj )
{
   DWORD dwWaitResult = WaitForSingleObject( m_D->m_Mutex, 0x00 );

   switch ( dwWaitResult )
   {
   case WAIT_OBJECT_0:
      // Then lock it
      m_D->m_Obj = obj;

      ++m_D->m_RecurseDepth;

      return true;
   case WAIT_TIMEOUT:
      // Don't lock it if we timed out
      return false;
   default:
      return false;
   }
}

void AWT::Mutex::acquire( void* obj )
{
   //while ( m_D->m_Obj != 0 );

   // Make the windows-specific call
   DWORD dwWaitResult = WaitForSingleObject( m_D->m_Mutex, INFINITE );

   switch ( dwWaitResult )
   {
   case WAIT_OBJECT_0:
      // Then lock it
      m_D->m_Obj = obj;

      ++m_D->m_RecurseDepth;

      break;
   }
   
}

void AWT::Mutex::release( void* obj )
{
   // Release the mutex if this is the locking object
   if ( m_D->m_Obj == obj )
   {
      if ( ReleaseMutex( m_D->m_Mutex ) )
      {
         if ( --m_D->m_RecurseDepth == 0 )
            m_D->m_Obj = 0;
      }
      else
      {
         std::cerr << "\t" << "I can't release the windows mutex!" << m_D->m_Obj;
         std::cerr << std::endl;
      }
   }
   else
   {
      std::cerr << "\t" << "I can't release mutex " << this << " to " << obj << " because it is held by " << m_D->m_Obj << std::endl;
   }
}

bool AWT::Mutex::locked( ) const
{
   return m_D->m_Obj != 0 ;
}

#else

#error "Not implemented on this platform!"

#endif