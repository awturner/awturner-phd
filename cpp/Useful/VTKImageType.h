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
#ifndef __VTKIMAGETYPE_H__
#define __VTKIMAGETYPE_H__

namespace AWT
{
   template <class T>
   int getVTKImageType(T arg);
}

#define VTK_TYPE_SPECIALIZED_TEMPLATE(Ty, VTK_Ty) template <> int AWT::getVTKImageType(Ty) { return VTK_Ty; }

#include "vtkType.h"

VTK_TYPE_SPECIALIZED_TEMPLATE(float,          VTK_FLOAT);
VTK_TYPE_SPECIALIZED_TEMPLATE(double,         VTK_DOUBLE);
VTK_TYPE_SPECIALIZED_TEMPLATE(int,            VTK_INT);
VTK_TYPE_SPECIALIZED_TEMPLATE(unsigned int,   VTK_UNSIGNED_INT);
VTK_TYPE_SPECIALIZED_TEMPLATE(long,           VTK_LONG);
VTK_TYPE_SPECIALIZED_TEMPLATE(unsigned long,  VTK_UNSIGNED_LONG);
VTK_TYPE_SPECIALIZED_TEMPLATE(short,          VTK_SHORT);
VTK_TYPE_SPECIALIZED_TEMPLATE(unsigned short, VTK_UNSIGNED_SHORT);
VTK_TYPE_SPECIALIZED_TEMPLATE(unsigned char,  VTK_UNSIGNED_CHAR);
VTK_TYPE_SPECIALIZED_TEMPLATE(char,           VTK_CHAR);

#undef VTK_TYPE_SPECIALIZED_TEMPLATE

#endif // __VTKIMAGETYPE_H__