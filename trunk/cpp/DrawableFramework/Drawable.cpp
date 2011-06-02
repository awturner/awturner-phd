#include "Drawable.h"

#include "Useful/Mutex.h"

/*
AWT::Mutex* AWT::Drawable::ms_Mutex = new AWT::Mutex( );

bool AWT::Drawable::acquireMutexIfNotLocked( void* obj )
{
   return ms_Mutex->tryAcquire( obj );
}

void AWT::Drawable::acquireMutex( void* obj )
{
   ms_Mutex->acquire( obj );
}

void AWT::Drawable::releaseMutex( void* obj )
{
   ms_Mutex->release( obj );
}
*/

struct AWT::Drawable::D
{
   ManagedObject::P m_TimeObject;
};

AWT::Drawable::Drawable( )
{
   m_D = new D;
   *m_D->m_TimeObject = 0;
}

AWT::Drawable::~Drawable( )
{
   delete m_D;
}

void AWT::Drawable::setTimeObject( AWT::ManagedObject::P obj )
{
   m_D->m_TimeObject = obj;
}

AWT::ManagedObject::P AWT::Drawable::getTimeObject( )
{
   return m_D->m_TimeObject;
}

AWT::ModifiedTime AWT::Drawable::getTimeObjectModified( )
{
   if ( *m_D->m_TimeObject != 0 )
      return m_D->m_TimeObject->getModifiedTime();
   else
      return this->getModifiedTime();
}