


#ifndef __AWTSTACKIMPL_H__
#define __AWTSTACKIMPL_H__

#include <iostream>

#include "Useful/Stack.h"
#include "Useful/Iterator.h"

namespace AWT
{
   namespace Container
   {
      template <class T>
      class StackImpl : public AWT::Container::Stack<T>
      {
      public:
         StackImpl( );
         ~StackImpl( );

         virtual const bool isEmpty( );
         virtual const int  size( );
         virtual const T    top( );

         virtual void    push( const T data );
         virtual const T pop( );

         virtual Iterator<T>* iterator( );

      protected:
         class LinkedListNode
         {
         public:
            LinkedListNode ( T _data ) : next( 0 ), data( _data ) { NodeCount++; }
            ~LinkedListNode ( )
            {
               NodeCount--;
               if ( next )
                  delete next;
            }

            LinkedListNode* push( const T data )
            {
               LinkedListNode* newNode = new LinkedListNode( data );
               newNode->next = this;

               return newNode;
            }

            LinkedListNode* pop( )
            {
               LinkedListNode* ret = this->next;
               this->next = 0;
               delete this;

               return ret;
            }

            const int size( )
            {
               int size = 0;
               AWT::Container::LinkedListNode<T>* node = this;

               while ( node != 0 )
               {
                  size++;
                  node = node->next;
               }

               return size;
            }

            static int NodeCount;

            const T            data;
            LinkedListNode* next;
         };

         virtual class StackImplIterator : public AWT::Container::Iterator<T>
         {
         public:
            StackImplIterator( StackImpl* p )
            {
               node = p->m_Top;
            }

            virtual bool hasNext( )
            {
               return node != 0;
            }

            virtual T    next( )
            {
               T ret = node->data;
               node = node->next;
               return ret;
            }

         private:
            LinkedListNode* node;
         };
      protected:
         LinkedListNode* m_Top;
         int             m_Size;
      };
   }
}

template <class T>
AWT::Container::Iterator<T>* AWT::Container::StackImpl<T>::iterator()
{
   return new AWT::Container::StackImpl<T>::StackImplIterator( this );
}

template <class T>
AWT::Container::StackImpl<T>::StackImpl( )
{
   m_Size = 0;
   m_Top = 0;
}

template <class T>
AWT::Container::StackImpl<T>::~StackImpl( )
{
   if ( m_Top != 0 )
      delete m_Top;
}

template <class T>
const bool AWT::Container::StackImpl<T>::isEmpty( )
{
   return m_Size == 0;
}

template <class T>
const int AWT::Container::StackImpl<T>::size( )
{
   return m_Size;
}

template <class T>
const T AWT::Container::StackImpl<T>::top( )
{
   return m_Top->data;
}

template <class T>
void AWT::Container::StackImpl<T>::push( const T data )
{
   m_Size++;
   m_Top = m_Top->push( data );
}

template <class T>
const T AWT::Container::StackImpl<T>::pop( )
{
   T ret = m_Top->data;
   m_Top = m_Top->pop( );
   m_Size--;

   return ret;
}

template <class T>
int AWT::Container::StackImpl<T>::LinkedListNode::NodeCount = 0;


#endif // __AWTSTACKIMPL_H__