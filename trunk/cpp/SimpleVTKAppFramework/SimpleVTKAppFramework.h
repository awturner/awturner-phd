#ifndef __VTKAPPFRAMEWORK_H__
#define __VTKAPPFRAMEWORK_H__

class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;

namespace AWT
{
   class SimpleVTKAppFramework
   {
   public:

      SimpleVTKAppFramework( );
      ~SimpleVTKAppFramework( );

      vtkRenderer*               getRenderer( );
      vtkRenderWindow*           getRenderWindow( );
      vtkRenderWindowInteractor* getRenderWindowInteractor( );

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