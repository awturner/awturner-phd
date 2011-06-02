#ifndef __FORMPOINTPICKER_H__
#define __FORMPOINTPICKER_H__

#include "ui_FormPointPicker.h"
#include "include/PickedPoint.h"
#include "include/PointListModel.h"

#include "QtForm/include/MoveCameraMouseBehaviour.h"
#include "QtForm/include/PointPickMouseBehaviour.h"
#include "QtForm/include/Notifiable.h"

#include "Mesh/Tuples.h"
#include "Mesh/Mesh.h"

class FormPointPicker : public QMainWindow, public Ui::FormPointPicker, public Notifiable
{
   Q_OBJECT

public:
   FormPointPicker( );
   virtual ~FormPointPicker( );

   void notify( );

public slots:
   void switchMouseMode( bool state );

   void load( );
   void save( );

   void view1( );
   void view2( );
   void view3( );
   void view4( );
   void view5( );
   void view6( );

protected:
   bool loadVTK( const QString& filename, AWT::Mesh<double>::P& mesh );
   bool loadLua( const QString& filename, AWT::Mesh<double>::P& mesh );

   PickedPoints m_PickedPoints;
   PointListModel* m_PointListModel;

   AWT::Tuples<double>::P m_Points;
   QString m_Filename;

   AWT::MoveCameraMouseBehaviour::P m_MoveCamera;
   AWT::PointPickMouseBehaviour::P m_PointPick;
};

#endif // __FORMPOINTPICKER_H__