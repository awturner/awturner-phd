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
#include "QtForm/include/QtForm.h"

#include "Useful/PrintMacros.h"

#include <QApplication.h>

#include "Useful/Exception.h"
#include "OpenGLDrawables/OpenGLHeaders.h"
#include <limits>
#include <set>

#include "Mesh/Pose.h"
#include "QtForm/include/StateMachineThread.h"
#include "QtForm/include/OpenGLWidget.h"

#include "Mesh/AreaAveragedNormalCalculator.h"
#include "DrawableFramework/DrawableFactory.h"
#include "OpenGLDrawables/OpenGLColourMapper.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/TuplesFunctions.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/MeshGenerator.h"
#include "MeshBreaker/FindGenerators.h"
#include "MeshBreaker/FlattenMesh.h"
#include "MeshBreaker/ShapeImage.h"
#include "MeshBreaker/DrawFindGenerators.h"
#include "MeshBreaker/PelvicGeneratorDirection.h"

#include "MeshSearching/FacesRayIntersectionSearch.h"

#include "QtForm/include/MouseListener.h"

typedef double T;
using namespace AWT;

#include "LuaPlus.h"

using namespace LuaPlus;

namespace AWT
{
class PointClickLocator : public MouseListener
   {
      typedef double T;
   public:
      typedef ManagedAutoPointer<PointClickLocator> P;

      static P getInstance(Mesh<T>::P mesh, Tuples<T>::P pt)
      {
         AUTOGETINSTANCE(AWT::PointClickLocator, (mesh, pt));
      }

   protected:
      PointClickLocator(Mesh<T>::P mesh, Tuples<T>::P pt)
         : m_Mesh(mesh), m_Pt(pt)
      {
         m_RaySearch = FacesRayIntersectionSearch<T>::getInstance();
      }

   public:
      std::string getClassName() const { return "AWT::PointClickLocator"; }
      void mouseEntered() {}
      void mouseExited()  {}

      void mouseChanged(const double* eye, const double* normal, const int /*button*/)
      {
         //if (button == 0)
         //   return;

         double nml[] = { normal[0]-eye[0], normal[1]-eye[1], normal[2]-eye[2] };

         m_RaySearch->reset();
         m_RaySearch->setTestPoint(eye);
         m_RaySearch->setTestNormal(nml);

         m_Mesh->search(m_RaySearch);

         T np[3];
         if (m_RaySearch->getNearestPoint(np) != -1)
         {
            m_Pt->setPoint(0, np);
            PRINTVEC(np, 3);
         }
      }

   protected:
      Mesh<T>::P m_Mesh;
      FacesRayIntersectionSearch<T>::P m_RaySearch;
      Tuples<T>::P m_Pt;
   };

   struct WorldMouseState
   {
      WorldMouseState()
      {
         eye[0] = eye[1] = eye[2] = 0;
         normal[0] = normal[1] = normal[2] = 0;
         button = 0;
         over = false;
      }

      double eye[3];
      double normal[3];
      int button;
      bool over;
   };

   class SimpleMouseListener : public MouseListener
   {
   public:
      typedef ManagedAutoPointer<SimpleMouseListener> P;

   protected:
      SimpleMouseListener(WorldMouseState* state)
      {
         this->m_State = state;
      }

      virtual ~SimpleMouseListener()
      {
      }

   public:
      static P getInstance(WorldMouseState* state)
      {
         AUTOGETINSTANCE(SimpleMouseListener, (state));
      }

      virtual void mouseEntered()
      {
         m_State->over = true;
      }

      virtual void mouseExited()
      {
         m_State->over = false;
      }

      virtual std::string getClassName() const
      {
         return "AWT::SimpleMouseListener";
      }

      virtual void mouseChanged(const double* eye, const double* normal, const int button)
      {
         for (int i = 0; i < 3; ++i)
         {
            m_State->eye[i]    = eye[i];
            m_State->normal[i] = normal[i] - eye[i];
         }
         normalize(m_State->normal, 3);
         m_State->button = button;
      }

   protected:
      WorldMouseState* m_State;
   };
}

#include "Useful/Noise.h"

template <class T>
void randomPointOnSurface(typename Mesh<T>::P mesh, T* vtx)
{
   // Pick a random face
   MeshIndex f = (rand() << 15 | rand()) % mesh->getNumberOfFaces();

   // Pick a pair of weights which sum to no more than 1
   T a = Noise<T>::randu(0, 1);
   T b = Noise<T>::randu(0, 1-a);

   // Now get the point which this corresponds to
   T vtxA[3], vtxB[3], vtxC[3];
   mesh->getFace(f, vtxA, vtxB, vtxC);

   FOREACHAXIS(ax)
      vtx[ax] = vtxA[ax] + a*(vtxB[ax] - vtxA[ax]) + b*(vtxC[ax] - vtxA[ax]);
}

void loadLuaFile(LuaStateOwner& luaState, const char* filename)
{
   int iret = luaState->DoFile(filename);
   if (iret != 0)
   {
      DEBUGMACRO("Could not parse file " << filename << " (Lua returned error code " << iret << ")");
      DEBUGLINEANDEXIT(1);
   }
}

unsigned char* generateCheckerboard(unsigned int w, unsigned int h, unsigned int checksize)
{
   unsigned char* data = new unsigned char[ 3*w*h ];

   unsigned int offset = 0;
   for (unsigned int y = 0; y < h; ++y)
   {
      for (unsigned int x = 0; x < w; ++x, offset += 3)
      {
         const bool blackorwhite = ((x / checksize) % 2 == 1) ^ ((y / checksize) % 2 == 1);
         
         data[offset + 0] = (y % checksize) < 2 ? 0 : 255;
         data[offset + 1] = (x % checksize) < 2 ? 0 : 255;
         data[offset + 2] = ((y % checksize) < 2 || (x % checksize) < 2) ? 0 : 255;

         //DEBUGMACRO(x << "," << y << " -> " << (blackorwhite?"black":"white"));
      }
      //PAUSE;
   }

   return data;
}

void init(QtForm::P form)
{
   Noise<T>::timeSeed();

   LuaStateOwner luaState;

   loadLuaFile(luaState, "materials.lua");
   loadLuaFile(luaState, "QtSandbox.config.lua");

   typedef double T;

   NormalCalculator<T>::P nc = AreaAveragedNormalCalculator<T>::getInstance();
   AWT::DrawableFactory<T>::P fact(form->getViewer(1)->getDrawableWorld()->getFactory());

   for (int i = 0; i < 2; ++i)
   {
      Drawable::P dax = fact->createAxis(100);
      form->getViewer(i)->getDrawableWorld()->add("World Axes", dax);
      dax->setVisible(false);
   }

   AWT::Mesh<T>::P mesh;

   LuaObject objMesh = luaState->GetGlobal("mesh");
   if (objMesh.IsNil())
      DEBUGLINEANDEXIT(1);

   LuaObject objFilename = objMesh.GetByName("filename");

   if (objFilename.IsString())
   {
      const char* filename = objFilename.GetString();

      PRINTVBL(filename);
      form->setWindowTitle(QObject::tr(filename));
      mesh = VTKMeshLoader<T>::load(filename, false);
      DEBUGLINE;
   }
   else
   {
      DEBUGLINE;
      LuaObject objDefaultTorus = luaState->GetGlobal("defaultTorus");
      if (objDefaultTorus.IsNil())
         DEBUGLINEANDEXIT(1);

      LuaObject objRadius = objDefaultTorus.GetByName("radius");
      if (objRadius.IsNil())
         DEBUGLINEANDEXIT(1);

      LuaObject objResolution = objDefaultTorus.GetByName("resolution");
      if (objResolution.IsNil())
         DEBUGLINEANDEXIT(1);

      LuaObject objOuterRadius     = objRadius.GetByIndex(1);
      LuaObject objInnerRadius     = objRadius.GetByIndex(2);
      LuaObject objThetaResolution = objResolution.GetByIndex(1);
      LuaObject objPhiResolution   = objResolution.GetByIndex(2);

      const T outerRadius         = (objOuterRadius.IsNumber()) ? objOuterRadius.ToNumber() : 2;
      const T innerRadius         = (objInnerRadius.IsNumber()) ? objInnerRadius.ToNumber() : (outerRadius/2);
      const unsigned int thetaRes = (objThetaResolution.IsInteger()) ? objThetaResolution.ToInteger() : 16;
      const unsigned int phiRes   = (objPhiResolution.IsInteger()) ? objPhiResolution.ToInteger() : 16;

      mesh = AWT::MeshImpl<T>::getInstance();
      MeshGenerator<T>::generateTorus(mesh, outerRadius, innerRadius, thetaRes, phiRes);
   }

   DEBUGLINE;
   /*
   {
      Tuples<T>::P pt = TuplesImpl<T>::getInstance(3, 1);
      DrawablePoints<T>::P dpt = fact->createPoints(pt);
      dpt->setPointSize(5);
      dpt->setMaterial(fact->createColour(1.f, 0.f, 0.f, 1.f, false));

      form->getViewer(1)->getDrawableWorld()->add("The point", dpt);

      PointClickLocator::P pcl = PointClickLocator::getInstance(mesh, pt);
      form->getViewer(1)->addMouseListener(pcl);
   }
   */

   DEBUGLINE;
   // Calculate the Euler characteristic of the mesh - should be zero for a torus!
   {      
      const int euler = MeshFunctions<T>::getEulerCharacteristic(mesh);

      if (euler != 0)
         DEBUGMACRO("This mesh is not a closed 1-torus!  (Euler characteristic = " << euler << ")");
   }

   DEBUGLINE;
   LuaObject objDoGenerators = luaState->GetGlobal("calcgenerators");
   if (!objDoGenerators.IsBoolean())
      DEBUGLINEANDEXIT(1);

   Tuples<T>::P tcoords;
   FlattenMeshPair<T>::P flattened;
        
   DEBUGLINE;
   // UNCOMMENT to display generators
   if (objDoGenerators.GetBoolean())
   {
      DEBUGLINE; 
      FindGenerators<T>::P findGenerators = FindGenerators<T>::getInstance(mesh, FindGenerators<T>::TRAVERSE_DEPTH);
      DEBUGLINE; 

      PelvicGeneratorDirection<T>::P pgd = PelvicGeneratorDirection<T>::getInstance(findGenerators, true);
      mesh = pgd->getMesh();

      // Now, I want to correct the generators for the fact that it's a hemipelvis
      {
         T basis[3][3], origin[3];
         pgd->getBasis(origin, basis[0], basis[1], basis[2]);

         Tuples<T>::P basisPoints = TuplesImpl<T>::getInstance(3, 6);
         for (int i = 0; i < 3; ++i)
         {
            for (int j = 0; j < 3; ++j)
               basis[i][j] = origin[j] + 10*basis[i][j];

            basisPoints->addPoint(origin);
            basisPoints->addPoint(basis[i]);
         }

         DrawablePoints<T>::P dbasis = fact->createPoints(basisPoints);
         dbasis->setPointSize(0.f);
         dbasis->setLineWidth(3.f);
         dbasis->setMaterial(fact->createColour(1.f, 1.f, 0.f, 1.f, false));
         dbasis->setClosed(true);

         form->getViewer(1)->getDrawableWorld()->add("Pelvic Basis", dbasis);
         dbasis->setVisible(false);
      }

      {
         DrawFindGenerators<T>::P dfg = DrawFindGenerators<T>::getInstance(findGenerators);
         form->getViewer(1)->getDrawableWorld()->add("Found Generators", dfg);
         dfg->setVisible(false);
      }

      // UNCOMMENT to do the mesh flattening
      LuaObject objDoGenerators = luaState->GetGlobal("flattenmesh");
      if (!objDoGenerators.IsBoolean())
         DEBUGLINEANDEXIT(1);

      
      if (objDoGenerators.GetBoolean())
      {
         LuaObject objWeightType = luaState->GetGlobal("weightType");
         FlattenMesh<T>::WeightType weightType = FlattenMesh<T>::getWeightType(objWeightType.IsNil() ? "" : objWeightType.GetString());

         LuaObject objSpreadType = luaState->GetGlobal("spreadType");
         FlattenMesh<T>::SpreadType spreadType = FlattenMesh<T>::getSpreadType(objSpreadType.IsNil() ? "" : objSpreadType.GetString());
         
         flattened = FlattenMesh<T>::flattenToPair(pgd, weightType, spreadType);

         vnl_matrix<T> R(3, 3);
         R.fill(0);
         R(0,0) = 1;
         R(1,2) = 1;
         R(2,1) = 1;
         tcoords = TuplesFunctions<T>::transform(flattened->getFlattenedMesh()->getVertices(), R);

         {
            LuaObject objDefaultTorus = luaState->GetGlobal("defaultTorus");
            LuaObject objResolution   = objDefaultTorus.GetByName("resolution");
            if (objResolution.IsNil())
               DEBUGLINEANDEXIT(1);

            LuaObject objUResolution = objResolution.GetByIndex(1);
            LuaObject objVResolution = objResolution.GetByIndex(2);

            DrawableMesh<T>::P dsamp = fact->createMesh(flattened->toShapeImage(
               objUResolution.IsInteger() ? objUResolution.ToInteger() : 16, 
               objVResolution.IsInteger() ? objVResolution.ToInteger() : 16
           ));
            nc->calculateNormalsAndSet(dsamp->getMesh());

            dsamp->setMaterial(fact->createColour(1.f, 0.f, 1.f, 1.f, true));
            dsamp->setDrawAs(AWT::DRAWAS_LINES);
            form->getViewer(1)->getDrawableWorld()->add("Shape Image", dsamp);
         }

         Mesh<T>::P flatMesh    = flattened->getFlattenedMesh();
         Mesh<T>::P nonflatMesh = flattened->getMesh();

         Tuples<T>::P corrPoints[2];

         nc->calculateNormalsAndSet(flattened->getFlattenedMesh());

         {
            Mesh<T>::P flatMesh = flattened->getFlattenedMesh();
            DrawableMesh<T>::P drawFlatten = fact->createMesh(flattened->getFlattenedMesh());
            PRINTVBL(flatMesh->getNumberOfVertices());
            PRINTVBL(flatMesh->getNumberOfFaces());

            drawFlatten->setDrawAs(AWT::DRAWAS_POINTS);
            drawFlatten->setPointSize(4.f);
            drawFlatten->setMaterial(fact->createColour(0.f, 0.f, 1.f, 1.0f, false));
            form->getViewer(0)->getDrawableWorld()->add("Flattened Mesh Vertices", drawFlatten);
            drawFlatten->setVisible(true);
         }

         DrawableMesh<T>::P drawFlatten = fact->createMesh(flattened->getFlattenedMesh());
         drawFlatten->setDrawAs(AWT::DRAWAS_LINES);
         drawFlatten->setPointSize(4.f);
         drawFlatten->setMaterial(fact->createColour(0.f, 0.f, 0.f, 1.0f, false));

         Drawable::P dflat = drawFlatten;
         form->getViewer(0)->getDrawableWorld()->add("Flattened Mesh", dflat);
         dflat->setVisible(true);

         {
            Tuples<T>::P zeroo = TuplesImpl<T>::getInstance(3, 1);
            T zz[] = { 0, 0, 0 };
            zeroo->addPoint(zz);

            DrawablePoints<T>::P dzero = fact->createPoints(zeroo);
            dzero->setPointSize(10.f);
            dzero->setMaterial(fact->createColour(1.f, 0.f, 0.f, 1.f, false));

            form->getViewer(0)->getDrawableWorld()->add("Fixed Point", dzero);
            dzero->setVisible(false);
         }
      }
      DEBUGLINE;
   }

   // UNCOMMENT to display mesh
   {
      DEBUGLINE;
      mesh = flattened->getMesh();

      nc->calculateNormalsAndSet(mesh);

      DrawableMesh<T>::P dmesh = fact->createMesh(mesh);
      dmesh->setDrawAs(AWT::DRAWAS_SOLID);

      LuaObject objMeshDisplayName = objMesh.GetByName("displayName");
      const char* meshDisplayName = objMeshDisplayName.IsNil() ? "The Mesh!" : objMeshDisplayName.GetString();

      form->getViewer(1)->getDrawableWorld()->add(meshDisplayName, dmesh);
      dmesh->setVisible(false);

      DEBUGLINE;
      LuaObject objMeshMaterial = objMesh.GetByName("material");
      if (*tcoords != 0)
      {
         DEBUGLINE;
         Tuples<T>::P ntcoords = TuplesImpl<T>::getInstance(2, tcoords->getNumberOfPoints());
         T vtx3[3];
         for (MeshIndex i = 0, imax = tcoords->getNumberOfPoints(); i < imax; ++i)
         {
            tcoords->getPoint(i, vtx3);
            vtx3[1] = vtx3[2];
            ntcoords->setPoint(i, vtx3);
         }
         
         QGLWidget* widget = dynamic_cast<QGLWidget*>(form->getViewer(1).getData());
         widget->makeCurrent();

         unsigned char* checker = generateCheckerboard(256, 256, 16);
         dmesh->getMesh()->setTextureCoords(ntcoords);
         dmesh->setMaterial(fact->createTexture(checker, 256, 256, true));
         delete [] checker;

         /*
         OpenGLColourMapper<T>::P mapper = OpenGLColourMapper<T>::getInstance();
         mapper->setData(tcoords);
         dmesh->setMaterial(mapper);
         */
         DEBUGLINE;
      }
      else if (!objMeshMaterial.IsNil())
      {
         DEBUGLINE;
         float ambient[3];
         float diffuse[3];
         float specular[3];
         float shininess;

         LuaObject objAmbient = objMeshMaterial.GetByName("ambient");
         for (unsigned int i = 0; i < 3; ++i)
            ambient[i] = static_cast<float>(objAmbient.GetByIndex(i+1).GetNumber());

         LuaObject objDiffuse = objMeshMaterial.GetByName("diffuse");
         for (unsigned int i = 0; i < 3; ++i)
            diffuse[i] = static_cast<float>(objDiffuse.GetByIndex(i+1).GetNumber());

         LuaObject objSpecular = objMeshMaterial.GetByName("specular");
         for (unsigned int i = 0; i < 3; ++i)
            specular[i] = static_cast<float>(objSpecular.GetByIndex(i+1).GetNumber());

         shininess = static_cast<float>(objMeshMaterial.GetByName("shininess").GetNumber());

         dmesh->setMaterial(fact->createMaterial(ambient, diffuse, specular, shininess));
         DEBUGLINE;
      }
      else
      {
         Tuples<T>::P curv = MeshFunctions<T>::calculatePrincipalCurvature(mesh);

         OpenGLColourMapper<T>::P mapper = OpenGLColourMapper<T>::getInstance();
         mapper->setData(curv);
         dmesh->setMaterial(mapper);
         
         // Let's calculate the 5% and 95% values
         T* vs = new T[ curv->getNumberOfPoints() ];
         for (MeshIndex v = 0, vmax = curv->getNumberOfPoints(); v < vmax; ++v)
            vs[v] = curv->getPointElement(v, 0);

         DEBUGLINE;
         heapSort(vs, curv->getNumberOfPoints());
         DEBUGLINE;

         PRINTVBL(vs[0]);
         PRINTVBL(vs[curv->getNumberOfPoints()-1]);

         PRINTVBL(mapper->getMin());
         PRINTVBL(mapper->getMax());

         MeshIndex first = 0, last = curv->getNumberOfPoints()-1;
         
         while (first < last && (vs[first] != vs[first] || abs(vs[first]) == std::numeric_limits<T>::infinity()))
            ++first;
         
         while (last > first && (vs[last] != vs[last] || abs(vs[last]) == std::numeric_limits<T>::infinity()))
            --last;

         MeshIndex lowerQuart = first + static_cast<MeshIndex>(floor(0.05*(last-first)));
         MeshIndex upperQuart = first + static_cast<MeshIndex>(ceil(0.95*(last-first)));

         mapper->setMax(vs[upperQuart]);
         mapper->setMin(vs[lowerQuart]);

         PRINTVBL(upperQuart);
         PRINTVBL(lowerQuart);

         PRINTVBL(vs[upperQuart]);
         PRINTVBL(vs[lowerQuart]);
         delete [] vs;
      }
      
   }

   DEBUGLINE;
   return;

   OpenGLWidget* widget;
   widget = dynamic_cast<OpenGLWidget*>(form->getViewer(0).getData());
   widget->changeToWhiteBackground();

   widget = dynamic_cast<OpenGLWidget*>(form->getViewer(1).getData());
   widget->changeToWhiteBackground();

   LuaObject camPos = luaState->GetGlobal("cameraPos");
   if (!camPos.IsNil())
   {
      T centre[3];
      T rot[3];

      for (int i = 0; i < 2; ++i)
      {
         LuaObject centreObj = camPos.GetByIndex(i+1).GetByName("orbit");
         if (!centreObj.IsNil())
         {
            for (int ax = 0; ax < 3; ++ax)
               centre[ax] = centreObj.GetByIndex(ax+1).GetNumber();
            form->getViewer(i)->setOrbitPoint(centre);

            PRINTVEC(centre, 3);
         }

         LuaObject rotateObj = camPos.GetByIndex(i+1).GetByName("rotation");
         if (!rotateObj.IsNil())
         {
            for (int ax = 0; ax < 3; ++ax)
               rot[ax] = rotateObj.GetByIndex(ax+1).GetNumber();
            form->getViewer(i)->setRotation(rot);

            PRINTVEC(rot, 3);
         }
      }
   }
   DEBUGLINE;
}

int main(int argc, char** argv)
{
   ReferenceCountedObject::debug = false;
   glutInit(&argc, argv);

   QApplication app(argc, argv);

   int ret = 999;

   try
   {
      AWT::QtForm::P mainForm = AWT::QtForm::getInstance(1, 2);

      /*
      if (argc == 2)
         mainForm->load(QObject::tr(argv[1]));
      */

      DEBUGLINE;
      init(mainForm);
      DEBUGLINE;

      mainForm->showMaximized();

      ret = app.exec();

      DEBUGLINE;
   }
   catch (AWT::Exception& ex)
   {
      std::cerr << ex << std::endl;
      throw ex;
   }

   AWT::ManagedObject::report(true);
   DEBUGLINE;

   return ret;
}