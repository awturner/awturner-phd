
#ifndef ___AWTHEAP_H___
#define ___AWTHEAP_H___

#include <iostream>

namespace AWT
{
   namespace Container
   {
      template <class T>
      class Comparator
      {
      public:
         virtual const bool compare( const T a, const T b ) = 0;
      };

      template <class T>
      class DefaultComparator : public Comparator<T>
      {
      public:
         static const DefaultComparator* instance( )
         {
            if ( m_Instance == 0 )
               m_Instance = new DefaultComparator( );

            return m_Instance;
         }

         const bool compare( const T a, const T b )
         {
            return a < b;
         }
      private:
         // Make the constructor private
         DefaultComparator( ) {}

         static DefaultComparator* m_Instance;
      };

      template <class T>
      class Heap
      {
      public:
         Heap( int in_Capacity, Comparator<T>* in_comparator );
         Heap(int in_Capacity);
         ~Heap();

         void add( T in_obj );
         T    remove( );

         T    get( );

         bool isHeap();
         bool isEmpty();
         int  size();

         friend std::ostream& operator <<(std::ostream &os,const Heap<T> &obj)
         {
            int i;
            os << "[";
            for ( i = 0; i < obj.m_size; i++ )
            {
               if ( i != 0 )
                  os << ", ";

               os << obj.m_Data[i];
            }
            os << "]";

            return os;
         }

         static bool DefaultComparator( T a, T b )
         {
            return a < b;
         }

      protected:
         void Init( int in_Capacity, Comparator<T>* in_comparator );

         int  NextPowerOfTwo(int n);
         void Reallocate(int in_Capacity);

         void ReheapifyUp();
         void ReheapifyDown();
         void Swap(int a, int b);
         int  ParentOf(int a);

         Comparator<T>* m_Comparator;

         T*   m_Data;
         int  m_Capacity;
         int  m_size;

         
      };
   }
}

template <class T>
AWT::Container::DefaultComparator<T>* AWT::Container::DefaultComparator<T>::m_Instance = 0;

template <class T>
void AWT::Container::Heap<T>::Init( int in_Capacity, Comparator<T>* in_Comparator )
{
   m_size     = 0;
   m_Data     = 0;

   Reallocate( in_Capacity );

   m_Comparator = in_Comparator;
}

template <class T>
AWT::Container::Heap<T>::Heap( int in_Capacity, Comparator<T>* in_Comparator )
{
   Init( in_Capacity, in_Comparator );
}

template <class T>
AWT::Container::Heap<T>::Heap(int in_Capacity)
{
   Init( in_Capacity, &AWT::Container::DefaultComparator<T>::getInstance( ) );
}

template <class T>
bool AWT::Container::Heap<T>::isHeap( )
{
   for ( int i = size( )-1; i >= 0; i-- )
   {
      if ( 2*i + 1 < size( ) )
         if (m_Comparator->compare( m_Data[2*i + 1], m_Data[i]) )
         {
            //for ( i = 0; i < size( ); i++ )
            //   std::cerr << "### " << *m_Data[i] << std::endl;

            return false;
         }

      if ( 2*i + 2 < size( ) )
         if (m_Comparator->compare( m_Data[2*i + 2], m_Data[i]) )
         {
            //for ( i = 0; i < size( ); i++ )
            //   std::cerr << "~~~ " << *m_Data[i] << std::endl;

            return false;
         }
   }

   return true;
}

template <class T>
void AWT::Container::Heap<T>::add/*Push*/( T in_obj )
{
   if ( m_size == m_Capacity )
   {
      Reallocate( 2 * m_Capacity );
   }

   m_Data[m_size++] = in_obj;
   ReheapifyUp();
}

template <class T>
T AWT::Container::Heap<T>::remove/*Pop*/()
{
   T ret = m_Data[0];
   m_Data[0] = m_Data[--m_size];

   ReheapifyDown();
   return ret;
}

template <class T>
T AWT::Container::Heap<T>::get( )
{
   return m_Data[0];
}

template <class T>
void AWT::Container::Heap<T>::ReheapifyUp()
{
   int ptr = m_size - 1;
   int PoP;
   while ( ptr > 0 )
   {
      PoP = ParentOf(ptr);

      if ( m_Comparator->compare( m_Data[ptr], m_Data[PoP] ) )
         Swap( PoP, ptr );
      
      ptr = PoP;
   }
}

template <class T>
void AWT::Container::Heap<T>::ReheapifyDown()
{
   int ptr = 0;
   int kid;

   while ( (kid = (ptr << 1) + 1) < m_size )
   {
      //kid = 2 * ptr + 1;

      if ( ( kid < m_size - 1 ) && ( m_Comparator->compare( m_Data[kid + 1], m_Data[kid] ) ) )
      {
         ++kid;
      }

      if ( m_Comparator->compare( m_Data[ptr], m_Data[kid] ) )
      {
         break;
      }
      else
      {
         Swap( kid, ptr );
         ptr = kid;
      }
   }
}

template <class T>
void AWT::Container::Heap<T>::Swap(int a, int b)
{
   T tmp = m_Data[a];
   m_Data[a] = m_Data[b];
   m_Data[b] = tmp;
}

template <class T>
int AWT::Container::Heap<T>::ParentOf(int a)
{
   return (a - 1) / 2;
}

template <class T>
void AWT::Container::Heap<T>::Reallocate( int in_Capacity )
{
   int new_Capacity = NextPowerOfTwo(in_Capacity+1)-1;
   //std::cerr << "Creating array with capacity " << new_Capacity << std::endl;

   T* new_Data     = new T[new_Capacity];

   if ( m_Data != 0 )
   {
      for (int i = 0; i < m_size; i++)
      {
         new_Data[i] = m_Data[i];
      }
   }

   m_Data     = new_Data;
   m_Capacity = new_Capacity;
}

template <class T>
int AWT::Container::Heap<T>::NextPowerOfTwo(int n)
{
   int i = 1;
   while (i < n)
      i *= 2;

   return i;
}

template <class T>
AWT::Container::Heap<T>::~Heap()
{
   delete m_Data;
}

template <class T>
bool AWT::Container::Heap<T>::isEmpty()
{
   return m_size <= 0;
}

template <class T>
int AWT::Container::Heap<T>::size()
{
   return m_size;
}

#endif // ___AWT::Container::Heap_H___