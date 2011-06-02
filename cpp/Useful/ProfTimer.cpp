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
#include "ProfTimer.h"


#ifdef _WIN32

// Windows specific
#include <windows.h>

#include "PrintMacros.h"

class AWT::ProfilingTimer::D
{
public:
   void start( );
   void stop( );

   double getDurationInSecs( );

protected:
   LARGE_INTEGER mTimeStart;
   LARGE_INTEGER mTimeStop;
};

void AWT::ProfilingTimer::D::start( )
{
   QueryPerformanceCounter(&mTimeStart);
}

void AWT::ProfilingTimer::D::stop( )
{
   QueryPerformanceCounter(&mTimeStop);
}

double AWT::ProfilingTimer::D::getDurationInSecs( )
{
   LARGE_INTEGER freq;
   QueryPerformanceFrequency(&freq);
   double duration = (double)(mTimeStop.QuadPart-mTimeStart.QuadPart)/(double)freq.QuadPart;
   return duration;
}

#else

#error "Not implemented for this platform!"

#endif

AWT::ProfilingTimer::ProfilingTimer( )
{
   m_D = new D;
}

AWT::ProfilingTimer::~ProfilingTimer( )
{
   delete m_D;
}

void AWT::ProfilingTimer::start( )
{
   m_D->start( );
}

void AWT::ProfilingTimer::stop( )
{
   m_D->stop( );
}

double AWT::ProfilingTimer::getDurationInSecs( )
{
   return m_D->getDurationInSecs( );
}

#include <string>

struct AWT::TimedBlock::D
{
   ProfilingTimer timer;
   std::string msg;
};

AWT::TimedBlock::TimedBlock( const char* msg )
{
   m_D = new D;

   m_D->msg = std::string( msg );
   m_D->timer.start();
}

AWT::TimedBlock::~TimedBlock( )
{
   m_D->timer.stop();
   const double time = m_D->timer.getDurationInSecs();

   DEBUGMACRO( m_D->msg << ": " << time << " seconds" );

   delete m_D;
}

double AWT::TimedBlock::getDurationInSecs( )
{
   return m_D->timer.getDurationInSecs();
}