#ifndef __AWTSIMPLEDERIVATIVES_H__
#define __AWTSIMPLEDERIVATIVES_H__

namespace AWT
{
   template <class T>
   class SimpleDerivatives
   {
   public:
      static T firstDerivative( T in_v1, T in_v2 );
      static T firstDerivative( T in_v1, T in_v2, T in_spacing );

      static T secondDerivative( T in_v1, T in_v2, T in_v3 );
      static T secondDerivative( T in_v1, T in_v2, T in_v3, T in_spacing );

      static T crossDerivative( T in_v11, T in_v12, T in_v21, T in_v22 );
      static T crossDerivative( T in_v11, T in_v12, T in_v21, T in_v22, T in_spacing1, T in_spacing2 );
   };
}

#endif // __AWTSIMPLEDERIVATIVES_H__