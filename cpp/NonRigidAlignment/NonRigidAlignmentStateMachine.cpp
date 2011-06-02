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
#include "NonRigidAlignmentStateMachine.h"

#include "ControlPoint.h"
#include "ControlPointSet.h"

#include "Mesh/AreaAveragedNormalCalculator.h"
#include "Mesh/MeshSampler.h"
#include "Mesh/RandomMeshSampler.h"
#include "Mesh/SamplePoints.h"
#include "Mesh/SampledMesh.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/VTKMeshWriter.h"

#include "MeshSmoothing/MeshSmoothing.h"

#include "NonRigidAlignment.h"

#include "Useful/PrintMacros.h"
#include "Useful/Exception.h"

#include <fstream>
#include <sstream>
#include <limits>
#include <string>
#include <vector>

template <class T>
struct AWT::NonRigidAlignmentStateMachine<T>::D
{
   enum State
   {
      STATE_SETUP                     = 1,
      STATE_INITIALIZE                = 2,
      STATE_BEFORE_RIGID_ALIGNMENT    = 3,
      STATE_RIGID_ALIGNMENT           = 4,
      STATE_MESH_REFINEMENT           = 5,
      STATE_BEFORE_NRIGID_ALIGNMENT   = 6,
      STATE_NRIGID_ALIGNMENT          = 7,
      STATE_DECIDE_TO_TERMINATE       = 8,
      STATE_STOP                      = 9,
      STATE_ERROR                     = 10,
      STATE_UNDEFINED                 = 11
   };

   std::string getStateName(State state)
   {
      switch (state)
      {
      case STATE_SETUP:
         return "Setup";
      case STATE_INITIALIZE:
         return "Initializing";
      case STATE_BEFORE_RIGID_ALIGNMENT:
         return "Before Rigid Alignment";
      case STATE_RIGID_ALIGNMENT:
         return "Rigid Alignment";
      case STATE_MESH_REFINEMENT:
         return "Mesh Refinement";
      case STATE_BEFORE_NRIGID_ALIGNMENT:
         return "Before Non-Rigid Alignment";
      case STATE_NRIGID_ALIGNMENT:
         return "Non-Rigid Alignment";
      case STATE_DECIDE_TO_TERMINATE:
         return "Deciding whether to terminate";
      case STATE_STOP:
         return "Stopped";
      case STATE_ERROR:
         return "Error";
      case STATE_UNDEFINED:
      default:
         return "UNDEFINED";
      }
   }

   bool step()
   {
      m_CurrentState = m_NextState;
      m_NextState = D::STATE_UNDEFINED;

      if (!m_ExternalStop)
      {
         switch (m_CurrentState)
         {
         case D::STATE_SETUP:
            stepSetup();
            break;
         case D::STATE_INITIALIZE:
            stepInitialize();
            break;
         case D::STATE_BEFORE_RIGID_ALIGNMENT:
            stepBeforeRigidAlignment();
            break;
         case D::STATE_RIGID_ALIGNMENT:
            DEBUGMACRO("RIGID");
            stepRigidAlignment();
            break;
         case D::STATE_MESH_REFINEMENT:
            DEBUGMACRO("STATE_MESH_REFINEMENT");
            stepMeshRefinement();
            break;
         case D::STATE_BEFORE_NRIGID_ALIGNMENT:
            stepBeforeNonRigidAlignment();
            break;
         case D::STATE_NRIGID_ALIGNMENT:
            DEBUGMACRO("NON RIGID");
            stepNonRigidAlignment();
            break;
         case D::STATE_DECIDE_TO_TERMINATE:
            stepDecideToTerminate();
            break;
         case D::STATE_STOP:
            m_Stopped = true;
            m_NextState = D::STATE_STOP;
            break;
         case D::STATE_ERROR:
            m_Error = true;
            m_NextState = D::STATE_ERROR;
            break;
         case D::STATE_UNDEFINED:
            m_Error = true;
            m_LastError = "STATE UNDEFINED!";
            break;
         }
      }
      else
      {
         m_Stopped = true;
         m_NextState = D::STATE_STOP;
      }

      return false;
   }

   bool stepSetup()
   {
      if (
         !m_SourceMeshFilenameSet || 
         !m_TargetMeshFilenameSet ||
         (m_MeshSpacings.size() == 0 && *m_Nra->getControlPoints() == 0)
        )
      {
         m_NextState = STATE_ERROR;

         if (!m_SourceMeshFilenameSet)
            m_LastError = "Source mesh filename not set!";
         else if (!m_TargetMeshFilenameSet)
            m_LastError = "Target mesh filename not set!";
         else if (m_MeshSpacings.size() == 0 && *m_Nra->getControlPoints() == 0)
            m_LastError = "No mesh spacings!";
         else
            m_LastError = "Shouldn't have got to here";

         return false;
      }

      m_NextState = STATE_INITIALIZE;

      return true;
   }

   bool stepInitialize()
   {
      m_MeshSpacingPointer = 0;

      m_TotalIterations = 0;

      m_LastRms         = std::numeric_limits<T>::infinity();

      m_NextState = m_LoadOnly ? STATE_STOP : STATE_BEFORE_RIGID_ALIGNMENT;

      m_AllowScale = false;

      return true;
   }

   bool stepBeforeRigidAlignment()
   {
      m_RigidIterations = 0;
      m_NextState           = STATE_RIGID_ALIGNMENT;

      return true;
   }

   bool stepBeforeNonRigidAlignment()
   {
      m_NRigidIterations = 0;
      m_NextState           = STATE_NRIGID_ALIGNMENT;

      return true;
   }

   bool stepRigidAlignment()
   {
      ++m_RigidIterations;
      ++m_TotalIterations;

      PRINTBOOL(m_AllowScale);
      m_Nra->rigidAlignmentStep(m_AllowScale);

      T gradient = m_Nra->getRmsError()-m_LastRms;

      m_LastRms = m_Nra->getRmsError();

      PRINTVBL(gradient);
      PRINTVBL(m_Nra->getRmsError());
      PRINTVBL(m_Nra->getMaxError());

      if (gradient > 0)
      {
         m_LastError = "Convergence is not monotonic!";
         DEBUGMACRO(m_LastError);
      }

      if (gradient > -1e-4 )
      {
         if (m_RigidOnly)
         {
            if (gradient < 0)
            {
               m_NextState = STATE_DECIDE_TO_TERMINATE;
               return true;
            }
            else
            {
               m_NextState = STATE_RIGID_ALIGNMENT;
               return true;
            }
         }
         else
         {
            if (!m_AllowScale)
            {
               m_AllowScale = true;
               m_NextState = STATE_BEFORE_RIGID_ALIGNMENT;
               return true;
            }
            else
            {
               m_NextState = STATE_BEFORE_NRIGID_ALIGNMENT;
               return true;
            }
         }
      }
      else
      {
         m_NextState = STATE_RIGID_ALIGNMENT;
      }

      return true;
   }

   bool stepMeshRefinement()
   {
      if (m_MeshSpacingPointer < m_MeshSpacings.size())
      {
         // Retrieve the spacing from the list
         T spacing = m_MeshSpacings[ m_MeshSpacingPointer ];

         // Increment the pointer so that we don't reuse this
         ++m_MeshSpacingPointer;

         ControlPointSet<T>::P cps = ControlPointSet<T>::getInstance(m_SourceBounds, spacing);

         if (*m_Nra->getControlPoints() != 0 && m_Nra->getControlPoints()->getNumberOfObjects() > 0)
         {
            // We need to fit to the existing deformation values
            cps->fit(m_Nra->getControlPoints());   
         }

         m_Nra->setControlPoints(cps);

         m_Nra->updateSourcePoints();
         
         PRINTVBL(m_Nra->getControlPoints()->getNumberOfObjects());
      }

      m_NextState = STATE_BEFORE_RIGID_ALIGNMENT; //STATE_BEFORE_NRIGID_ALIGNMENT;
      return true;
   }

   bool stepNonRigidAlignment()
   {
      ControlPointSet<T>::P cps = m_Nra->getControlPoints();

      if (*cps == 0)
      {
         m_NextState = STATE_MESH_REFINEMENT; //STATE_DECIDE_TO_TERMINATE;
         return true;
      }

      if (m_Nra->getControlPoints()->getNumberOfObjects() == 0)
      {
         m_NextState = STATE_MESH_REFINEMENT; //STATE_DECIDE_TO_TERMINATE;
         return true;
      }

      ++m_NRigidIterations;
      ++m_TotalIterations;
      m_Nra->nonRigidAlignmentStep();
      T rmsGradient = m_Nra->getRmsError()-m_LastRms;
      m_LastRms = m_Nra->getRmsError();

      PRINTVBL(rmsGradient);
      PRINTVBL(m_Nra->getRmsError());
      PRINTVBL(m_Nra->getMaxError());

      if (rmsGradient > 0)
      {
         m_LastError = "Convergence is not monotonic!";
         DEBUGMACRO(m_LastError);
      }

      if (abs(rmsGradient) < 1e-3 || m_NRigidIterations >= 20)
      {
         m_NextState = STATE_DECIDE_TO_TERMINATE;
         return true;
      }
      else
      {
         m_NextState = STATE_NRIGID_ALIGNMENT;
         return true;
      }
   }

   bool stepDecideToTerminate()
   {
      if (m_RigidIterations < 3 && m_NRigidIterations < 3)
      {
         m_NextState = STATE_STOP;
         return true;
      }
      else
      {
         m_NextState = STATE_MESH_REFINEMENT; //STATE_BEFORE_RIGID_ALIGNMENT;
         return true;
      }
   }

   typename NonRigidAlignment<T>::P m_Nra;

   State                 m_NextState;
   State                 m_CurrentState;

   bool                  m_SourceMeshFilenameSet;
   std::string           m_SourceMeshFilename;

   bool                  m_TargetMeshFilenameSet;
   std::string           m_TargetMeshFilename;

   std::vector<T>        m_MeshSpacings;
   unsigned int          m_MeshSpacingPointer;

   unsigned int          m_TotalIterations;
   unsigned int          m_RigidIterations;
   unsigned int          m_NRigidIterations;
   T                     m_LastRms;

   std::string           m_LastError;

   T                     m_SourceBounds[6];

   bool                  m_AllowScale;

   bool                  m_Stopped;
   bool                  m_Error;

   StateMachineStack::P  m_Stack;

   bool                  m_ExternalStop;
   bool                  m_LoadOnly;
   bool                  m_RigidOnly;
};

template <class T>
AWT::NonRigidAlignmentStateMachine<T>::NonRigidAlignmentStateMachine(StateMachineStack::P stack, const bool rigidOnly)
{
   m_D = new D;

   m_D->m_ExternalStop = false;
   m_D->m_LoadOnly = false;

   m_D->m_RigidOnly = rigidOnly;

   m_D->m_Stack = stack;

   m_D->m_Nra = NonRigidAlignment<T>::getInstance();
   
   m_D->m_SourceMeshFilenameSet = false;
   m_D->m_TargetMeshFilenameSet = false;

   m_D->m_Stopped = false;
   m_D->m_Error   = false;

   m_D->m_MeshSpacings.clear();

   m_D->m_NextState = D::STATE_SETUP;
}

template <class T>
AWT::NonRigidAlignmentStateMachine<T>::~NonRigidAlignmentStateMachine()
{
   delete m_D;
}

template <class T>
typename AWT::NonRigidAlignmentStateMachine<T>::P AWT::NonRigidAlignmentStateMachine<T>::getInstance(StateMachineStack::P stack, const bool rigidOnly)
{
   AUTOGETINSTANCE(AWT::NonRigidAlignmentStateMachine<T>, (stack, rigidOnly));
}

template <class T>
GETNAMEMACRO(AWT::NonRigidAlignmentStateMachine<T>);

template <class T>
AWT::NonRigidAlignment<T>* AWT::NonRigidAlignmentStateMachine<T>::getNonRigidAlignment()
{
   return *m_D->m_Nra;
}

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::setControlPoints(typename ControlPointSet<T>::P cps)
{
   m_D->m_MeshSpacings.clear();

   m_D->m_Nra->setControlPoints(cps);

   m_D->m_Nra->updateSourcePoints();

   return true;
}

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::addControlPointSpacing(T spacing)
{
   switch (m_D->m_NextState)
   {
   case D::STATE_SETUP:
      break;
   default:
      return false;
   }

   if (m_D->m_MeshSpacings.size() == 0 || m_D->m_MeshSpacings[m_D->m_MeshSpacings.size()-1] > spacing)
   {
      m_D->m_MeshSpacings.push_back(spacing);
      return true;
   }
   else
   {
      return false;
   }
}

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::setSourceMeshFilename(const char *fn)
{
   switch (m_D->m_NextState)
   {
   case D::STATE_SETUP:
      break;
   default:
      return false;
   }

   m_D->m_SourceMeshFilename = fn;
   m_D->m_SourceMeshFilenameSet = true;

   typename Mesh<T>::P mesh = VTKMeshLoader<T>::load(m_D->m_SourceMeshFilename.c_str());
   if (*mesh == 0)
   {
      m_D->m_NextState = D::STATE_ERROR;
      m_D->m_LastError = "Could not load mesh!";
      return false;
   }

   typename NormalCalculator<T>::P nc = AreaAveragedNormalCalculator<T>::getInstance();
   nc->calculateNormalsAndSet(*mesh);
   
   m_D->m_Nra->setSourceMesh(*mesh);

   // Calculate the mesh bounds
   {
      m_D->m_SourceBounds[0] = m_D->m_SourceBounds[2] = m_D->m_SourceBounds[4] =  std::numeric_limits<T>::infinity();
      m_D->m_SourceBounds[1] = m_D->m_SourceBounds[3] = m_D->m_SourceBounds[5] = -std::numeric_limits<T>::infinity();

      T vtx[3];
      //for (MeshIndex v = 0; v < mesh->getNumberOfVertices(); ++v)
      MESH_EACHVERTEX(mesh, v)
      {
         mesh->getVertex(v, vtx);

         for (int i = 0; i < 3; ++i)
         {
            m_D->m_SourceBounds[2*i+0] = std::min(m_D->m_SourceBounds[2*i+0], vtx[i]);
            m_D->m_SourceBounds[2*i+1] = std::max(m_D->m_SourceBounds[2*i+1], vtx[i]);
         }
      }
   }

   return true;
}

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::setTargetMeshFilename(const char *fn)
{
   switch (m_D->m_NextState)
   {
   case D::STATE_SETUP:
      break;
   default:
      return false;
   }

   m_D->m_TargetMeshFilename = fn;
   m_D->m_TargetMeshFilenameSet = true;

   Mesh<T>::P mesh = VTKMeshLoader<T>::load(m_D->m_TargetMeshFilename.c_str());
   if (*mesh == 0)
   {
      m_D->m_NextState = D::STATE_ERROR;
      m_D->m_LastError = "Could not load mesh!";
      return false;
   }

   typename NormalCalculator<T>::P nc = AreaAveragedNormalCalculator<T>::getInstance();
   nc->calculateNormalsAndSet(*mesh);
   
   if (false)
   {
      RandomMeshSampler<T>::P sampler = RandomMeshSampler<T>::getInstance();
      sampler->setMaximumNumberOfSamples(1000);

      SamplePoints::P            samples     = sampler->sampleMesh(mesh);
      typename SampledMesh<T>::P sampledMesh = SampledMesh<T>::getInstance(mesh, samples);

      m_D->m_Nra->setTargetMesh(sampledMesh);
   }
   else
   {
      m_D->m_Nra->setTargetMesh(*mesh);
   }

   return true;
}

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::step()
{
   return m_D->step();
}

template <class T>
void AWT::NonRigidAlignmentStateMachine<T>::getBounds(T* out_Bounds)
{
   for (int i = 0; i < 6; ++i)
      out_Bounds[i] = m_D->m_SourceBounds[i];
}

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::isError() const
{
   return m_D->m_Error;
}

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::isRunning() const
{
   return !m_D->m_Stopped;
}

template <class T>
std::string AWT::NonRigidAlignmentStateMachine<T>::getLastError() const
{
   return m_D->m_LastError;
}

template <class T>
std::string AWT::NonRigidAlignmentStateMachine<T>::getStateName() const
{
   return m_D->getStateName(m_D->m_CurrentState);
}

#define LOAD_SAVE_LUA 1

#ifndef LOAD_SAVE_LUA

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::load(const std::string& filename)
{
   std::ifstream is(filename.c_str());

   PRINTVBL(filename);

   std::string magic;
   std::getline(is, magic);

   PRINTVBL(magic);

   if (magic != getMagic())
   {
      std::stringstream sstream;
      sstream << "Invalid Magic Number (expecting " << getMagic() << ", found " << magic << ")";
      m_D->m_LastError = sstream.str();
      return false;
   }

   std::string sourceFilename;
   std::string targetFilename;

   is >> sourceFilename >> targetFilename;

   PRINTVBL(sourceFilename);
   PRINTVBL(targetFilename);

   try
   {
      setSourceMeshFilename(sourceFilename.c_str());
   }
   catch (AWT::Exception& ex)
   {
      m_D->m_LastError = ex.getMessage();
      return false;
   }

   try
   {
      setTargetMeshFilename(targetFilename.c_str());
   }
   catch (AWT::Exception& ex)
   {
      m_D->m_LastError = ex.getMessage();
      return false;
   }
   
   T transformation[4][4];

   for (int r = 0; r < 4 && !is.fail(); ++r)
      for (int c = 0; c < 4 && !is.fail(); ++c)
         is >> transformation[r][c];

   if (is.fail())
   {

      addControlPointSpacing(80.0);
      addControlPointSpacing(40.0);
      addControlPointSpacing(20.0);
      addControlPointSpacing(10.0);

      return true;
   }

   m_D->m_Nra->setRigidTransformation(transformation);

   ControlPointSet<T>::P cps = ControlPointSet<T>::getInstance();

   T pos[3], scale, val[3];
   unsigned int used;

   while (true)
   {
      is >> pos[0] >> pos[1] >> pos[2] >> scale >> val[0] >> val[1] >> val[2] >> used;

      if (is.fail())
         break;

      PRINTVEC(pos, 3);
      PRINTVEC(val, 3);
      PRINTVBL(scale);
      PRINTVBL(used*1);
      ControlPoint<T>::P cp = ControlPoint<T>::getInstance(pos, scale);
      cp->setValue(val);
      cp->setUsed(used != 0);
      cps->add(cp);
      
      if (is.eof())
         break;
   }

   setControlPoints(cps);

   is.close();

   return true;
}

template <class T>
void AWT::NonRigidAlignmentStateMachine<T>::save(const std::string& filename)
{
   std::ofstream os(filename.c_str());

   os << "<?xml version=\"1.0\"?>" << std::endl;
   os << "<nrigid>" << std::endl;

   os << "  <files>" << std::endl;
   os << "    <source>" << m_D->m_SourceMeshFilename << "</source>" << std::endl;
   os << "    <target>" << m_D->m_TargetMeshFilename << "</target>" << std::endl;
   os << "  </files>" << std::endl;

   T rot[3][3];
   T tra[3];
   T scale;
   scale = m_D->m_Nra->getScale();
   m_D->m_Nra->getRotation(rot);
   m_D->m_Nra->getTranslation(tra);

   os << "  <rigid>" << std::endl;

   for (int r = 0; r < 3; ++r)
   {
      for (int c = 0; c < 3; ++c)
         os << "    <m" << r << c << ">" << scale*rot[r][c] << "</m" << r << c << ">" << std::endl;

      os << "    <m" << r << "3>" << tra[r] << "</m" << r << "3>" << std::endl;
   }
   os << "    <m30>0</m30>" << std::endl;
   os << "    <m31>0</m31>" << std::endl;
   os << "    <m32>0</m32>" << std::endl;
   os << "    <m33>1</m33>" << std::endl;
   os << "  </rigid>" << std::endl;

   os << "  <nonrigid>" << std::endl;

   os << "    <refine>" << std::endl;
   for (unsigned int i = m_D->m_MeshSpacingPointer; i < m_D->m_MeshSpacings.size(); ++i)
   {
      os << "      <spacing>" << m_D->m_MeshSpacings[i] << "</spacing>" << std::endl;
   }
   os << "    </refine>" << std::endl;

   ControlPointSet<T>::P cps = m_D->m_Nra->getControlPoints();
   if (*cps != 0)
   {
      os << "    <controlPoints>" << std::endl;

      T pos[3];
      T val[3];
      T cscale;
      for (OEKDTree::ObjectIndex v = 0; v < cps->getNumberOfObjects(); ++v)
      {
         const ControlPoint<T>::P cp = cps->getControlPointConst(v);
         cp->getPosition(pos);
         cp->getValue(val);
         cscale = cp->getScale();

         os << "      <controlPoint>" << std::endl;
         os << "        <used>" << (cp->isUsed()?1:0) << "</used>" << std::endl;
         os << "        <scale>" << cscale << "</scale>" << std::endl;
         os << "        <pos>" << std::endl;
         os << "          <x>" << pos[0] << "</x>" << std::endl;
         os << "          <y>" << pos[1] << "</y>" << std::endl;
         os << "          <z>" << pos[2] << "</z>" << std::endl;
         os << "        </pos>" << std::endl;
         os << "        <val>" << std::endl;
         os << "          <x>" << val[0] << "</x>" << std::endl;
         os << "          <y>" << val[1] << "</y>" << std::endl;
         os << "          <z>" << val[2] << "</z>" << std::endl;
         os << "        </val>" << std::endl;
         os << "      </controlPoint>" << std::endl;
      }
      os << "    </controlPoints>" << std::endl;
   }

   os << "  </nonrigid>" << std::endl;

   os << "</nrigid>" << std::endl;

   os.close();
}

#else

#include "LuaPlus.h"

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::load(const std::string& filename)
{
   DEBUGMACRO("Lua is loading state machine from " << filename);

   LuaStateOwner state;

   int iret = state->DoFile(filename.c_str());
   if (iret != 0)
   {
      std::stringstream sstream;
      sstream << "Could not parse file (Lua returned error code " << iret << ")";
      m_D->m_LastError = sstream.str();
      return false;
   }

   LuaObject objJob = state->GetGlobal("alignment");

   if (objJob.IsNil())
   {
      m_D->m_LastError = "Could not find object called \"alignment\"";
      return false;
   }

   try
   {
      std::string sourceFilename(objJob["source"].GetString());
      setSourceMeshFilename(sourceFilename.c_str());
      PRINTVBL(sourceFilename);
   }
   catch (AWT::Exception& ex)
   {
      m_D->m_LastError = ex.getMessage();
      return false;
   }

   try
   {
      std::string targetFilename(objJob["target"].GetString());
      setTargetMeshFilename(targetFilename.c_str());
      PRINTVBL(targetFilename);
   }
   catch (AWT::Exception& ex)
   {
      m_D->m_LastError = ex.getMessage();
      return false;
   }

   ////////
   // Start: Get the Rigid Transformation
   {
      T transformation[4][4];

      // At the very least, set the transformation to identity
      for (int r = 0; r < 4; ++r)
         for (int c = 0; c < 4; ++c)
            transformation[r][c] = static_cast<T>((r == c) ? 1 : 0);

      PRINTVBL(*m_D->m_Nra);
      m_D->m_Nra->setRigidTransformation(transformation);

      try
      {
         LuaObject objRigid = objJob["rigid"];
         if (objRigid.IsTable())
         {
            for (int r = 0; r < 4; ++r)
            {
               LuaObject objRow = objRigid[r+1];

               for (int c = 0; c < 4; ++c)
                  transformation[r][c] = static_cast<T>(objRow[c+1].GetDouble());
            }
         }/*
         else if (!objRigid.IsNone())
         {
            m_D->m_Nra->setRigidScale(objRigid.GetDouble());
         }*/

         m_D->m_Nra->setRigidTransformation(transformation);
      }
      catch (AWT::Exception& ex)
      {
         m_D->m_LastError = ex.getMessage();
         return false;
      }

   }
   // End: Get the Rigid Transformation
   ////////

   try
   {
      LuaObject objNonRigid = objJob["nonrigid"];
      if (objNonRigid.IsTable())
      {
         PRINTVBL(objNonRigid.GetTableCount());

         ControlPointSet<T>::P cps = ControlPointSet<T>::getInstance();

         T position[3], value[3];
         for (int i = 1; i <= objNonRigid.GetTableCount(); ++i)
         {
            LuaObject objControlPoint = objNonRigid[i];

            bool used = objControlPoint["used"].GetBoolean();
            T scale   = static_cast<T>(objControlPoint["scale"].GetDouble());

            for (int ax = 0; ax < 3; ++ax)
            {
               position[ax] = static_cast<T>(objControlPoint["position"][ax+1].GetDouble());
               value[ax]    = static_cast<T>(objControlPoint["value"][ax+1].GetDouble());
            }

            ControlPoint<T>::P cp = ControlPoint<T>::getInstance(position, scale);
            cp->setValue(value);
            cp->setUsed(used);
            cps->add(cp);
         }

         setControlPoints(cps);
      }
      else
      {
         DEBUGMACRO("No control points; assuming normal (80,40,20) refinement strategy");
         addControlPointSpacing(80.0);
         addControlPointSpacing(40.0);
         addControlPointSpacing(20.0);
      }
   }
   catch (AWT::Exception& ex)
   {
      m_D->m_LastError = ex.getMessage();
      return false;
   }

   return true;
}

template <class T>
void AWT::NonRigidAlignmentStateMachine<T>::save(const std::string& filename)
{
   std::ofstream os(filename.c_str());

   os << "--[[ Non-rigid Alignment State Machine" << std::endl;
   os << "    RMS Move  = " << m_D->m_Nra->getRmsMove() << std::endl;
   os << "    Max Move  = " << m_D->m_Nra->getMaxMove() << std::endl;
   os << "    RMS Error = " << m_D->m_Nra->getRmsError() << std::endl;
   os << "    Max Error = " << m_D->m_Nra->getMaxError() << std::endl;
   os << "--]]" << std::endl << std::endl;

   os << "alignment = {" << std::endl;
   os << "  source = '" << m_D->m_SourceMeshFilename << "'," << std::endl;
   os << "  target = '" << m_D->m_TargetMeshFilename << "'," << std::endl;

   T rot[3][3];
   T tra[3];
   T scale;
   scale = m_D->m_Nra->getScale();
   m_D->m_Nra->getRotation(rot);
   m_D->m_Nra->getTranslation(tra);

   os << "  rigid = {" << std::endl;

   for (int r = 0; r < 3; ++r)
   {
      os << "    { ";
      for (int c = 0; c < 3; ++c)
         os << scale*rot[r][c] << ", ";

      os << tra[r] << " }," << std::endl;
   }
   os << "    { 0, 0, 0, 1 }" << std::endl;
   os << "  }," << std::endl;
   
   // Nothing in here about refinement, yet?

   ControlPointSet<T>::P cps = m_D->m_Nra->getControlPoints();
   if (*cps != 0)
   {
      os << "  nonrigid = {" << std::endl;

      T pos[3];
      T val[3];
      T cscale;
      for (OEKDTree::ObjectIndex v = 0; v < cps->getNumberOfObjects(); ++v)
      {
         const ControlPoint<T>::P cp = cps->getControlPointConst(v);
         cp->getPosition(pos);
         cp->getValue(val);
         cscale = cp->getScale();

         os << "    {" << std::endl;
         os << "      used = " << (cp->isUsed()?"true":"false") << "," << std::endl;
         os << "      scale = " << cscale << "," << std::endl;
         os << "      position = { " << pos[0] << ", " << pos[1] << ", " << pos[2] << " }," << std::endl;
         os << "      value = { " << val[0] << ", " << val[1] << ", " << val[2] << " }," << std::endl;
         os << "    }," << std::endl;
      }
      os << "  }," << std::endl;
   }

   os << "  }," << std::endl;

   os << "};" << std::endl;

   os.close();
}

#endif

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::saveDeformedModel(const char* fn) const
{
   VTKMeshWriter<T>::write(m_D->m_Nra->getSourceMesh(), fn);

   return true;
}

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::saveTargetModel(const char* fn) const
{
   VTKMeshWriter<T>::write(m_D->m_Nra->getTargetMesh(), fn);

   return true;
}

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::saveProjected(const char* fn) const
{
   DEBUGLINE;
   VTKMeshWriter<T>::write(m_D->m_Nra->getProjected(), fn);
   DEBUGLINE;

   return true;
}

template <class T>
std::string AWT::NonRigidAlignmentStateMachine<T>::getMagic() const
{
   return "NRAL";
}

template <class T>
AWT::StateMachineStack* AWT::NonRigidAlignmentStateMachine<T>::getStack()
{
   return *m_D->m_Stack;
}

template <class T>
void AWT::NonRigidAlignmentStateMachine<T>::externalStop()
{
   m_D->m_ExternalStop = true;
}

template <class T>
void AWT::NonRigidAlignmentStateMachine<T>::loadOnly()
{
   m_D->m_LoadOnly = true;
}

template <class T>
bool AWT::NonRigidAlignmentStateMachine<T>::aboutToRefine() const
{
   return m_D->m_NextState == D::STATE_MESH_REFINEMENT;
}

template class AWT::NonRigidAlignmentStateMachine<double>;
template class AWT::NonRigidAlignmentStateMachine<float>;
