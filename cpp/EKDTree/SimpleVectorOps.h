//#ifndef __SIMPLEVECTOROPS_H__
//#define __SIMPLEVECTOROPS_H__
//
//template <class T, unsigned int N>
//T dot( const T* in_Vec1, const T* in_Vec2 )
//{
//   T ret = 0;
//
//   for ( int i = 0; i < N; i++ )
//      ret += in_Vec1[i]*in_Vec2[i];
//
//   return ret;
//}
//
//template <class T, unsigned int N>
//void normalize( T* io_Vec )
//{
//   // Multiplication is more efficient than division...
//   T len = static_cast<T>( 1.0 / sqrt( dot<T,N>( io_Vec, io_Vec ) ) );
//
//   for ( int i = 0; i < N; i++ )
//      io_Vec[i] *= len;
//}
//
//template <class T>
//void cross( T* in_Vec1, T* in_Vec2, T* out_Vec )
//{
//   out_Vec[0] = in_Vec1[1]*in_Vec2[2] - in_Vec1[2]*in_Vec2[1];
//   out_Vec[1] = in_Vec1[2]*in_Vec2[0] - in_Vec1[0]*in_Vec2[2];
//   out_Vec[2] = in_Vec1[0]*in_Vec2[1] - in_Vec1[1]*in_Vec2[0];
//}
//
//#endif // __SIMPLEVECTOROPS_H__