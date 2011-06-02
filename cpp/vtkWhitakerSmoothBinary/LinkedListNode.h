
#include <iostream>


#ifndef __LINKEDLISTNODE_H__
#define __LINKEDLISTNODE_H__

template <class T>
class LinkedListNode
{
public:
   LinkedListNode<T> ( const T _data ) : next( 0 ), data( _data ) {}
   ~LinkedListNode<T> ( )
   {
      if ( next )
         delete next;
   }

   LinkedListNode<T>* push( T data );

   LinkedListNode<T>* pop( );

   const int size( );

   const T            data;
   LinkedListNode<T>* next;
};

template <class T>
LinkedListNode<T>* LinkedListNode<T>::push( const T data )
{
   LinkedListNode<T>* newNode = new LinkedListNode<T>( data );
   newNode->next = this;

   return newNode;
}

template <class T>
LinkedListNode<T>* LinkedListNode<T>::pop( )
{
   LinkedListNode<T>* ret = this->next;
   this->next = 0;
   delete this;

   return ret;
}

template <class T>
const int LinkedListNode<T>::size( )
{
   int size = 0;
   LinkedListNode<T>* node = this;

   while ( node != 0 )
   {
      size++;
      node = node->next;
   }

   return size;
}

#endif // __LINKEDLISTNODE_H__