#include "AWTUseful/SimpleDerivatives.h"

template <class T>
T AWT::SimpleDerivatives<T>::firstDerivative( T in_v1, T in_v2 )
{
   return in_v2 - in_v1;
}

template <class T>
T AWT::SimpleDerivatives<T>::firstDerivative( T in_v1, T in_v2, T in_spacing )
{
   return firstDerivative( in_v1, in_v2 ) / in_spacing;
}

template <class T>
T AWT::SimpleDerivatives<T>::secondDerivative( T in_v1, T in_v2, T in_v3 )
{
   return in_v1 - 2*in_v2 + in_v3;
}

template <class T>
T AWT::SimpleDerivatives<T>::secondDerivative( T in_v1, T in_v2, T in_v3, T in_spacing )
{
   return secondDerivative( in_v1, in_v2, in_v3 ) / ( in_spacing * in_spacing );
}

template <class T>
T AWT::SimpleDerivatives<T>::crossDerivative(T in_v11, T in_v12, T in_v21, T in_v22, T in_spacing1, T in_spacing2 )
{
   return crossDerivative( in_v11, in_v12, in_v21, in_v22 ) / ( in_spacing1 * in_spacing2 );
}

template <class T>
T AWT::SimpleDerivatives<T>::crossDerivative(T in_v11, T in_v12, T in_v21, T in_v22 )
{
   return (( in_v22 - in_v21 ) - ( in_v12 - in_v11 ));
}

template AWT::SimpleDerivatives<double>;
template AWT::SimpleDerivatives<float>;
template AWT::SimpleDerivatives<long>;
template AWT::SimpleDerivatives<int>;
template AWT::SimpleDerivatives<unsigned char>;