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
#include "DeformableMeshFittingStateMachine.h"

#include "DeformableMesh/DeformableMesh.h"
#include "DeformableMesh/DeformableMeshLoader.h"
#include "DeformableMeshFittingParser.h"

#include "DeformableMeshFitting.h"

#include "Useful/Exception.h"

#include "Mesh/Mesh.h"
#include "Mesh/MeshSimilarity.h"
#include "Mesh/RandomMeshSampler.h"
#include "Mesh/NormalSpaceSampler.h"
#include "Mesh/SamplePoints.h"
#include "Mesh/SampledMesh.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/Pose.h"

#include "Useful/Noise.h"
#include "Useful/PrintMacros.h"
#include "Useful/ReadEntireFile.h"

#include <sstream>
#include <fstream>
#include <limits>
#include <vector>

#include "MoghariStateMachine.h"

template <class T>
struct AWT::DeformableMeshFittingStateMachine<T>::D
{
   enum State
   {
      STATE_SETUP,
      STATE_INITIALIZE,
      STATE_EVALONLY,
      STATE_RIGID,
      STATE_PARAMETER,
      STATE_DECIDE_TO_TERMINATE,
      STATE_STOP,
      STATE_ERROR,
      STATE_UNDEFINED
   };

   std::string getStateName( State state )
   {
      switch ( state )
      {
      case STATE_SETUP:
         return "Setup";
      case STATE_INITIALIZE:
         return "Initializing";
      case STATE_EVALONLY:
         return "Evaluate only";
      case STATE_RIGID:
         return "Pose Update";
      case STATE_PARAMETER:
         return "Model Parameter Update";
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

   void step( )
   {
      m_CurrentState = m_NextState;
      m_NextState = D::STATE_UNDEFINED;

      switch ( m_CurrentState )
      {
      case STATE_SETUP:
         stepSetup( );
         break;
      case STATE_INITIALIZE:
         stepInitialize( );
         break;
      case STATE_EVALONLY:
         stepEvalOnly( );
         break;
      case STATE_RIGID:
         stepRigidUpdate( );
         break;
      case STATE_PARAMETER:
         stepParameterUpdate( );
         break;
      case STATE_DECIDE_TO_TERMINATE:
         stepDecideToTerminate( );
         break;
      case STATE_STOP:
         stepStop( );
         break;
      case STATE_ERROR:
         stepError( );
         break;
      case STATE_UNDEFINED:
      default:
         stepUndefined( );
      }
   }

   bool stepSetup( )
   {
      if ( *m_MeshFitting == 0 )
      {
         m_Error = true;
         m_NextState = STATE_ERROR;
         m_LastError = "Not initialized";

         return false;
      }

      m_LastError = "";
      m_Error = false;
      m_Stopped = false;

      m_DonePoseInitialization = false;

      m_NextState = STATE_INITIALIZE;

      m_DoParameters = false;

      // This causes us to branch off to a separate state machine to do the initial alignment
      if ( false )
      {
         m_MoghariStateMachine = MoghariStateMachine<T>::getInstance( *m_Stack );

         m_MoghariStateMachine->setTargetMesh( m_MeshFitting->getDeformableMesh( ) );
         m_MoghariStateMachine->setFragment( m_MeshFitting->getFragment( 0 ) );
         m_MoghariStateMachine->setMeasureFilename( m_MoghariRadius, m_MoghariFilename );

         m_Stack->push( m_MoghariStateMachine );
      }

      return true;
   }

   bool stepInitialize( )
   {
      m_RigidIters = 0;
      m_ModelIters = 0;

      m_MeshFitting->updateCorrespondences( );

      m_StartingRmsError = m_MeshFitting->getRmsError( );

      if ( m_EvalOnly )
         m_NextState = STATE_EVALONLY;
      else
         m_NextState = STATE_RIGID;
      
      m_LastRms         = std::numeric_limits<T>::infinity( );

      return true;
   }

   bool stepEvalOnly( )
   {
      m_MeshFitting->updateCorrespondences( );
      m_NextState = STATE_STOP;
      return true;
   }

   bool stepRigidUpdate( )
   {
      m_MeshFitting->updateCorrespondences( );
      m_MeshFitting->stepRigidUpdate( );
      
      T currentRms = m_MeshFitting->getRmsError( );

      T gradient = currentRms - m_LastRms;

      m_LastRms = currentRms;

      ++m_RigidIters;

      if ( m_RigidIters >= 10 || gradient > -1e-4 )
      {
         m_NextState = STATE_PARAMETER;
      }
      else
      {
         m_NextState = STATE_RIGID;
      }

      return true;
   }

   bool stepParameterUpdate( )
   {
      T gradient;

      if ( m_DoParameters )
      {
         m_MeshFitting->updateCorrespondences( );
         //DEBUGMACRO( "Skipping parameter update!" );
         m_MeshFitting->stepParameterUpdate( );

         T currentRms = m_MeshFitting->getRmsError( );

         gradient = currentRms - m_LastRms;
         
         m_LastRms = currentRms;

         ++m_ModelIters;
      }
      else
      {
         gradient = 0;
      }

      if ( m_ModelIters >= 10 || gradient > -1e-4 )
      {
         m_NextState = STATE_DECIDE_TO_TERMINATE;
      }
      else
      {
         m_NextState = STATE_PARAMETER;
      }

      return true;
   }

   bool stepDecideToTerminate( )
   {
      T deltaRms = m_StartingRmsError - m_MeshFitting->getRmsError( );

      PRINTVBL( deltaRms );

      if ( deltaRms < 1e-2 )
      {
         if ( !m_DoParameters )
         {
            m_DoParameters = true;
            m_NextState = STATE_INITIALIZE;
         }
         else
         {
            m_NextState = STATE_STOP;
         }
      }
      else
      {
         m_NextState = STATE_INITIALIZE;
      }

      return true;
   }

   bool stepStop( )
   {
      m_Stopped = true;
      m_NextState = STATE_STOP;

      return true;
   }

   bool stepError( )
   {
      m_Error = true;
      m_NextState = STATE_ERROR;

      return false;
   }

   bool stepUndefined( )
   {
      m_Error = true;
      m_LastError = "UNDEFINED STATE";
      
      return false;
   }

   D()
   {
      m_EvalOnly = false;
   }

   State                                 m_NextState;             
   State                                 m_CurrentState;          
                                       
   typename DeformableMeshFitting<T>::P  m_MeshFitting;           
   std::string                           m_LastError;             
                                       
   std::string                           m_DeformFilename;        
   std::vector<std::string>              m_TargetFilenames;       
                                       
   bool                                  m_DeformFilenameSet;     
   bool                                  m_TargetFilenameSet;     
                                       
   bool                                  m_MoghariFilenameSet;    
   std::string                           m_MoghariFilename;       
   T                                     m_MoghariRadius;         
                                       
   bool                                  m_DonePoseInitialization;
                                       
   bool                                  m_Error;                 
   bool                                  m_Stopped;               
                                       
   bool                                  m_DoParameters;          
                                       
   unsigned int                          m_RigidIters;            
   unsigned int                          m_ModelIters;            
                                       
   T                                     m_StartingRmsError;      
   T                                     m_LastRms;               
                                       
   typename MeshSimilarity<T>::P         m_MeshSimilarity;        
                                       
   StateMachineStack::P                  m_Stack;                 
                                       
   typename MoghariStateMachine<T>::P    m_MoghariStateMachine;   
                                       
   bool                                  m_EvalOnly;              

};

template <class T>
AWT::DeformableMeshFittingStateMachine<T>::DeformableMeshFittingStateMachine( StateMachineStack::P stack )
{
   m_D = new D;

   m_D->m_Stack = stack;

   m_D->m_MeshFitting = DeformableMeshFitting<T>::getInstance( );
   
   m_D->m_NextState    = D::STATE_SETUP;
   m_D->m_CurrentState = D::STATE_SETUP;

   m_D->m_LastError = "";

   m_D->m_DeformFilename = "";
   m_D->m_TargetFilenames.clear( );

   m_D->m_DeformFilenameSet = false;
   m_D->m_TargetFilenameSet = false;

   m_D->m_Error = false;
   m_D->m_Stopped = false;

   m_D->m_LastRms    = std::numeric_limits<T>::infinity( );

   m_D->m_RigidIters = 0;
   m_D->m_ModelIters = 0;                               

   m_D->m_StartingRmsError = std::numeric_limits<T>::infinity( );
}

template <class T>
AWT::DeformableMeshFittingStateMachine<T>::~DeformableMeshFittingStateMachine( )
{
   delete m_D;
}

template <class T>
typename AWT::DeformableMeshFittingStateMachine<T>::P AWT::DeformableMeshFittingStateMachine<T>::getInstance( StateMachineStack::P stack )
{
   AUTOGETINSTANCE( AWT::DeformableMeshFittingStateMachine<T>, ( stack ) );
}

template <class T>
typename AWT::DeformableMeshFitting<T>::P AWT::DeformableMeshFittingStateMachine<T>::getDeformableMeshFitting( )
{
   return m_D->m_MeshFitting;
}

template <class T>
GETNAMEMACRO( AWT::DeformableMeshFittingStateMachine<T> );

template <class T>
std::string AWT::DeformableMeshFittingStateMachine<T>::getStateName( ) const
{
   return m_D->getStateName( m_D->m_CurrentState );
}

template <class T>
bool AWT::DeformableMeshFittingStateMachine<T>::step( )
{
   if ( *m_D->m_MeshFitting != 0 )
   {
      T rms = m_D->m_MeshFitting->getRmsError( );
      T max = m_D->m_MeshFitting->getMaxError( );

      std::cerr << "R:" << rms << "\tM:" << max << std::endl;
   }

   m_D->step( );

   return true;
}

template <class T>
void AWT::DeformableMeshFittingStateMachine<T>::addTargetFilename( std::string& filename )
{
   m_D->m_TargetFilenames.push_back( filename );
   m_D->m_TargetFilenameSet = true;

   typename Mesh<T>::P targetMesh = VTKMeshLoader<T>::load( filename.c_str( ) );

   Mesh<T>* meshPtr = *targetMesh;

   //RandomMeshSampler<T>* randMesh = RandomMeshSampler<T>::getInstance( );
   NormalSpaceSampler<T>::P randMesh = NormalSpaceSampler<T>::getInstance( );
   //randMesh->setMaximumNumberOfSamples( std::numeric_limits<MeshIndex>::max( ) /*5000*/ );
   randMesh->setMaximumNumberOfSamples( targetMesh->getNumberOfVertices( ) / 10 );

   //PRINTVBL( randMesh->getMaximumNumberOfSamples() );

   SamplePoints::P sampPts = randMesh->sampleMesh( meshPtr );

   //PRINTVBL( sampPts->getNumberOfSamples() );

   //m_D->m_MeshFitting->setTargetMesh( msp->getExcluded( ) );
   m_D->m_MeshFitting->addTargetMesh( meshPtr, sampPts );
   //m_D->m_MeshFitting->setSamplePoints( sampPts );

   m_D->m_CurrentState = m_D->m_NextState = D::STATE_SETUP;
}

template <class T>
void AWT::DeformableMeshFittingStateMachine<T>::setDeformableFilename( std::string& filename )
{
   m_D->m_DeformFilename    = filename;
   m_D->m_DeformFilenameSet = true;    

   typename DeformableMesh<T>::P defMesh = DeformableMeshLoader<T>::load( filename.c_str( ) );

   m_D->m_MeshFitting->setDeformableMesh( *defMesh );

   m_D->m_CurrentState = m_D->m_NextState = D::STATE_SETUP;
}

template <class T>
void AWT::DeformableMeshFittingStateMachine<T>::setInitialPose( const AWT::TargetIndex i, typename AWT::Pose<T>::P pose )
{
   m_D->m_MeshFitting->setInitialPose( i, pose );
}

template <class T>
void AWT::DeformableMeshFittingStateMachine<T>::setInitialParameters( const T* params )
{
   m_D->m_MeshFitting->setInitialParameters( params );
}

template <class T>
bool AWT::DeformableMeshFittingStateMachine<T>::load( const std::string& filename )
{
   std::string contents;

   try
   {
      contents = ReadEntireFile::read( filename );
   }
   catch ( AWT::Exception ex )
   {
      return false;
   }

   DeformableMeshFittingParser<T>::P dmfp = DeformableMeshFittingParser<T>::getInstance( );

   bool success = dmfp->parse( contents, this );

   return success;
}

template <class T>
void AWT::DeformableMeshFittingStateMachine<T>::save( const std::string& filename )
{
   std::ofstream os( filename.c_str( ) );

   os << "deformable" << std::endl;
   os << "\t" << "\"" << m_D->m_DeformFilename << "\"" << std::endl;
   os << "\tparams = [";
   {
      unsigned int nmodes = m_D->m_MeshFitting->getDeformableMesh( )->getNumberOfActiveModes( );
      T* params = new T[ nmodes ];

      m_D->m_MeshFitting->getDeformableMesh( )->getModeWeights( params );

      for ( unsigned int i = 0; i < nmodes; ++i )
         os << params[i] << " ";

      delete[] params;
   }
   os << "]"   << std::endl;
   os << "end" << std::endl;

   vnl_matrix_fixed<T,3,3> R;
   vnl_vector_fixed<T,3> t;

   typename Pose<T>::P pose = Pose<T>::getInstance( );

   for ( TargetIndex i = 0; i < m_D->m_MeshFitting->getNumberOfTargetMeshes( ); ++i )
   {
      os << "fragment" << std::endl;

      os << "\t" << "\"" << m_D->m_TargetFilenames[i] << "\"" << std::endl;
      os << "\t" << "pose = [" << std::endl;

      m_D->m_MeshFitting->getPose( i, *pose );

      typename Pose<T>::PoseMatrix mtx = pose->getMatrix( );

      os << "\t\t" << mtx[0][0] << " " << mtx[0][1] << " " << mtx[0][2] << " " << mtx[0][3] << std::endl;
      os << "\t\t" << mtx[1][0] << " " << mtx[1][1] << " " << mtx[1][2] << " " << mtx[1][3] << std::endl;
      os << "\t\t" << mtx[2][0] << " " << mtx[2][1] << " " << mtx[2][2] << " " << mtx[2][3] << std::endl;
       
      os << "\t"   << "]"       << std::endl;
         

      os << "end" << std::endl;
   }

   T rms = m_D->m_MeshFitting->getRmsError( );
   T max = m_D->m_MeshFitting->getMaxError( );

   os << "/*" << std::endl;
   os << "rms " << rms << std::endl;
   os << "max " << max << std::endl;

   for ( TargetIndex i = 0; i < m_D->m_MeshFitting->getNumberOfTargetMeshes( ); ++i )
      for ( TargetIndex j = i+1; j < m_D->m_MeshFitting->getNumberOfTargetMeshes( ); ++j )
      {
         T trans, ang;
         DEBUGLINE;
         PRINTVBL( i );
         PRINTVBL( j );
         m_D->m_MeshFitting->calculateRegistrationError( i, j, ang, trans );
         DEBUGLINE;

         os << "err_ang(" << i << "," << j << ") " << ang   << std::endl;
         os << "err_tra(" << i << "," << j << ") " << trans << std::endl;
      }

   os << "*/" << std::endl;

   os.close( );
}

template <class T>
bool AWT::DeformableMeshFittingStateMachine<T>::isRunning( ) const
{
   return !m_D->m_Error && !m_D->m_Stopped;
}

template <class T>
bool AWT::DeformableMeshFittingStateMachine<T>::isError( ) const
{
   return m_D->m_Error;
}

template <class T>
std::string AWT::DeformableMeshFittingStateMachine<T>::getLastError( ) const
{
   return m_D->m_LastError;
}

template <class T>
std::string AWT::DeformableMeshFittingStateMachine<T>::getMagic( ) const
{
   return "DEMF";
}

template <class T>
AWT::MeshIndex AWT::DeformableMeshFittingStateMachine<T>::getNumberOfTargetFilenames( ) const
{
   return static_cast<AWT::MeshIndex>( m_D->m_TargetFilenames.size( ) );
}

template <class T>
void AWT::DeformableMeshFittingStateMachine<T>::setMoghariFilename( const T radius, const std::string& filename )
{
   m_D->m_MoghariRadius = radius;
   m_D->m_MoghariFilename = filename;
}

template <class T>
AWT::StateMachineStack* AWT::DeformableMeshFittingStateMachine<T>::getStack( )
{
   return *m_D->m_Stack;
}

template <class T>
void AWT::DeformableMeshFittingStateMachine<T>::evalOnly( )
{
   m_D->m_EvalOnly = true;
}

template class AWT::DeformableMeshFittingStateMachine<double>;
template class AWT::DeformableMeshFittingStateMachine<float>;