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
//#ifndef __SIMPLEVECTOROPS_H__
//#define __SIMPLEVECTOROPS_H__
//
//template <class T, unsigned int N>
//T dot(const T* in_Vec1, const T* in_Vec2)
//{
//   T ret = 0;
//
//   for (int i = 0; i < N; i++)
//      ret += in_Vec1[i]*in_Vec2[i];
//
//   return ret;
//}
//
//template <class T, unsigned int N>
//void normalize(T* io_Vec)
//{
//   // Multiplication is more efficient than division...
//   T len = static_cast<T>(1.0 / sqrt(dot<T,N>(io_Vec, io_Vec)));
//
//   for (int i = 0; i < N; i++)
//      io_Vec[i] *= len;
//}
//
//template <class T>
//void cross(T* in_Vec1, T* in_Vec2, T* out_Vec)
//{
//   out_Vec[0] = in_Vec1[1]*in_Vec2[2] - in_Vec1[2]*in_Vec2[1];
//   out_Vec[1] = in_Vec1[2]*in_Vec2[0] - in_Vec1[0]*in_Vec2[2];
//   out_Vec[2] = in_Vec1[0]*in_Vec2[1] - in_Vec1[1]*in_Vec2[0];
//}
//
//#endif // __SIMPLEVECTOROPS_H__