#ifndef __VTKAPPFRAMEWORK_H__
#define __VTKAPPFRAMEWORK_H__

class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkPolyData;
class vtkPolyDataAlgorithm;
class vtkActor;
class vtkPolyDataReader;

namespace AWT
{
   class VTKAppFramework
   {
   public:

      VTKAppFramework( );
      ~VTKAppFramework( );

      vtkRenderer*               getRenderer( );
      vtkRenderWindow*           getRenderWindow( );
      vtkRenderWindowInteractor* getRenderWindowInteractor( );

      vtkActor*                  addPolyData( vtkPolyData* );
      vtkActor*                  addPolyData( vtkPolyDataAlgorithm* );
      vtkActor*                  addPolyData( vtkPolyDataReader* );

      void setWindowSize( int w, int h );

      virtual void initialize( ) {}

      void start( );

   protected:
      vtkRenderer*               m_Renderer;
      vtkRenderWindow*           m_RenderWindow;
      vtkRenderWindowInteractor* m_RenderWindowInteractor;
   };
}

#endif // __VTKAPPFRAMEWORK_H__