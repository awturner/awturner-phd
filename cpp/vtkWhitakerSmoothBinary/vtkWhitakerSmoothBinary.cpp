
#include "vtkWhitakerSmoothBinary.h"

//#include "AWTSmoothBinary/LinkedListNode.h"

#include "AWTUseful/MaxMin.h"
#include "AWTUseful/LevelSets.h"
#include "AWTUseful/SimpleDerivatives.h"
#include "Useful/Stack.h"

#include "vtkObjectFactory.h"
#include "vtkImageGradientMagnitude.h"

vtkCxxRevisionMacro(vtkWhitakerSmoothBinary, "$Revision: 95 $");
vtkStandardNewMacro(vtkWhitakerSmoothBinary);

inline void vtkWhitakerSmoothBinary::calculateXYZ( const int nn, const int area, 
                                                   const int w, int &x, int &y, 
                                                   int &z )
{
   z = nn / area;
   y = nn % area;
   x = y  % w;
   y /= w;
}

vtkWhitakerSmoothBinary::vtkWhitakerSmoothBinary( )
{
   this->SetMaxIterations( 50 );
   this->SetStoppingThreshold( 1e-3 );
   this->SetBandWidth( 5 );
   this->SetProgressStep( 1 );
}

vtkWhitakerSmoothBinary::~vtkWhitakerSmoothBinary( )
{
}

void vtkWhitakerSmoothBinary::extrapolateValues( float* io_values, int in_x, int in_y, int in_z, int in_w, int in_h, int in_d )
{
   int vi, vii;

   // Extrapolate in the x-direction if necessary
   if ( in_w == 1 )
   {
      // This is a degenerate case.  Copy the middle column to outer columns
      for ( vi = 1; vi < 27; vi += 3 )
         io_values[vi-1] = io_values[vi+1] = io_values[vi];
   }
   else
   {
      if ( in_x == 0 )
      {
         for ( vi = 1; vi < 27; vi += 3 )
            io_values[vi-1] = 2*io_values[vi] - io_values[vi+1];
      }
      else if ( in_x == in_w-1 )
      {
         for ( vi = 1; vi < 27; vi += 3 )
            io_values[vi+1] = 2*io_values[vi] - io_values[vi-1];
      }
   }

   // Extrapolate in the y-direction if necessary
   if ( in_h == 1 )
   {
      // This is a degenerate case.  Copy the middle column to outer columns
      for ( vii = 0; (vi = 3+vii+6*(vii/3)) < 27; vii++ )
         io_values[vi-3] = io_values[vi+3] = io_values[vi];

   }
   else
   {
      if ( in_y == 0 )
      {
         for ( vii = 0; (vi = 3+vii+6*(vii/3)) < 27; vii++ )
            io_values[vi-3] = 2*io_values[vi] - io_values[vi+3];
      }
      else if ( in_y == in_h-1 )
      {
         for ( vii = 0; (vi = 3+vii+6*(vii/3)) < 27; vii++ )
            io_values[vi+3] = 2*io_values[vi] - io_values[vi-3];
      }
   }
   
   // Extrapolate in the z-direction if necessary
   if ( in_d == 1 )
   {
      // This is a degenerate case.  Copy the middle column to outer columns
      for ( vi = 9; vi < 18; vi++ )
         io_values[vi-9] = io_values[vi+9] = io_values[vi];
   }
   else
   {
      if ( in_z == 0 )
      {
         for ( vi = 9; vi < 18; vi++ )
            io_values[vi-9] = 2*io_values[vi] - io_values[vi+9];
      }
      else if ( in_z == in_d-1 )
      {
         for ( vi = 9; vi < 18; vi++ )
            io_values[vi+9] = 2*io_values[vi] - io_values[vi-9];
      }
   }
}

float vtkWhitakerSmoothBinary::calculateTimeStep( float* in_spacing )
{
   float min_spacing = AWT::MaxMin<float>::minimum( in_spacing, 3 );

   return min_spacing * min_spacing / 6.f;
}

LinkedListNode<int>* vtkWhitakerSmoothBinary::markActiveSetImage(int nn, int c, 
                                                     int maxc, 
                                                     int step, unsigned char *ptr, 
                                                     unsigned char val,
                                                     LinkedListNode<int>* expandList
                                                     )
{
   if ( c > 0 && ptr[nn-step] < val )
   {
      expandList = expandList->push( nn - step );
      ptr[nn-step] = val;
   }

   if ( c < (maxc-1) && ptr[nn+step] < val )
   {
      expandList = expandList->push( nn + step );
      ptr[nn+step] = val;
   }

   return expandList;
}

template <class IT>
LinkedListNode<int>* vtkWhitakerSmoothBinary::findActiveSet( vtkImageData* threshData, 
                                                             int* dims,
                                                             IT* forceType )
{
   vtkDebugMacro( << "Finding narrow band of active points" );

   int nn, x, y, z, area, npoints;
   int xstep, ystep, zstep;

   vtkImageData* activeSet = vtkImageData::New( );
   activeSet->SetScalarTypeToUnsignedChar( );

   // This is dull preamble to get the size of the image dataset
   activeSet->SetExtent( 1, dims[0], 1, dims[1], 1, dims[2] );
   
   area    = dims[0] * dims[1];
   npoints = area * dims[2];

   xstep = 1;
   ystep = dims[0];
   zstep = area;

   // End of dull preamble

   unsigned char* activePtr = 0;
   
   // Firstly, make sure that the active set image is zeroed
   //std::cerr << "Zeroing image" << std::endl;
   activePtr = reinterpret_cast<unsigned char*>( activeSet->GetScalarPointer( ) );
   for ( nn = 0; nn < npoints; nn++, activePtr++ )
      *activePtr = 0;
   activePtr = 0;

   // Now find the boundary list
   LinkedListNode<int>* expandList = findBoundary( threshData, dims, forceType );
   
   // Next, go through the boundary list and mark all b vertices with markValue
   //std::cerr << "Marking seeds" << std::endl;

   unsigned char markValue = static_cast<unsigned char>( BandWidth );
   activePtr = reinterpret_cast<unsigned char*>( activeSet->GetScalarPointer( ) );

   LinkedListNode<int>* bNode      = expandList;
   while ( bNode != 0 )
   {
      nn = bNode->data;
      bNode = bNode->next;
      
      if ( activePtr[nn] != markValue )
         activePtr[nn] = markValue;
   }

   /*
   int idx;
   int steps[] = { 1, dims[0], dims[0]*dims[1] };
   IT* threshPtr = reinterpret_cast<IT*>( threshData->GetScalarPointer( ) );

   for ( int r = 0; r < 3; ++r )
   {
      int xmin = ( r == 0 ) ? 1 : 0;
      int ymin = ( r == 1 ) ? 1 : 0;
      int zmin = ( r == 2 ) ? 1 : 0;

      int xmax = dims[0] - ( r == 0 ) ? 1 : 0;
      int ymax = dims[1] - ( r == 1 ) ? 1 : 0;
      int zmax = dims[2] - ( r == 2 ) ? 1 : 0;

      for ( int z = zmin; z < zmax; ++z )
         for ( int y = ymin; y < ymax; ++y )
            for ( int x = xmin; x < xmax; ++x )
            {
               idx = x + dims[0]*( y + dims[1]*z );

               if ( threshPtr[ idx-steps[r] ] == threshPtr[ idx+steps[r] ] && threshPtr[idx] != threshPtr[ idx-steps[r] ] )
                  activePtr[idx] = BandWidth + 1;
            }
   }
   */

   //std::cerr << "Seeds marked." << std::endl;

   // Now, expandList should contain a list of vertices without repeats.
   // These need to be expanded outwards
   LinkedListNode<int>* updateList = 0;
   unsigned char val;

   //std::cerr << "Expanding boundary..." << std::endl;
   while ( expandList != 0 )
   {
      nn = expandList->data;

      expandList = expandList->pop( );

      val = activePtr[nn] - 1;
      
      if ( val > 0 )
      {
         updateList = updateList->push( nn );
       
         calculateXYZ( nn, area, dims[0], x, y, z );
         expandList = markActiveSetImage( nn, z, dims[2], zstep, activePtr, val, expandList );
         expandList = markActiveSetImage( nn, y, dims[1], ystep, activePtr, val, expandList );
         expandList = markActiveSetImage( nn, x, dims[0], xstep, activePtr, val, expandList );
      }
   }
   activePtr = 0;
   activeSet->Delete( );

   delete expandList;

   return updateList;
}

template <class IT>
LinkedListNode<int>* vtkWhitakerSmoothBinary::findBoundary( vtkImageData* threshData, 
                                                           int* dims,
                                                           IT* )
{
   // Works out the magnitude of the gradient of the binary image...
   // this should only be non-zero 1 pixel either side of the boundary.

   IT* pData = reinterpret_cast<IT*>( threshData->GetScalarPointer( ) );

   LinkedListNode<int>* expandList = new LinkedListNode<int>( 0 );

   int x, y, z, idx;
   std::cerr << "Looking for the boundary..." << std::endl;

   // This method never checks pixel (0,0,0) for boundary, but should check
   // all others, hence the slight hack of adding (0) to the expandList at
   // creation
   for ( z = 1; z < dims[2]; ++z )
   {
      for ( y = 1; y < dims[1]; ++y )
      {
         for ( x = 1; x < dims[0]; ++x )
         {
            idx = z*dims[0]*dims[1] + y*dims[0] + x;

            if ( pData[idx] != pData[idx-1] )
               expandList = expandList->push( idx );
            else if ( pData[idx] != pData[idx-dims[0]] )
               expandList = expandList->push( idx );
            else if ( pData[idx] != pData[idx-dims[0]*dims[1]] )
               expandList = expandList->push( idx );
         }
      }
   }
   return expandList;
}

template <class IT>
void vtkWhitakerSmoothBinary::initializeOutputImage( vtkImageData* phi, 
                                                     vtkImageData* threshData, 
                                                     int npoints,
                                                     IT* )
{
   // This is the pointer into the output image
   float* phiPtr    = reinterpret_cast<float*>( phi->GetScalarPointer( ) );

   // This is the pointer into the input (i.e. threshold) image
   IT* threshPtr = reinterpret_cast<IT*>( threshData->GetScalarPointer( ) );

   // This is the first value in the threshold image
   const IT firstThreshValue = *threshPtr;

   for ( int nn = 0; nn < npoints; nn++, threshPtr++, phiPtr++ )
   {
      *phiPtr = ( *threshPtr == firstThreshValue ) ? -1.f : 1.f;
   }
}

void vtkWhitakerSmoothBinary::SimpleExecute( vtkImageData* input, vtkImageData* output )
{
   void* inPtr = input->GetScalarPointer();

   switch( input->GetScalarType( ) )
   {
      vtkTemplateMacro(
         vtkWhitakerSmoothBinaryExecute( input, output,
                                             (VTK_TT *)(inPtr)));
   default:
      vtkGenericWarningMacro("Execute: Unknown input ScalarType");
      return;
    }
}

template <class IT>
void vtkWhitakerSmoothBinary::vtkWhitakerSmoothBinaryExecute( vtkImageData* threshData, 
         vtkImageData* phi, IT* forceInput )
{
   int abortExecute = this->GetAbortExecute( );

   double _spacing[3];
   threshData->GetSpacing( _spacing );
   float spacing[3];

   spacing[0] = static_cast<float>( _spacing[0] );
   spacing[1] = static_cast<float>( _spacing[1] );
   spacing[2] = static_cast<float>( _spacing[2] );

   int* extents = threshData->GetExtent( );
   const int w = 1 + extents[1] - extents[0];
   const int h = 1 + extents[3] - extents[2];
   const int d = 1 + extents[5] - extents[4];

   int dims[3] = { w, h, d };

   phi->Initialize( );
   phi->SetScalarTypeToFloat( );
   phi->SetSpacing( _spacing );
   phi->SetExtent( extents );
   phi->SetOrigin( threshData->GetOrigin( ) );

   // This is the area of each z-slice
   const int area = w * h;
   // This is the number of points in total
   const int npoints = area * d;

   const int xstep = 1;
   const int ystep = w;
   const int zstep = area;

   int x, y, z;
   int nn;

   float*         phiPtr;

   // This is the L1 norm from the threshold boundary to
   // the edge of the active set.
   
   float* updatePhi = 0;

   // Doing things from here onwards...

   initializeOutputImage( phi, threshData, npoints, forceInput );

   LinkedListNode<int>* updateList = findActiveSet( threshData, dims, forceInput );

   unsigned int idx;

   int updateCnt = updateList->size( );

   updatePhi = new float[ updateCnt ];

   // This is a local store of the current phi in the
   // neighbourhood.
   float* values = new float[27];

   LinkedListNode<int>* linkedListPtr;
   
   float dt = calculateTimeStep( spacing );
   float delta;
   float rmsChange;
   float clampValue;

   float fNumberOfPoints = updateCnt;
   float HH;
   bool wasPositive;

   int vi, vii;

   //std::cerr << "npoints = " << npoints << std::endl;

   vtkDebugMacro( << "Performing the smoothing over " << updateCnt << " points." );
   //std::cerr << "Doing the level set magic over " << (updateCnt) << " points..." << std::endl;
   phiPtr = reinterpret_cast<float*>( phi->GetScalarPointer( ) );
   for ( int iteration = 0; !abortExecute && iteration < MaxIterations; )
   {
      if ( ++iteration%ProgressStep == 0 )
      {
         vtkDebugMacro(<<"Iteration #" << iteration);
         this->UpdateProgress ((double)iteration/MaxIterations);
         abortExecute = this->GetAbortExecute();
      }

      rmsChange = 0.f;

      // First calculate all of the updates
      
      linkedListPtr = updateList;
      for ( idx = 0; linkedListPtr != 0; idx++ )
      {
         nn = linkedListPtr->data;
         linkedListPtr = linkedListPtr->next;
      
         calculateXYZ( nn, area, w, x, y, z );

		   for ( vi = 0; vi < 27; vi++ )
         {
            vii = nn + zstep*( vi/9 - 1 ) + ystep*( (vi/3) % 3 - 1 ) 
               + xstep*( vi % 3 - 1 );

            if ( vii >= 0 && vii < w*h*d )
               values[vi] = *( phiPtr + vii );
         }

         // Instead of numerous boundary cases, want to do calculate away invalid
         // values by assuming piecewise linearity

         extrapolateValues( values, x, y, z, w, h, d );

         // So now, the gradient calculation is much more straightforward.

         HH = AWT::LevelSets<float>::meanCurvatureTimesGradientMagnitude( values, spacing );
         updatePhi[idx] = values[13];

         if ( HH != 0.f )
         {
            delta = dt * HH;
            
            // Check out to see if the current value is >= or < zero
            wasPositive = ( updatePhi[idx] >= 0 );

            // and update the update array
            updatePhi[idx] += delta;

            // But, if phi has changed sign, we need to clamp it back
            // to its original sign.  Clamping to a small value either
            // side of zero guarantees that the surface doesn't move by
            // more than 1 voxel.

            if ( (updatePhi[idx] >= 0 ) ^ wasPositive )
            {
               clampValue = wasPositive ? 1e-6 : -1e-6;
               delta = values[13] - clampValue;
               updatePhi[idx] = clampValue;

               HH = delta / dt;
            }

            rmsChange += delta*delta;
         }
      }

      // Then go back to phi and update all the values from the array
      linkedListPtr = updateList;
      for ( idx = 0; linkedListPtr != 0; idx++ )
      {
         nn = linkedListPtr->data;
         linkedListPtr = linkedListPtr->next;
      
         *( phiPtr + nn ) = updatePhi[idx];
      }

      rmsChange = sqrtf( rmsChange / fNumberOfPoints );
      //std::cerr << "RMS Change = " << rmsChange << std::endl;

      if ( rmsChange < StoppingThreshold )
         break;
   }

   this->UpdateProgress( 1.0 );
   vtkDebugMacro( << "Smoothing completed" );
   //std::cerr << "Done." << std::endl;
   phiPtr = 0;

   while ( updateList != 0 )
      updateList = updateList->pop( );

   delete values;
   delete updatePhi;

   //std::cerr << "returning" << std::endl;

   phi->Squeeze( );
}
