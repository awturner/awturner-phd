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
#include "include/FormPointPicker.h"

#include "include/PointListModel.h"

#include "DrawableFramework/DrawableWorld.h"
#include "DrawableFramework/DrawableMesh.h"
#include "DrawableFramework/DrawablePoints.h"
#include "DrawableFramework/DrawableFactory.h"

#include "Mesh/AreaAveragedNormalCalculator.h"

#include "Mesh/TuplesImpl.h"
#include "Mesh/VTKMeshLoader.h"

#include <QMessageBox>
#include <QTextStream>

#include "LuaPlus.h"

using namespace AWT;

FormPointPicker::FormPointPicker()
{
   setupUi(this);

   wdgView->initialize(1, 1);

   connect(btnPlacePoint, SIGNAL(toggled(bool)), this, SLOT(switchMouseMode(bool)));

   connect(btnView1, SIGNAL(clicked()), this, SLOT(view1()));
   connect(btnView2, SIGNAL(clicked()), this, SLOT(view2()));
   connect(btnView3, SIGNAL(clicked()), this, SLOT(view3()));
   connect(btnView4, SIGNAL(clicked()), this, SLOT(view4()));
   connect(btnView5, SIGNAL(clicked()), this, SLOT(view5()));
   connect(btnView6, SIGNAL(clicked()), this, SLOT(view6()));

   connect(actionOpen, SIGNAL(triggered()), this, SLOT(load()));
   connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));

   actionOpen->setShortcut(tr("Ctrl+O"));
   actionSave->setShortcut(tr("Ctrl+S"));

   btnPlacePoint->setShortcut(tr("Ctrl+P"));

   //load();
   
}

FormPointPicker::~FormPointPicker()
{

}

#include "QtForm/include/OpenGLWidget.h"

void FormPointPicker::switchMouseMode(bool state)
{
   OpenGLWidget* widget = dynamic_cast<OpenGLWidget*>(*wdgView->getViewer(0));

   if (state)
      widget->setMouseBehaviour(m_PointPick);
   else
      widget->setMouseBehaviour(m_MoveCamera);
}

void FormPointPicker::notify()
{
   double pnt[3];
   m_PointPick->getLastPicked(pnt);

   QModelIndex idx = tblPointList->selectionModel()->currentIndex();
   if (idx.isValid())
   {
      m_PickedPoints[idx.row()].setPoint(pnt);
      m_Points->setPoint(idx.row(), pnt);
      m_PointListModel->notifyOfChange();

      btnPlacePoint->setChecked(false);
   }
}

#include <QFileDialog>

void FormPointPicker::load()
{
   QString filename = QFileDialog::getOpenFileName(this, "Select a file", QString(), "VTK files (*.vtk);;Lua files (*.lua)");

   if (filename.length() == 0)
      return;

   bool success = false;
   Mesh<double>::P mesh(0);

   if (filename.endsWith(".lua", Qt::CaseInsensitive))
      success = loadLua(filename, mesh);
   else if (filename.endsWith(".vtk", Qt::CaseInsensitive))
      success = loadVTK(filename, mesh);
   
   if (success == false)
   {
      QMessageBox::warning(this, tr("Load Error"), tr("Could not load %1").arg(filename));
      return;
   }

   typedef double T;

   m_Filename = filename;
   setWindowTitle(tr("Point Picker - %1").arg(m_Filename));

   AreaAveragedNormalCalculator<T>::getInstance()->calculateNormalsAndSet(mesh);

   DrawableWorld::P dworld = wdgView->getViewer(0)->getDrawableWorld();
   dworld->clear();
   DrawableMesh<T>::P dmesh = dworld->getFactory()->createMesh(mesh);
   dmesh->setMaterial(dworld->getFactory()->createColour(1.f, 0.5f, 0.5f, 1.f, true));
   
   dworld->add("Mesh", dmesh);

   m_Points = TuplesImpl<T>::getInstance(3, m_PickedPoints.size());
   for (int i = 0; i < m_PickedPoints.size(); ++i)
   {
      double vtx[3];
      m_PickedPoints[i].getPoint(vtx);
      m_Points->addPoint(vtx);
   }

   DrawablePoints<T>::P dpnts = dworld->getFactory()->createPoints(m_Points);
   dpnts->setPointSize(10.f);
   dpnts->setMaterial(dworld->getFactory()->createColour(0.f, 0.f, 1.f, 1.f, true));
   dworld->add("Display Points", dpnts);

   m_PointListModel = new PointListModel(m_PickedPoints);
   tblPointList->setModel(m_PointListModel);

   tblPointList->setAlternatingRowColors(true);

   for (int i = 0; i < 4; ++i)
      tblPointList->resizeColumnToContents(i);

   m_MoveCamera = MoveCameraMouseBehaviour::getInstance();
   m_PointPick = PointPickMouseBehaviour::getInstance();

   m_PointPick->setTarget(mesh);
   m_PointPick->setNotifiable(this);

   switchMouseMode(btnPlacePoint->isChecked());

   dynamic_cast<OpenGLWidget*>(*wdgView->getViewer(0))->resetView(true);
}

bool FormPointPicker::loadVTK(const QString& filename, AWT::Mesh<double>::P& mesh)
{
   typedef double T;
   mesh = VTKMeshLoader<T>::load(filename.toStdString().c_str());

   if (*mesh == 0)
      return false;

   // Static list, for now
   m_PickedPoints.clear();
   m_PickedPoints.push_back(PickedPoint(tr("asis")));
   m_PickedPoints.push_back(PickedPoint(tr("pubictubercle")));
   m_PickedPoints.push_back(PickedPoint(tr("ischialtuberosity")));
   m_PickedPoints.push_back(PickedPoint(tr("wallanterior")));
   m_PickedPoints.push_back(PickedPoint(tr("wallposterior")));
   m_PickedPoints.push_back(PickedPoint(tr("iliumface")));

   return true;
}

bool FormPointPicker::loadLua(const QString& filename, AWT::Mesh<double>::P& mesh)
{
   LuaStateOwner state;
   if (state->DoFile(filename.toStdString().c_str()) != 0)
      return false;

   LuaObject objFilename = state->GetGlobal("filename");

   if (!objFilename.IsString())
      return false;

   typedef double T;

   mesh = VTKMeshLoader<T>::load(objFilename.GetString());

   if (*mesh == 0)
      return false;

   m_PickedPoints.clear();
   LuaObject objPoints = state->GetGlobal("points");
   PRINTVBL(objPoints.GetTableCount());
   for (int i = 1; i <= objPoints.GetTableCount(); ++i)
   {
      LuaObject thisPoint = objPoints.GetByIndex(i);

      LuaObject objPointName = thisPoint.GetByIndex(1);
      PRINTVBL(objPointName.GetString());

      LuaObject objX = thisPoint.GetByIndex(2);
      LuaObject objY = thisPoint.GetByIndex(3);
      LuaObject objZ = thisPoint.GetByIndex(4);

      if (!objPointName.IsString())
      {
         DEBUGMACRO("No name for point " << i << "; skipping.");
      }
      else if (!objX.IsNumber() || !objY.IsNumber() || !objZ.IsNumber())
      {
         DEBUGMACRO("Could not read coordinates for point " << i << "; skipping.");
      }
      else
      {
         const T pos[] = { objX.GetNumber(), objY.GetNumber(), objZ.GetNumber() };

         m_PickedPoints.push_back(PickedPoint(tr(objPointName.GetString()), pos));
      }
   }

   return true;
}

void FormPointPicker::save()
{
   QString filename = QFileDialog::getSaveFileName(this, "Select a file", QString(), "Lua files (*.lua)");

   if (filename.length() == 0)
      return;

   QFile file(filename);
   if (!file.open(QIODevice::WriteOnly))
   {
      QMessageBox::critical(this, "Write failed!", "Could not open the file for writing.");
      return;
   }

   QTextStream ts(&file);
   ts << "filename = '" << m_Filename << "'\n";

   ts << "points = {\n";
   for (int i = 0; i < m_PickedPoints.size(); ++i)
   {
      double vtx[3];
      m_PickedPoints[i].getPoint(vtx);

      ts << "\t{ '" << m_PickedPoints[i].getName() << "', " <<vtx[0] << ", " << vtx[1] << ", " << vtx[2] << " },\n";
   }
   ts << "}\n";

   file.close();
}

void FormPointPicker::view1()
{
   double rot[] = { 0, 0, 0 };
   wdgView->getViewer(0)->setRotation(rot);
}

void FormPointPicker::view2()
{
   double rot[] = { -90, 0, 0 };
   wdgView->getViewer(0)->setRotation(rot);
}

void FormPointPicker::view3()
{
   double rot[] = { 0, 0, -90 };
   wdgView->getViewer(0)->setRotation(rot);
}

void FormPointPicker::view4()
{
   double rot[] = { -180, 0, 0 };
   wdgView->getViewer(0)->setRotation(rot);
}

void FormPointPicker::view5()
{
   double rot[] = { 90, 0, 0 };
   wdgView->getViewer(0)->setRotation(rot);
}

void FormPointPicker::view6()
{
   double rot[] = { 0, 0, 90 };
   wdgView->getViewer(0)->setRotation(rot);
}