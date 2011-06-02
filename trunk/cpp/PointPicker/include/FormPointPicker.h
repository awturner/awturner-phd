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
   FormPointPicker();
   virtual ~FormPointPicker();

   void notify();

public slots:
   void switchMouseMode(bool state);

   void load();
   void save();

   void view1();
   void view2();
   void view3();
   void view4();
   void view5();
   void view6();

protected:
   bool loadVTK(const QString& filename, AWT::Mesh<double>::P& mesh);
   bool loadLua(const QString& filename, AWT::Mesh<double>::P& mesh);

   PickedPoints m_PickedPoints;
   PointListModel* m_PointListModel;

   AWT::Tuples<double>::P m_Points;
   QString m_Filename;

   AWT::MoveCameraMouseBehaviour::P m_MoveCamera;
   AWT::PointPickMouseBehaviour::P m_PointPick;
};

#endif // __FORMPOINTPICKER_H__