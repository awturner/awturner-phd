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

#ifndef __AWTSMOOTHBINARYIMAGE_H__
#define __AWTSMOOTHBINARYIMAGE_H__

#include "vtkImageData.h"
#include "vtkSimpleImageToImageFilter.h"

#include "LinkedListNode.h"

class VTK_IMAGING_EXPORT vtkWhitakerSmoothBinary : public vtkSimpleImageToImageFilter
{
public:
   static vtkWhitakerSmoothBinary* New();
   vtkTypeRevisionMacro(vtkWhitakerSmoothBinary,vtkSimpleImageToImageFilter);
   
   vtkSetMacro(StoppingThreshold, double);
   vtkGetMacro(StoppingThreshold, double);

   vtkSetMacro(MaxIterations, int);
   vtkGetMacro(MaxIterations, int);

   vtkSetClampMacro(BandWidth, int, 2, 8);
   vtkGetMacro(BandWidth, int);

   vtkSetClampMacro(ProgressStep, int, 1, 10000000);
   vtkGetMacro(ProgressStep, int);
   
protected:
   vtkWhitakerSmoothBinary( );
   ~vtkWhitakerSmoothBinary( );

protected:
   virtual void SimpleExecute( vtkImageData* threshData, vtkImageData* phi );

   template <class IT>
   void vtkWhitakerSmoothBinaryExecute( vtkImageData* threshData, vtkImageData* phi, IT* forceInput );

   static void extrapolateValues( float* io_values, int in_x, int in_y, int in_z, int in_w, int in_h, int in_d );
   static inline void calculateXYZ( const int nn, const int area, const int w, int &x, int &y, int &z );

   template <class IT>
   void initializeOutputImage( vtkImageData* phi, vtkImageData* thresh, int npoints, IT* );

   template <class IT>
   LinkedListNode<int>* findBoundary( vtkImageData* threshData, int* dims, IT* );

   template <class IT>
   LinkedListNode<int>* findActiveSet( vtkImageData* threshData, int* dims, IT* );

   LinkedListNode<int>* markActiveSetImage( int nn, int c, int maxc, int step, 
      unsigned char* ptr, unsigned char val, LinkedListNode<int>* expandList );

   float calculateTimeStep( float* in_spacing );
   
   double StoppingThreshold;
   int    MaxIterations;
   int    BandWidth;
   int    ProgressStep;

private:
   vtkWhitakerSmoothBinary(const vtkWhitakerSmoothBinary&);  // Not implemented.
   void operator=(const vtkWhitakerSmoothBinary&);  // Not implemented.
};

#endif //__AWTSMOOTHBINARYIMAGE_H__