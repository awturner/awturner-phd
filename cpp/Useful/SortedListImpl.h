#ifndef __SortedListImpl_H__
#define __SortedListImpl_H__

#include <iostream>

#include "AWTUseful/SortedList.h"
#include "AWTUseful/Iterable.h"
#include "AWTUseful/Iterator.h"
#include "AWTUseful/MaxMin.h"

namespace AWT
{
   namespace Container
   {
      template <class T>
      class SortedListImpl /* : public SortedList<T>*/
      {
      public:
         SortedListImpl( );
         ~SortedListImpl( );

         virtual const bool isEmpty( );
         virtual const int size( );

         virtual void push( const T data );
         virtual T    pop( );

         virtual const int  depth( );

         friend std::ostream& operator<<(std::ostream& os, SortedListImpl& ls )
         {
            os << "Sorted List = (" << *ls.bst << ")";
            return os;
         }

      public:
         class BinarySearchTree
         {
         public:
            BinarySearchTree( BinarySearchTree* _parent, T _data ) : parent( _parent ),
               data( _data )
            {
               AllocatedNodes++;
               left  = 0;
               right = 0;
               col   = RED;
            }

            ~BinarySearchTree( )
            {
               AllocatedNodes--;
               //std::cerr << "There are still " << AllocatedNodes << " in existence" << std::endl;

               if ( left != 0 )
                  delete left;

               if ( left != 0 )
                  delete right;
            }

            friend std::ostream& operator<<(std::ostream& os, BinarySearchTree& bst )
            {
               os << "( ";
               if ( bst.left != 0 )
                  os << *bst.left << "  ";

               os << bst.data << "<" << bst.col << ">";

               if ( bst.right != 0 )
                  os << "  " << *bst.right;

               os << " )";
               return os;
            }

            static const int balanceFactor( BinarySearchTree* t )
            {
               if ( t == 0 )
                  return 0;

               return depth( t->right ) - depth( t->left );
            }

            static const int size( BinarySearchTree* t )
            {
               if ( t == 0 )
                  return 0;

               return 1 + size( t->left ) + size( t->right );
            }

            static const int depth( BinarySearchTree* t )
            {
               if ( t == 0 )
                  return 0;

               return 1 + AWT::MaxMin<int>::maximum( depth( t->left ), depth( t->right ) );
            }

            static bool isBST( BinarySearchTree* _tree )
            {
               if ( _tree == 0 )
                  return true;

               if ( !isBST( _tree->left ) || !isBST( _tree->right ) )
                  return false;

               if ( _tree->left != 0 && _tree->left->data >= _tree->data )
                  return false;

               if ( _tree->right != 0 && _tree->right->data < _tree->data )
                  return false;

               return true;
            }

            static BinarySearchTree* grandparent( BinarySearchTree* node )
            {
               return node->parent->parent;
            }

            static BinarySearchTree* uncle( BinarySearchTree* node )
            {
               BinarySearchTree* gp = grandparent( node );

               if ( gp == 0 )
                  return 0;

               if ( node->parent = gp->left )
                  return gp->right;
               else
                  return gp->left;
               
            }

            static BinarySearchTree* rotate_right( BinarySearchTree* q )
            {
               BinarySearchTree* p = q->left;

               // Change over the downward links
               q->left = p->right;
               p->right = q;

               // Make q's parent point to p
               if ( q->parent != 0 )
               {
                  if ( q->parent->left == q )
                     q->parent->left = p;
                  else
                     q->parent->right = p;
               }

               // Now change over the parent links
               p->parent = q->parent;
               q->parent = p;

               return p;
            }

            static BinarySearchTree* rotate_left( BinarySearchTree* p )
            {
               BinarySearchTree* q = p->right;

               // Change over the downward links
               p->right = q->left;
               q->left = p;

               // Make p's parent point to q
               if ( p->parent != 0 )
               {
                  if ( p->parent->left == p )
                     p->parent->left = q;
                  else
                     p->parent->right = q;
               }

               // Now change over the parent links
               q->parent = p->parent;
               p->parent = q;

               return q;
            }

            static void insertCase5( BinarySearchTree* n )
            {
               BinarySearchTree* gp = grandparent( n );

               if ( gp == 0 )
                  return;

               n->parent->col = BLACK;
               gp->col = RED;
               if (n == n->parent->left && n->parent == gp->left) {
                  rotate_right(gp);
               } else {
                  rotate_left(gp);
               }
            }

            static void insertCase4( BinarySearchTree* n )
            {
               if ( n->parent == 0 )
                  return;

               BinarySearchTree* gp = grandparent( n );

               if ( gp != 0 )
               {
                  if (n == n->parent->right && n->parent == gp->left) {
                     rotate_left(n->parent);
                     n = n->left;
                  } else if (n == n->parent->left && n->parent == gp->right) {
                     rotate_right(n->parent);
                     n = n->right;
                  }
               }
               insertCase5(n);
            }

            static void insertCase3( BinarySearchTree* node )
            {
               BinarySearchTree* gp = grandparent( node );
               BinarySearchTree* un = uncle( node );

               if ( un != 0 && un->col == RED )
               {
                  node->parent->col = BLACK;
                  un->col = BLACK;
                  gp->col = RED;

                  insertCase1( gp );
               }
               else
               {
                  insertCase4( node );
               }
            }

            static void insertCase2( BinarySearchTree* node )
            {
               if ( node->parent->col == BLACK )
               {
                  std::cerr << "2. The parent is black, so I can return" << std::endl;
                  return;
               }
               else
                  insertCase3( node );
            }

            static void insertCase1( BinarySearchTree* node )
            {
               if ( node->parent == 0 )
               {
                  std::cerr << "1. This is the root node, so make it black" << std::endl;
                  node->col = BLACK;
               }
               else
                  insertCase2( node );
            }

            static BinarySearchTree* insert( BinarySearchTree* tree, T _data )
            {
               BinarySearchTree* _prev = 0;
               BinarySearchTree* _tree = tree;

               while ( _tree != 0 )
               {
                  _prev = _tree;

                  if ( _data < _tree->data )
                     _tree = _tree->left;
                  else
                     _tree = _tree->right;
               }

               _tree = new BinarySearchTree( _prev, _data );
                  
               if ( _prev != 0 )
               {
                  if ( _data < _prev->data )
                     _prev->left = _tree;
                  else
                     _prev->right = _tree;

               }

               insertCase1( _tree );

               while ( _tree->parent != 0 )
                  _tree = _tree->parent;

               return _tree;
            }

            static BinarySearchTree* remove( BinarySearchTree* _tree )
            {
               T data                   = _tree->data;
               BinarySearchTree* parent = _tree->parent;
               BinarySearchTree* left   = _tree->left;
               BinarySearchTree* right  = _tree->right;

               _tree->left = _tree->right = 0;
               delete _tree;

               if ( left == 0 && right == 0 )
               {
                  return 0;
               }
               else if ( left == 0 && right != 0 )
               {
                  right->parent = parent;
                  return right;
               }
               else
               {
                  BinarySearchTree *newNode = new BinarySearchTree( parent, data );
                  newNode->left = remove( left );
                  newNode->right = right;

                  return newNode;
               }
            }

            
            enum Colour { RED, BLACK };

            T      data;
            Colour col;

            BinarySearchTree* parent;
            BinarySearchTree* left;
            BinarySearchTree* right;

         };

         static int AllocatedNodes;
         BinarySearchTree* bst;
         int m_Size;
      };
   }
}

template <class T>
int AWT::Container::SortedListImpl<T>::AllocatedNodes = 0;

template <class T>
AWT::Container::SortedListImpl<T>::SortedListImpl( )
{
   bst = 0;
   m_Size = 0;
}

template <class T>
AWT::Container::SortedListImpl<T>::~SortedListImpl( )
{
   delete bst;
}

template <class T>
const int AWT::Container::SortedListImpl<T>::depth( )
{
   return AWT::Container::SortedListImpl<T>::BinarySearchTree::depth( bst );
}

template <class T>
const bool AWT::Container::SortedListImpl<T>::isEmpty( )
{
   return m_Size == 0;
}

template <class T>
const int AWT::Container::SortedListImpl<T>::size( )
{
   return m_Size;
}

template <class T>
void AWT::Container::SortedListImpl<T>::push( const T data )
{
   m_Size++;
   
   bst = AWT::Container::SortedListImpl<T>::BinarySearchTree::insert( bst, data );
}

template <class T>
T    AWT::Container::SortedListImpl<T>::pop( )
{
   BinarySearchTree* leftMost = bst;

   while ( leftMost->left != 0 )
      leftMost = leftMost->left;

   T ret = leftMost->data;

   bst = AWT::Container::SortedListImpl<T>::BinarySearchTree::remove( bst );

   m_Size--;

   return ret;
}

#endif // __SortedListImpl_H__
