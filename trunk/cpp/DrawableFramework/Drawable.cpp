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