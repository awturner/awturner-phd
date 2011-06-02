#ifndef __SIMPLELINKEDLIST_H__
#define __SIMPLELINKEDLIST_H__

#include <vector>

namespace AWT
{
   template <class T>
   class SimpleLinkedList
   {
   public:
      SimpleLinkedList( T _data, SimpleLinkedList<T>* _next = 0 )
         : data( _data ), next( _next )
      {
         std::vector<int> foo;
         
      }

      const T data;
      SimpleLinkedList<T>* next;
   };


}

#endif // __SIMPLELINKEDLIST_H__