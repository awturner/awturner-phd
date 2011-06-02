#ifndef __AREAAVERAGEDNORMALCALCULATOR_H__
#define __AREAAVERAGEDNORMALCALCULATOR_H__

#include "NormalCalculator.h"

namespace AWT
{
   template <class T>
   class AreaAveragedNormalCalculator : public NormalCalculator<T>
   {
   public:
      typedef ManagedAutoPointer<AreaAveragedNormalCalculator<T>> P;

   protected:
      AreaAveragedNormalCalculator( );
      virtual ~AreaAveragedNormalCalculator( );
   public:
      static P getInstance( );

      virtual void calculateNormals( typename Mesh<T>::P mesh, typename Tuples<T>::P vtxNormals, typename Tuples<T>::P meshNormals );

      virtual std::string getClassName( ) const;

   protected:
      struct D;
      D* m_D;
   };
};

#endif // __AREAAVERAGEDNORMALCALCULATOR_H__