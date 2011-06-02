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