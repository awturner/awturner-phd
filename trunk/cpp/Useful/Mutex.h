#ifndef __MUTEX_H__
#define __MUTEX_H__

namespace AWT
{
   class Mutex
   {
   public:
      Mutex( );
      virtual ~Mutex( );

      void acquire( void* obj );

      bool tryAcquire( void* obj );

      void release( void* obj );

      bool locked( ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __MUTEX_H__