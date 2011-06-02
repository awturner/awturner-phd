#ifndef __CLEANEDVTKPOLYDATA_H__
#define __CLEANEDVTKPOLYDATA_H__

class vtkPolyData;
class vtkPolyDataAlgorithm;

namespace AWT
{
   class CleanedVtkPolyData
   {
   public:
      CleanedVtkPolyData( vtkPolyData* in_Poly );
      CleanedVtkPolyData( vtkPolyDataAlgorithm* in_PolyAlgo );

      ~CleanedVtkPolyData( );

      vtkPolyData* getCleanedPolyData( );

   protected:
      void init( vtkPolyData* in_Poly );

      vtkPolyData* m_CleanedData;
   };
}

#endif // __CLEANEDVTKPOLYDATA_H__