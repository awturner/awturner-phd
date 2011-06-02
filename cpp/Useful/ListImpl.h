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
#ifndef __AWTLISTIMPL_H__
#define __AWTLISTIMPL_H__

#include "Useful/Iterator.h"
#include "AWTUseful/List.h"

namespace AWT
{
   namespace Container
   {
      template <class T>
      class ListImpl : public List<T>
      {
      public:
         ListImpl( );
         ~ListImpl( );

         virtual const bool isEmpty( );
         virtual const int size( );
         virtual const T get( int );

         virtual void add( const T );
         virtual void add( int, const T );

         virtual T remove( int );
         virtual void clear( );

         virtual Iterator<T>* iterator( );

      protected:
         class DoubleLinkedListNode
         {
         public:
            DoubleLinkedListNode( T _data ) : data( _data )
            {
               next = prev = 0;

               //std::cerr << "Created" << std::endl;
            }

            ~DoubleLinkedListNode( )
            {
               //std::cerr << "Destroyed" << std::endl;
            }

         public:
            DoubleLinkedListNode* next;
            DoubleLinkedListNode* prev;
            const T               data;
         };

         DoubleLinkedListNode* getIthNode( int idx )
         {
            DoubleLinkedListNode* node;
            int curSize = size( );

            // Wrap the index around

            while ( idx < 0 )
               idx += curSize;

            while ( idx >= curSize )
               idx -= curSize;

            if ( idx < curSize / 2 )
            {
               node = m_header->next;
               while ( idx > 0 )
               {
                  node = node->next;
                  idx--;
               }
            }
            else
            {
               idx = curSize - 1 - idx;
               node = m_header->prev;
               while ( idx > 0 )
               {
                  node = node->prev;
                  idx--;
               }
            }

            return node;
         }

         virtual class ListImplIterator : public AWT::Container::Iterator<T>
         {
         public:
            ListImplIterator( ListImpl* p )
            {
               this->p = p;
               node = p->m_header;
            }

            virtual bool hasNext( )
            {
               return node->next != p->m_header;
            }

            virtual T    next( )
            {
               node = node->next;
               return node->data;
            }

         private:
            ListImpl*             p;
            DoubleLinkedListNode* node;
         };

         DoubleLinkedListNode* m_header;
         int                   m_size;
      };
   }
}

template <class T>
AWT::Container::Iterator<T>* AWT::Container::ListImpl<T>::iterator( )
{
   return new ListImplIterator( this );
}

template <class T>
AWT::Container::ListImpl<T>::ListImpl( )
{
   m_size = 0;

   m_header = new DoubleLinkedListNode( 0 );
   m_header->next = m_header;
   m_header->prev = m_header;
}

template <class T>
AWT::Container::ListImpl<T>::~ListImpl( )
{
   clear( );
   delete m_header;
}

template <class T>
const int AWT::Container::ListImpl<T>::size( )
{
   return m_size;
}

template <class T>
void AWT::Container::ListImpl<T>::add(T data)
{
   DoubleLinkedListNode* newNode = new DoubleLinkedListNode( data );

   newNode->next = m_header;
   newNode->prev = m_header->prev;

   m_header->prev->next = newNode;
   m_header->prev = newNode;

   m_size++;
}

template <class T>
void AWT::Container::ListImpl<T>::add(int idx, T data)
{
   DoubleLinkedListNode* node = getIthNode( idx );
   DoubleLinkedListNode* newNode = new DoubleLinkedListNode( data );

   newNode->next = node;
   newNode->prev = node->prev;

   node->prev->next = newNode;
   node->prev = newNode;

   m_size++;
}

template <class T>
const T AWT::Container::ListImpl<T>::get( int idx )
{
   return getIthNode( idx )->data;
}

template <class T>
T AWT::Container::ListImpl<T>::remove( int idx )
{
   DoubleLinkedListNode* node = getIthNode( idx );
   T ret = node->data;

   node->next->prev = node->prev;
   node->prev->next = node->next;
   
   node->prev = node->next = 0;

   delete node;
   m_size--;

   return ret;
}

template <class T>
const bool AWT::Container::ListImpl<T>::isEmpty( )
{
   return m_size == 0;
}

template <class T>
void AWT::Container::ListImpl<T>::clear( )
{
   if ( isEmpty( ) )
      return;

   DoubleLinkedListNode* node = m_header->next;
   
   while ( node != m_header )
   {
      node = node->next;
      delete node->prev;
   }

   m_header->next = m_header->prev = m_header;

   m_size = 0;
}

#endif // __AWTLISTIMPL_H__