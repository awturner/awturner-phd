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