#ifndef __MESHSIMILARITY_H__
#define __MESHSIMILARITY_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   template <class T>
   class Mesh;

   template <class T>
   class MeshSimilarity : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MeshSimilarity<T>> P;

   protected:
      MeshSimilarity( typename Mesh<T>::P meshA, typename Mesh<T>::P meshB, const T aodThresh, const unsigned int subdivs, const bool inCorr );
      virtual ~MeshSimilarity( );

   public:
      static P getInstance( typename Mesh<T>::P meshA, typename Mesh<T>::P meshB, const T aodThresh, const unsigned int subdivs, const bool inCorr = false );

      std::string getClassName( ) const;

      T getMeanDistance( )    const;
      T getRmsDistance( )     const;
      T getMaximumDistance( ) const;
      T getAreaOfDeviation( ) const;

      T getAreaOfDeviationThreshold( ) const;
      unsigned int getNumberOfSubdivisions( ) const;

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __MESHSIMILARITY_H__