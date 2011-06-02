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
#include "include/OpenGLWidget.h"

#include <QFileDialog>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMenu>
#include <QMutex>
#include <QTimer>

#include "Useful/PrintMacros.h"
#include "Useful/ArrayVectorFunctions.h"

#include "Maths/Constants.h"

#include "OpenGLDrawables/OpenGLDrawableBackground.h"
#include "OpenGLDrawables/OpenGLDrawableLight.h"
#include "OpenGLDrawables/OpenGLDrawableWorld.h"

#include "DrawableFramework/DrawableMesh.h"
#include "MeshSearching/FacesRayIntersectionSearch.h"

#include <cmath>

#include "Useful/Mutex.h"

using namespace AWT;

struct AWT::OpenGLWidget::D
{
   static int modifiers(QMouseEvent* ev)
   {
      int keys = 0;
      Qt::KeyboardModifiers mods = ev->modifiers();
      if (mods & Qt::ShiftModifier  ) keys |= 0x1;
      if (mods & Qt::ControlModifier) keys |= 0x2;

      return keys;
   }

   void setRotation(const double* v)
   {
      for (unsigned int ax = 0; ax < 3; ++ax)
         m_Rotation[ax] = v[ax];
   }

   void setOrbitPoint(const double* v)
   {
      for (unsigned int ax = 0; ax < 3; ++ax)
         m_Orbit[ax] = v[ax];
   }

   void setVisibleRadius(const double r)
   {
      // Clamp it
      double cr = std::min<double>(std::max<double>(r, 0.001), 1000);

      double fovy = m_FovY;
      double fovx = fovy * m_WindowSize[0] / m_WindowSize[1];

      double rx = cr / sin(fovx * AWT::Constants<double>::DegreesToRadians / 2.0);
      double ry = cr / sin(fovy * AWT::Constants<double>::DegreesToRadians / 2.0);

      setDistance(std::max(rx, ry));
   }

   void setDistance(const double v)
   {
      m_PullBack = log10(std::min<double>(std::max<double>(v, 0.001), 1000));
   }

   bool updateEyePosition()
   {
      // The centre of the camera is projected to (0,0,0) by the modelview matrix
      double matx[16];
      glGetDoublev(GL_MODELVIEW_MATRIX, matx);

      // Invert this matrix implicitly
      double eye[4];

      eye[0] = - matx[0]*matx[12] - matx[1]*matx[13] - matx[ 2]*matx[14];
      eye[1] = - matx[4]*matx[12] - matx[5]*matx[13] - matx[ 6]*matx[14];
      eye[2] = - matx[8]*matx[12] - matx[9]*matx[13] - matx[10]*matx[14];
      eye[3] = 1;

      return updateIfNecessary(eye, m_Eye, 4);
   }

   bool updateMousePosition(int x, int y)
   {
      GLdouble mv[16], proj[16];
      GLint viewport[4];
      glGetDoublev(GL_MODELVIEW_MATRIX, mv);
      glGetDoublev(GL_PROJECTION_MATRIX, proj);
      glGetIntegerv(GL_VIEWPORT, viewport);

      double winCoords[] = { x, y, 0 };
      double worldCoords[3];

      gluUnProject(winCoords[0], winCoords[1], winCoords[2], mv, proj, viewport, &worldCoords[0], &worldCoords[1], &worldCoords[2]);

      if (updateIfNecessary(worldCoords, m_WorldCoords, 3))
      {
         std::vector<MouseListener::P>::iterator it = m_MouseListeners.begin();
         std::vector<MouseListener::P>::iterator en = m_MouseListeners.end();

         for (; it != en; ++it)
            (*it)->mouseChanged(m_Eye, m_WorldCoords, m_MouseState);

         return true;
      }
      else
      {
         return false;
      }
   }

   template <class T>
   bool updateIfNecessary(const T* source, T* dest, const unsigned int n)
   {
      bool ret = false;

      for (unsigned int i = 0; i < n; ++i)
      {
         if (source[i] != dest[i])
         {
            dest[i] = source[i];
            ret = true;
         }
      }

      return ret;
   }

   QTimer* repaintTimer;

   DrawableWorld::P m_DrawableWorld;

   double m_Orbit[3];

   double m_Rotation[3];
   double m_PullBack;

   int    m_MouseState;

   double m_OrbitDown[3];
   double m_RotationDown[3];

   double m_PullBackDown;
   int    m_MouseXDown, m_MouseYDown;

   bool   m_ViewRelativeLighting;

   double m_Eye[4];

   QFont         m_Font;
   QFontMetrics* m_FontMetrics;

   GLdouble m_ZNear;
   GLdouble m_ZFar;
   GLdouble m_FovY;

   double m_WorldCoords[3];
   double m_WindowSize[2];

   bool m_MouseInside;

   std::vector<MouseListener::P> m_MouseListeners;

   QAction* m_ActionCentreView;
   QAction* m_ActionBackground[3];
   QAction* m_ActionScreenshot;

   bool m_CancelContextMenu;

   bool m_FirstDraw;

   OpenGLWidgetMouseBehaviour::P m_MouseBehaviour;

   QTimer* m_Timer;
   QMutex* m_Mutex;
};

AWT::OpenGLWidget::OpenGLWidget(QWidget* parent, QMutex* mutex)
   : QGLWidget(parent)
{
   m_D = new D;

   m_D->m_FirstDraw = true;

   m_D->m_Mutex = mutex;

   m_D->m_ActionCentreView = new QAction(tr("Centre View"), this);
   connect(m_D->m_ActionCentreView, SIGNAL(triggered()), this, SLOT(resetView()));

   m_D->m_ActionBackground[0] = new QAction(tr("User"), this);
   m_D->m_ActionBackground[0]->setCheckable(true);
   connect(m_D->m_ActionBackground[0], SIGNAL(triggered()), this, SLOT(changeToUserBackground()));

   m_D->m_ActionBackground[1] = new QAction(tr("White"), this);
   m_D->m_ActionBackground[1]->setCheckable(true);
   connect(m_D->m_ActionBackground[1], SIGNAL(triggered()), this, SLOT(changeToWhiteBackground()));

   m_D->m_ActionBackground[2] = new QAction(tr("Black"), this);
   m_D->m_ActionBackground[2]->setCheckable(true);
   connect(m_D->m_ActionBackground[2], SIGNAL(triggered()), this, SLOT(changeToBlackBackground()));

   m_D->m_ActionScreenshot = new QAction(tr("Screenshot"), this);
   connect(m_D->m_ActionScreenshot, SIGNAL(triggered()), this, SLOT(takeScreenshot()));

   m_D->m_MouseInside = false;

   m_D->m_ZNear  = 0.01;
   m_D->m_ZFar   = 1000;
   m_D->m_FovY   = 45;

   m_D->m_FontMetrics = new QFontMetrics(m_D->m_Font);

   m_D->m_ViewRelativeLighting = true;

   //countMe();

   // Set up the mouse interaction
   setMouseTracking(true);
   m_D->m_MouseState = 0;

   for (int i = 0; i < 3; ++i)
   {
      m_D->m_Orbit[i] = 0;
      m_D->m_Rotation[i] = 0;
      m_D->m_RotationDown[i] = 0;
   }

   m_D->m_PullBack = log10(200.0);
   
   m_D->m_DrawableWorld = OpenGLDrawableWorld::getInstance(DrawContext::P(this));
   
   {
      OpenGLDrawableBackground::P background = OpenGLDrawableBackground::getInstance();
      GLfloat botCol[] = { 0.7f, 0.8f, 0.9f };
      GLfloat topCol[] = { 0.3f, 0.4f, 0.5f };
      background->setTopColor(topCol);
      background->setBottomColor(botCol);

      m_D->m_DrawableWorld->setBackground(background);
   }

   // Set up the lighting
   {
#define DEFINELIGHT(v) { v, v, v, 1.f }
      GLfloat ambientLight[]  = DEFINELIGHT(0.f);
      GLfloat diffuseLight[]  = DEFINELIGHT(1.f);
      GLfloat specularLight[] = DEFINELIGHT(1.f);
#undef DEFINELIGHT

      GLfloat position[]      = { 0.f,  0.0f, 80.0f, 1.0f };

      Light::P light = AWT::Light::getInstance();
      light->setAmbient(ambientLight);
      light->setDiffuse(diffuseLight);
      light->setSpecular(specularLight);
      light->setPosition(position);
      m_D->m_DrawableWorld->setLight(light);
   }
}

AWT::OpenGLWidget::~OpenGLWidget()
{
   if (m_D->m_Mutex == 0)
      m_D->m_Mutex = new QMutex();

   //delete m_D->m_FontMetrics;

   delete m_D;

   countMe();
}

GETNAMEMACRO(AWT::OpenGLWidget);

AWT::DrawableWorld::P AWT::OpenGLWidget::getDrawableWorld()
{
   return m_D->m_DrawableWorld;
}

void AWT::OpenGLWidget::setDrawableWorld(AWT::DrawableWorld::P world)
{
   m_D->m_DrawableWorld = world;
   repaint();
}

void AWT::OpenGLWidget::mousePressEvent(QMouseEvent* ev)
{
   if (*m_D->m_MouseBehaviour)
   {
      if (m_D->m_MouseBehaviour->mousePressed(this, ev->x(), ev->y(), ev->button(), D::modifiers(ev)))
         repaint();
   }

   m_D->m_RotationDown[0] = m_D->m_Rotation[0];
   m_D->m_RotationDown[1] = m_D->m_Rotation[1];
   m_D->m_RotationDown[2] = m_D->m_Rotation[2];

   m_D->m_OrbitDown[0] = m_D->m_Orbit[0];
   m_D->m_OrbitDown[1] = m_D->m_Orbit[1];
   m_D->m_OrbitDown[2] = m_D->m_Orbit[2];

   m_D->m_PullBackDown = m_D->m_PullBack;

   m_D->m_MouseXDown  = ev->x();
   m_D->m_MouseYDown  = ev->y();
   m_D->m_MouseState |= ev->button();

   m_D->m_CancelContextMenu = false;

   if (m_D->m_MouseState == 7)
   {
      m_D->m_CancelContextMenu = true;
      resetView();
   }
}

void AWT::OpenGLWidget::mouseReleaseEvent(QMouseEvent* ev)
{
   //m_D->m_MouseState &= !(ev->button());
   if (*m_D->m_MouseBehaviour)
   {
      if (m_D->m_MouseBehaviour->mouseReleased(this, ev->x(), ev->y(), ev->button(), D::modifiers(ev)))
         repaint();
   }

   m_D->m_MouseState &= ~ev->button();
}

void AWT::OpenGLWidget::mouseMoveEvent(QMouseEvent* ev)
{
   bool dorepaint = false;

   m_D->m_CancelContextMenu = true;

   if (*m_D->m_MouseBehaviour)
   {
      dorepaint |= m_D->m_MouseBehaviour->mouseMoved(this, ev->x(), ev->y(), ev->button(), D::modifiers(ev));
   }

   if (m_D->m_CancelContextMenu)
   {
      switch (m_D->m_MouseState)
      {
      case 2:
         m_D->m_PullBack = std::min<double>(m_D->m_PullBackDown + (ev->y() - m_D->m_MouseYDown) / 1000.0, 4);
         break;
      }
   }

   /*
   switch (m_D->m_MouseState)
   {
   case 1:
      m_D->m_Rotation[0] = m_D->m_RotationDown[0] + (ev->y() - m_D->m_MouseYDown);
      m_D->m_Rotation[2] = m_D->m_RotationDown[2] + (ev->x() - m_D->m_MouseXDown);
      
      for (int i = 0; i < 3; ++i)
      {
         while (m_D->m_Rotation[i] < -180)
            m_D->m_Rotation[i] += 360;

         while (m_D->m_Rotation[i] >  180)
            m_D->m_Rotation[i] -= 360;
      }

      dorepaint = true;
      break;
   case 2:
      // Limit it to 1000 away...
      m_D->m_PullBack = std::min<double>(m_D->m_PullBackDown + (ev->y() - m_D->m_MouseYDown) / 1000.0, 4);
      dorepaint = true;
      break;
   case 4:
      {
         // This is the middle button translation
         double matx[16];
         glGetDoublev(GL_MODELVIEW_MATRIX, matx);

         double dx = -(ev->x() - m_D->m_MouseXDown) * pow(10, m_D->m_PullBack) / 1000;
         double dy =  (ev->y() - m_D->m_MouseYDown) * pow(10, m_D->m_PullBack) / 1000;

         m_D->m_Orbit[0] = m_D->m_OrbitDown[0] + matx[ 0]*dx + matx[ 1]*dy;
         m_D->m_Orbit[1] = m_D->m_OrbitDown[1] + matx[ 4]*dx + matx[ 5]*dy;
         m_D->m_Orbit[2] = m_D->m_OrbitDown[2] + matx[ 8]*dx + matx[ 9]*dy;

         dorepaint = true;
         break;
      }
   }
   */

   bool m = m_D->updateMousePosition(ev->x(), height() - ev->y());
   bool e = m_D->updateEyePosition();

   dorepaint |= m || e;

   if (dorepaint)
   {
      repaint();
   }
}

void AWT::OpenGLWidget::enterEvent(QEvent* /*ev*/)
{
   m_D->m_MouseInside = true;

   std::vector<MouseListener::P>::iterator it = m_D->m_MouseListeners.begin();
   std::vector<MouseListener::P>::iterator en = m_D->m_MouseListeners.end();

   for (; it != en; ++it)
      (*it)->mouseEntered();

   updateGL();
}

void AWT::OpenGLWidget::leaveEvent(QEvent* /*ev*/)
{
   m_D->m_MouseInside = false;

   std::vector<MouseListener::P>::iterator it = m_D->m_MouseListeners.begin();
   std::vector<MouseListener::P>::iterator en = m_D->m_MouseListeners.end();

   for (; it != en; ++it)
      (*it)->mouseExited();

   updateGL();
}

void AWT::OpenGLWidget::initializeGL()
{
   QGLWidget::initializeGL();
}

void AWT::OpenGLWidget::resizeGL(int width, int height)
{
   glViewport(0, 0, (GLsizei)width,(GLsizei)height);

   GLdouble aspect = (width + 0.f) / (height + 0.f);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(m_D->m_FovY, aspect, m_D->m_ZNear, m_D->m_ZFar);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glEnable(GL_DEPTH_TEST);

   m_D->m_WindowSize[0] = width;
   m_D->m_WindowSize[1] = height;
}

void AWT::OpenGLWidget::setRotation(const double* v)
{
   m_D->setRotation(v);
   updateGL();
}

void AWT::OpenGLWidget::setOrbitPoint(const double* v)
{
   m_D->setOrbitPoint(v);
   updateGL();
}

void AWT::OpenGLWidget::setVisibleRadius(const double r)
{
   m_D->setVisibleRadius(r);
   updateGL();
}

void AWT::OpenGLWidget::setDistance(const double r)
{
   m_D->setDistance(r);
   updateGL();
}

void AWT::OpenGLWidget::paintGL()
{
   QMutexLocker mutexLocker(m_D->m_Mutex);
   
   if (m_D->m_FirstDraw)
   {
      resetView(false);
      m_D->updateEyePosition();
      m_D->m_FirstDraw = false;
   }

   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);
   
   // This is setting up the camera... therefore, the matrix after this is the
   // camera matrix
   glLoadIdentity();

   if (isViewRelativeLighting())
      m_D->m_DrawableWorld->doLighting();

   // This is the amount to pull the camera back by
   glTranslatef(0, 0, -getDistance());

   // This is the pose of the camera
   glRotated(m_D->m_Rotation[0], 1, 0, 0);
   glRotated(m_D->m_Rotation[2], 0, 1, 0);

   // This is -ve the point we want to orbit
   glTranslated(-m_D->m_Orbit[0], -m_D->m_Orbit[1], -m_D->m_Orbit[2]);

   if (!isViewRelativeLighting())
      m_D->m_DrawableWorld->doLighting();

   // Opaque geometry rendering pass
   m_D->m_DrawableWorld->draw(this, false);

   // Transparent geometry rendering pass
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   m_D->m_DrawableWorld->draw(this, true);

   glDisable(GL_BLEND);

   QStringList msgs;
   msgs.append(QString("Eye: (%1, %2, %3)").arg(m_D->m_Eye[0]).arg(m_D->m_Eye[1]).arg(m_D->m_Eye[2]));
   msgs.append(QString("Centre: (%1, %2, %3)").arg(m_D->m_Orbit[0]).arg(m_D->m_Orbit[1]).arg(m_D->m_Orbit[2]));
   msgs.append(QString("Rotation: (%1, %2, %3)").arg(m_D->m_Rotation[0]).arg(m_D->m_Rotation[1]).arg(m_D->m_Rotation[2]));
   
   int maxWidth = 0;
   for (int i = 0; i < msgs.size(); ++i)
   {
      maxWidth = std::max(maxWidth, m_D->m_FontMetrics->width(msgs[i]));
   }

   const int lineHeight = m_D->m_FontMetrics->ascent() + m_D->m_FontMetrics->descent() + 5;
   const int xbase      = 5;
   const int ybase      = height() - m_D->m_FontMetrics->descent() - 5 - lineHeight*(msgs.count() - 1);

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(0, m_D->m_WindowSize[0], 0, m_D->m_WindowSize[1], -1, 1);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   glDisable(GL_LIGHTING);
   glEnable(GL_BLEND);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
   glBegin(GL_QUADS);
   glVertex2f( 0.f,    0.f);
   glVertex2f( maxWidth+2*xbase,  0.f);
   glVertex2f( maxWidth+2*xbase,  height() - ybase + lineHeight);
   glVertex2f( 0.f,               height() - ybase + lineHeight);
   glEnd();

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();

   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   glDisable(GL_BLEND);
   glColor3f(1.f, 1.f, 1.f);
   for (int i = 0; i < msgs.count(); ++i)
   {
      renderText(xbase, ybase + i*lineHeight, msgs[i], m_D->m_Font);
   }

   if (m_D->m_MouseInside)
   {
      glColor3f(1.f, 1.f, 0.f);
      glLineWidth(10.f);
      glBegin(GL_LINE);
      glVertex3dv(m_D->m_Eye);
      glVertex3dv(m_D->m_Orbit);
      glEnd();
   }
}

void AWT::OpenGLWidget::gluPerspective(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar)   
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan(fovY * AWT::Constants<double>::DegreesToRadians / 2.0);
   ymin = -ymax;
   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

void AWT::OpenGLWidget::resetView(const bool repaintAfter)
{
   double bounds[6];

   m_D->m_DrawableWorld->getBounds(bounds);

   double orbit[3];
   double maxR = 0;

   bool isOK = true;

   for (int i = 0; i < 3 && isOK; ++i)
   {
      if (bounds[2*i+0] != bounds[2*i+0] || bounds[2*i+0] != bounds[2*i+0])
         isOK = false;

      orbit[i] = (bounds[2*i+0] + bounds[2*i+1]) / 2;
      maxR += (bounds[2*i+0] - orbit[i])*(bounds[2*i+0] - orbit[i]);
   }

   if (isOK)
   {
      m_D->setOrbitPoint(orbit);

      if (maxR != 0)
         m_D->setVisibleRadius(sqrt(maxR));

      if (repaintAfter)
         updateGL();
   }

}

bool AWT::OpenGLWidget::isViewRelativeLighting() const
{
   return m_D->m_ViewRelativeLighting;
}

void AWT::OpenGLWidget::setViewRelativeLighting(const bool v)
{
   if (m_D->m_ViewRelativeLighting != v)
   {
      m_D->m_ViewRelativeLighting = v;
      repaint();
   }
}

void AWT::OpenGLWidget::getViewPosition(double* pos)
{
   for (int i = 0; i < 3; ++i)
      pos[i] = m_D->m_Eye[i];
}

void AWT::OpenGLWidget::getMouseDirection(double* norm)
{
   for (int i = 0; i < 3; ++i)
      norm[i] = m_D->m_WorldCoords[i] - m_D->m_Eye[i];

   normalize(norm, 3);
}

bool AWT::OpenGLWidget::isMouseOver() const
{
   return m_D->m_MouseInside;
}

void AWT::OpenGLWidget::drawText(int x, int y, const std::string& text)
{
   renderText(x, y, QObject::tr(text.c_str()));
}

void AWT::OpenGLWidget::drawText(double x, double y, double z, const std::string& text)
{
   renderText(x, y, z, QObject::tr(text.c_str()));
}

void AWT::OpenGLWidget::addMouseListener(AWT::MouseListener::P list)
{
   m_D->m_MouseListeners.push_back(list);
}

void AWT::OpenGLWidget::removeMouseListener(AWT::MouseListener::P list)
{
   std::vector<MouseListener::P>::iterator it = m_D->m_MouseListeners.begin();
   std::vector<MouseListener::P>::iterator en = m_D->m_MouseListeners.end();

   for (; it != en; ++it)
   {
      if (*it == list)
      {
         m_D->m_MouseListeners.erase(it, it+1);
         break;
      }
   }
}

void AWT::OpenGLWidget::contextMenuEvent(QContextMenuEvent* event)
{
   if (m_D->m_CancelContextMenu)
      return;

   QMenu menu(this);

   QMenu* backgroundSubMenu = menu.addMenu("Background");
   backgroundSubMenu->addAction(m_D->m_ActionBackground[0]);
   backgroundSubMenu->addAction(m_D->m_ActionBackground[1]);
   backgroundSubMenu->addAction(m_D->m_ActionBackground[2]);

   DrawableWorld::BackgroundMode bgMode = getDrawableWorld()->getBackgroundMode();

   m_D->m_ActionBackground[0]->setChecked(bgMode == AWT::DrawableWorld::BACKGROUND_USER );
   m_D->m_ActionBackground[1]->setChecked(bgMode == AWT::DrawableWorld::BACKGROUND_WHITE);
   m_D->m_ActionBackground[2]->setChecked(bgMode == AWT::DrawableWorld::BACKGROUND_BLACK);

   menu.addSeparator();

   menu.addAction(m_D->m_ActionCentreView);
   menu.addAction(m_D->m_ActionScreenshot);

   menu.exec(event->globalPos());
}

void AWT::OpenGLWidget::changeToUserBackground()
{
   getDrawableWorld()->setBackgroundMode(DrawableWorld::BACKGROUND_USER);
   repaint();
}

void AWT::OpenGLWidget::changeToWhiteBackground()
{
   getDrawableWorld()->setBackgroundMode(DrawableWorld::BACKGROUND_WHITE);
   repaint();
}

void AWT::OpenGLWidget::changeToBlackBackground()
{
   getDrawableWorld()->setBackgroundMode(DrawableWorld::BACKGROUND_BLACK);
   repaint();
}

void AWT::OpenGLWidget::takeScreenshot()
{
   QImage img = grabFrameBuffer();

   QString filename = QFileDialog::getSaveFileName(this, tr("Save Location"), QString(), tr("PNG Images (*.png)"));

   if (filename == tr(""))
      return;

   if (!filename.toLower().endsWith(tr(".png")))
      filename.append(tr(".png"));

   img.save(filename);
}

void AWT::OpenGLWidget::getOrbitPoint(double *v) const
{
   for (unsigned int i = 0; i < 3; ++i)
      v[i] = m_D->m_Orbit[i];
}

void AWT::OpenGLWidget::getEyePosition(double *v) const
{
   for (unsigned int i = 0; i < 3; ++i)
      v[i] = m_D->m_Eye[i];
}

void AWT::OpenGLWidget::getRotation(double *v) const
{
   for (unsigned int i = 0; i < 3; ++i)
      v[i] = m_D->m_Rotation[i];
}

double AWT::OpenGLWidget::getDistance() const
{
   return pow(10, m_D->m_PullBack);
}

void AWT::OpenGLWidget::refresh()
{
   updateGL();
}

AWT::OpenGLWidgetMouseBehaviour::P AWT::OpenGLWidget::getMouseBehaviour()
{
   return m_D->m_MouseBehaviour;
}

void AWT::OpenGLWidget::setMouseBehaviour(AWT::OpenGLWidgetMouseBehaviour::P arg)
{
   m_D->m_MouseBehaviour = arg;
}

void AWT::OpenGLWidget::makeCurrent()
{
   QGLWidget::makeCurrent();
}