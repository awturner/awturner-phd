#ifndef __PROFTIMER_H__
#define __PROFTIMER_H__

namespace AWT
{
   class ProfilingTimer
   {
   public:
      ProfilingTimer( );
      virtual ~ProfilingTimer( );

      void start( );
      void stop( );

      double getDurationInSecs( );

   protected:
      struct D;

      // Opaque pointer to hide platform-specific workings
      D* m_D;
   };

   class TimedBlock
   {
   public:
      TimedBlock( const char* msg );
      virtual ~TimedBlock( );

      double getDurationInSecs( );

   protected:
      struct D;
      D* m_D;
   };

   typedef ProfilingTimer ProfTimer;
}


#endif // __PROFTIMER_H__
