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

#include <QApplication>

#include "Cates/ParticleSystem.h"
#include "Cates/ParticlesAndMesh.h"
#include "DrawableFramework/DrawableFactory.h"
#include "Mesh/AreaAveragedNormalCalculator.h"
#include "Mesh/MeshFunctions.h"
#include "Mesh/MeshGenerator.h"
#include "Mesh/MeshImpl.h"
#include "Mesh/TuplesFunctions.h"
#include "Mesh/TuplesImpl.h"
#include "Mesh/VTKMeshLoader.h"
#include "MeshSearching/MeshApproxGeodesic.h"
#include "OpenGLDrawables/OpenGLColourMapper.h"
#include "StateMachine/SaveableStateMachine.h"
#include "StateMachine/StateMachineStack.h"
#include "Useful/ArrayVectorFunctions.h"
#include "Useful/Exception.h"
#include "Useful/Noise.h"
#include "Useful/PrintMacros.h"
#include "Useful/ProfTimer.h"

using namespace AWT;

typedef double T;

namespace AWT
{
   template <class T>
   class CatesStateMachine : public SaveableStateMachine
   {
   public:
      typedef ManagedAutoPointer<CatesStateMachine<T> > P;

   protected:
      CatesStateMachine(StateMachineStack::P stack)
      {
         sms = stack;
      }

      virtual ~CatesStateMachine()
      {
      }

   public:
      static P getInstance(StateMachineStack::P stack)
      {
         AUTOGETINSTANCE(AWT::CatesStateMachine<T>, (stack));
      }

      virtual std::string getClassName() const { return "CatesStateMachine<T>"; }

      virtual StateMachineStack* getStack() { return *this->sms; }

      virtual bool step()
      {
         static MeshIndex iters = 0;

         const MeshIndex N = pam->getParticles()->getNumberOfPoints();
         Tuples<T>::P delta = TuplesImpl<T>::getInstance(3, N);

         pam->calculateInternalForceUpdate(delta);

         T vtx[3];
         T rms = 0;
         for (MeshIndex i = 0; i < N; ++i)
         {
            delta->getPoint(i, vtx);
            rms += dot(vtx, vtx, 3);
         }

         rms = sqrt(rms / N);
         const T dt = 1 / rms; //*pam->getMinSigma()*pam->getMinSigma();
         PRINTVBL(rms);
         PRINTVBL(dt);
         PRINTVBL(rms * dt);

         pam->updateParticles(delta, dt);

         PRINTVBL(N);

         if ((++iters % 50) == 0 && N < 4096)
         {
            DEBUGMACRO("Splitting!");
            pam->splitParticles();
         }

         return true;
      }

      virtual bool isRunning() const
      {
         return true;
      }

      virtual bool isError() const
      {
         return false;
      }

      virtual std::string getLastError() const
      {
         return "What's an error?";
      }

      virtual std::string getStateName() const
      {
         return "Running";
      }

      virtual void setParticlesAndMesh(typename ParticlesAndMesh<T>::P pam)
      {
         this->pam = pam;
      }

      virtual std::string getMagic() const { return ""; }

      virtual bool load(const std::string& /*filename*/) { return false; }

      virtual void save(const std::string& /*filename*/) { }
   protected:
      struct D;
      D* m_D;

      typename ParticlesAndMesh<T>::P pam;
      StateMachineStack::P sms;
   };

   template <class T>
   class CatesStateMachine2 : public SaveableStateMachine
   {
   public:
      typedef ManagedAutoPointer<CatesStateMachine2<T> > P;

   protected:
      CatesStateMachine2(StateMachineStack::P stack)
      {
         sms = stack;
      }

      virtual ~CatesStateMachine2()
      {
      }

   public:
      static P getInstance(StateMachineStack::P stack)
      {
         AUTOGETINSTANCE(AWT::CatesStateMachine2<T>, (stack));
      }

      virtual std::string getClassName() const { return "CatesStateMachine2<T>"; }

      virtual StateMachineStack* getStack() { return *this->sms; }

      virtual bool step()
      {
         static MeshIndex iters = 0;

         BLANKLINE;
         BLANKLINE;
         std::cerr << "========================================" << std::endl;

         PRINTVBL(iters);
         ps->update(100);

         BLANKLINE;
         std::cerr << "========================================" << std::endl;
         BLANKLINE;

         ++iters;
         return true;
      }

      virtual bool isRunning() const
      {
         return true;
      }

      virtual bool isError() const
      {
         return false;
      }

      virtual std::string getLastError() const
      {
         return "What's an error?";
      }

      virtual std::string getStateName() const
      {
         return "Running";
      }

      virtual void setParticleSystem(typename ParticleSystem<T>::P ps)
      {
         this->ps = ps;
      }

      virtual std::string getMagic() const { return ""; }

      virtual bool load(const std::string& /*filename*/) { return false; }

      virtual void save(const std::string& /*filename*/) { }
   protected:
      typename ParticleSystem<T>::P ps;
      StateMachineStack::P sms;
   };
}

void init(QtForm::P mainForm)
{
   DrawableWorld::P dworld = mainForm->getViewer(0)->getDrawableWorld();
   DrawableFactory<T>::P fact = dworld->getFactory();

   ParticleSystem<T>::P ps = ParticleSystem<T>::getInstance();
   ps->setSigmaBounds(1, 100);

   const char* filenames[] = {
      "D:/PhdData/SmoothedBones/Part-1-1.vtk.5000.ply",
      "D:/PhdData/01-InnominateBones/Part-1-3.vtk"
   };
   const MeshIndex M = 1;
   const MeshIndex N = 10;

   Mesh<T>::P mesh[M];
   for (MeshIndex s = 0; s < M; ++s)
   {
      mesh[s] = VTKMeshLoader<T>::load(filenames[s], false);
      //mesh[s] = MeshImpl<T>::getInstance(0, 0);
      //MeshGenerator<T>::generateTorus(mesh[s], 25, 10, 32, 32);

      AreaAveragedNormalCalculator<T>::getInstance()->calculateNormalsAndSet(mesh[s]);
   }

   Tuples<T>::P particlesCommon = TuplesImpl<T>::getInstance(3, N);

   {
      T groupbounds[] = { 
         std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(),
         std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(),
         std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(),
      };

      for (MeshIndex s = 0; s < M; ++s)
      {
         T bounds[6];
         TuplesFunctions<T>::calculateBounds(mesh[s]->getVertices(), bounds);

         FOREACHAXIS(ax)
         {
            groupbounds[2*ax+0] = std::min(groupbounds[2*ax+0], bounds[2*ax+0]);
            groupbounds[2*ax+1] = std::max(groupbounds[2*ax+1], bounds[2*ax+1]);
         }
      }

      T vtx[3];
      for (MeshIndex i = 0; i < N; ++i)
      {
         for (int ax = 0; ax < 3; ++ax)
            vtx[ax] = Noise<T>::randu(groupbounds[2*ax], groupbounds[2*ax+1]);

         particlesCommon->addPoint(vtx);
      }
   }
   
   for (MeshIndex s = 0; s < M; ++s)
   {
      Tuples<T>::P particles = TuplesImpl<T>::getInstance(3, N);
      T vtx[3];
      for (MeshIndex i = 0; i < N; ++i)
      {
         particlesCommon->getPoint(i, vtx);
         particles->addPoint(vtx);
      }

      ps->addMesh(mesh[s], particles);
      
      DrawableMesh<T>::P dmesh = fact->createMesh(ps->getMesh(s));
      DrawablePoints<T>::P dpoints = fact->createPoints(ps->getParticles(s));
      
      dmesh->setMaterial(fact->createColour(1.f, 0.5f, 0.5f, .5f, true));
      //dmesh->setMaterial(colmap);
      dmesh->setDrawAs(AWT::DRAWAS_SOLID);

      dpoints->setMaterial(fact->createColour(0.f, (s==0)?0.f:1.f, (s==0)?1.f:0.f, 1.f, false));
      dpoints->setPointSize(2.f);

      dworld->add("Mesh", dmesh);
      dworld->add("Particles", dpoints);
   }

   StateMachineStack::P sms = StateMachineStack::getInstance();
   CatesStateMachine2<T>::P csm = CatesStateMachine2<T>::getInstance(sms);

   csm->setParticleSystem(ps);

   mainForm->setStateMachine(csm);
}

int main(int argc, char** argv)
{
   ReferenceCountedObject::debug = false;

   QApplication app(argc, argv);

   int ret = 999;

   try
   {
      AWT::QtForm::P mainForm = AWT::QtForm::getInstance(1, 1);

      init(mainForm);

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