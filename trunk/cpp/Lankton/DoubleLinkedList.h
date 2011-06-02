#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__

#include <ostream>

// http://24bytes.com/Double-linked-List.html

namespace AWT
{
   template <class T>
   class DLList;

   template <class T>
   class DLListNode
   {
   public:
      T value;           //value stored in the DLListNode 
      DLListNode<T>* next;          //pointer to next DLListNode 
      DLListNode<T>* prev;          //pointer to previous DLListNode 

      static void switchLists( DLListNode<T>* node, DLList<T>* listCurrent, DLList<T>* listNew );
   };

   template <class T>
   class DLList
   {
   public:
      DLListNode<T>* front;       //pointer to front of list   
      DLListNode<T>* back;        //pointer to back of list  

      DLList()
      {
         front = NULL;
         back  = NULL;
      }
      
      bool empty( ) const;
      unsigned int size( ) const;

      void insert( T value );
      void insert( DLListNode<T>* newNode );

      void remove(DLListNode<T>* newNode);

      void printListFront( std::ostream& os );
      void printListBack( std::ostream& os );

      void dispose( )
      {
         while ( front != NULL )
         {
            DLListNode<T>* node = front;
            remove( node );
            delete node;
         }
      }
   };

   template <class T>
   bool DLList<T>::empty( ) const
   {
      return front == NULL;
   }

   template <class T>
   unsigned int DLList<T>::size( ) const
   {
      unsigned int ret = 0;
      DLListNode<T>* node = front;
      while ( node != NULL )
      {
         ++ret;
         node = node->next;
      }
      return ret;
   }

   template <class T>
   void DLList<T>::insert( T value )
   {
      DLListNode<T>* newNode = new DLListNode<T>;
      newNode->value = value;

      // Always the last in the list, so there is never a next
      newNode->next  = NULL;

      insert( newNode );
   }

   template <class T>
   void DLList<T>::insert( DLListNode<T>* newNode )
   {
      if ( back == NULL )
      {
         // The list is currently empty
         newNode->prev = NULL;
         front = back = newNode;
      }
      else
      {
         // There is at least one element already in the list
         newNode->prev = back;
         back->next = newNode;
         back = newNode;
      }
   }

   //remove a perticular DLListNode 
   template <class T>
   void DLList<T>::remove(DLListNode<T>* nodeToRemove)
   {
      if ( nodeToRemove->prev != NULL )
         nodeToRemove->prev->next = nodeToRemove->next;

      if ( nodeToRemove->next != NULL )
         nodeToRemove->next->prev = nodeToRemove->prev;

      if ( nodeToRemove == front )
         front = nodeToRemove->next;

      if ( nodeToRemove == back )
         back = nodeToRemove->prev;

      // Disconnect this node
      nodeToRemove->next = nodeToRemove->prev = NULL;
   }

   //PrT the list from front 
   template <class T>
   void DLList<T>::printListFront( std::ostream& os )
   {
      DLListNode* curr2;
      curr2= this->front;
      os<<"\n-----\n";
      os<<"Queue\n";
      os<<"-----\n";
      //os<<"size:"<<getQueueSize()<<std::endl;
      while(curr2!=NULL)
      {
         os<<" |"<<curr2->value<<"|";
         curr2=curr2->next;
      }
      os<<std::endl;
   }// prT the Double Linked List from front


   // prT the Double Linked List from backwards
   template <class T>
   void DLList<T>::printListBack( std::ostream& os )
   {
      DLListNode* curr2;
      curr2= this->back;
      os<<"\n-----\n";
      os<<"Queue\n";
      os<<"-----\n";
      //os<<"size:"<<getQueueSize()<<std::endl;
      while(curr2!=NULL)
      {
         os<<" |"<<curr2->value<<"|";
         curr2=curr2->prev;
      }
      os<<std::endl;
   }// prT the Double Linked List from back

   template <class T>
   void DLListNode<T>::switchLists( DLListNode<T>* node, DLList<T>* listCurrent, DLList<T>* listNew )
   {
      listCurrent->remove( node );
      listNew->insert( node );
   }
}

#endif // __DOUBLELINKEDLIST_H__