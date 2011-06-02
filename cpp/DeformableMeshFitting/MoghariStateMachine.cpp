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
#include "MoghariStateMachine.h"

#include "Fragment.h"

#include "Mesh/AreaAveragedNormalCalculator.h"
#include "Mesh/SamplePoints.h"

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_svd.h"

#include "MoghariMeasure.h"

#include "Useful/ArrayVectorFunctions.h"

#include <fstream>
#include <queue>

template <class T>
struct AWT::MoghariStateMachine<T>::D
{
   enum State
   {
      STATE_INITIALIZE,
      STATE_NORMALIZE_MEASURES,
      STATE_CALCULATE_NCC,
      STATE_TRYPOSITION,
      STATE_PICKBEST,
      STATE_FINISHED,
      STATE_ERROR,
      STATE_UNDEFINED,
   };

   std::string getStateName( ) const
   {
      switch ( m_CurrentState )
      {
      case STATE_INITIALIZE:
         return "Initializing";
      case STATE_NORMALIZE_MEASURES:
         return "Normalizing Measures";
      case STATE_CALCULATE_NCC:
         return "Calculating NCCs";
      case STATE_TRYPOSITION:
         return "Trying positions";
      case STATE_PICKBEST:
         return "Picking best pose";
      case STATE_FINISHED:
         return "Finished";
      case STATE_ERROR:
         return "Error";
      case STATE_UNDEFINED:
      default:
         return "Undefined State!";
      }
   }

   D( )
      : m_Measures( 1, 1 )
   {
      m_NextState = m_CurrentState = STATE_INITIALIZE;
      m_LastError = "";
      m_SearchRadius = -1;
   }

   bool step( )
   {
      m_CurrentState = m_NextState;
      m_NextState = STATE_UNDEFINED;

      switch ( m_CurrentState )
      {
      case STATE_INITIALIZE:
         return stepInitialize( );
      case STATE_NORMALIZE_MEASURES:
         return stepNormalizeMeasures( );
      case STATE_CALCULATE_NCC:
         return stepCalculateNCCs( );
      case STATE_TRYPOSITION:
         return stepTryPosition( );
      case STATE_PICKBEST:
         return stepPickBest( );
      case STATE_FINISHED:
         return stepFinished( );
      case STATE_ERROR:
      case STATE_UNDEFINED:
      default:
         return false;
      }
   }

   bool stepInitialize( )
   {
      if ( *m_Fragment == 0 )
      {
         m_NextState = STATE_ERROR;
         m_LastError = "Source mesh not set!";
         return false;
      }

      if ( *m_TargetMesh == 0 )
      {
         m_NextState = STATE_ERROR;
         m_LastError = "Target mesh not set!";
         return false;
      }

      if ( m_Measures.cols() != 9 )
      {
         m_NextState = STATE_ERROR;
         m_LastError = "Measures not set!";
         return false;
      }

      if ( m_SearchRadius <= 0 )
      {
         m_NextState = STATE_ERROR;
         m_LastError = "Search radius not set!";
         return false;
      }

      m_NextState = STATE_NORMALIZE_MEASURES;
      return true;
   }

   bool stepNormalizeMeasures( )
   {
      T vec[9];

      // Normalize these because we are going to do NCC
      for ( MeshIndex v = 0; v < m_Measures.rows( ); ++v )
      {
         for ( int i = 0; i < 9; ++i )
            vec[i] = m_Measures[v][i];

         normalize<T>( vec, 9 );

         for ( int i = 0; i < 9; ++i )
            m_Measures[v][i] = vec[i];
      }

      m_NextState = STATE_CALCULATE_NCC;
      return true;
   }

   bool stepCalculateNCCs( )
   {
      // Pick a random vertex out of the fragment mesh
      m_SelectedVertex = ( ( rand( ) << 15 ) | rand( ) ) % m_Fragment->getMesh( )->getNumberOfVertices( );

      MoghariMeasure<T>::P mogMeas = MoghariMeasure<T>::getInstance( );

      vnl_vector<T> fragA( 9, 1 );
      T defAarr[9];

      // Calculate its a-vector
      mogMeas->calculateForVertex( Mesh<T>::P( m_Fragment->getMesh( ) ), m_SelectedVertex, m_SearchRadius, fragA );

      // ... and normalize it
      fragA.normalize( );

      // Clear the priority queue
      while ( !m_PriorityQueue.empty() )
         m_PriorityQueue.pop();

      // Now calculate all of the NCCs and put them into a priority queue
      for ( MeshIndex v = 0; v < m_Measures.rows( ); ++v )
      {
         for ( int i = 0; i < 9; ++i )
            defAarr[i] = m_Measures[v][i];

         T ncc = dot<T>( defAarr, fragA.data_block(), 9 );

         m_PriorityQueue.push( NCCPair( ncc, v ) );
      }

      m_NumberOfTries = 0;
      m_BestRms = std::numeric_limits<T>::infinity( );

      m_NextState = STATE_TRYPOSITION;
      return true;
   }

   void set4x4( vnl_matrix_fixed<T,4,4> matrixA, vnl_matrix_fixed<T,3,3> rot, vnl_vector_fixed<T,3> trans )
   {
      for ( unsigned int r = 0; r < 3; ++r )
      {
         for ( unsigned int c = 0; c < 3; ++c )
            matrixA(r,c) = rot(r,c);

         matrixA(r,3) = trans(r);
         matrixA(3,r) = 0;
      }
      matrixA(3,3) = 0;
   }

   bool stepTryPosition( )
   {
      // Do the transformation here

      Mesh<T>::P fragMesh  = m_Fragment->getMesh( );

      // Reset it to identity pose
      m_Fragment->setInitialPose( Pose<T>::getInstance( ) );

      // Righty tighty.  In order to calculate the matching transform, we need
      // to get the statistics of the two point neighbourhoods

      typedef typename MoghariMeasure<T>::LocalStatistics LocalStats;

      LocalStats* statsA = new LocalStats;
      LocalStats* statsB = new LocalStats;

      Pose<T>::P poseB = Pose<T>::getInstance( );

      vnl_matrix_fixed<T,4,4> matrixA;
      vnl_matrix_fixed<T,4,4> matrixB;

      MoghariMeasure<T>::P mogMeas = MoghariMeasure<T>::getInstance( );

      // Get the local statistics around our random test point
      mogMeas->calculateLocalStatistics( fragMesh, m_SelectedVertex, m_SearchRadius, statsA );

      PRINTVBL( statsA );

      // Get the pose described by these stats, relative to the common coordinate system
      set4x4( matrixA, statsA->eigenvectors, statsA->mean );

      vnl_matrix<T> iA = vnl_svd<T>( matrixA ).inverse( );

      vnl_matrix<T> rotate( 3, 3 );

      NormalCalculator<T>::P nc = AreaAveragedNormalCalculator<T>::getInstance( );

      D::NCCPair top = m_PriorityQueue.top( );
      DEBUGMACRO( top.second << "\t" << top.first );

      // Get the local statistics around the "matching" point on the other mesh
      mogMeas->calculateLocalStatistics( m_TargetMesh, top.second, m_SearchRadius, statsB );

      // Now I need to calculate the similarity transform.  Each of the point sets can be brought into the
      // neutral coordinate system by translating by -mean, then rotating by inv(eigenvalues).

      // Try the 3 RH and 3 LH coord system permutations
      for ( int flip = 0; flip < 2; ++flip )
      {
         for ( int rot = 0; rot < 3; ++rot )
         {
            for ( int c = 0; c < 3; ++c )
            {
               rotate[0][c] = statsB->eigenvectors[0][(c+rot)%3];
               rotate[1][c] = statsB->eigenvectors[1][(c+rot)%3];
               rotate[2][c] = (1-(2*flip)) * statsB->eigenvectors[2][(c+rot)%3];
            }

            set4x4( matrixB, rotate, statsB->mean );

            poseB->setMatrix( matrixB * iA );

            m_Fragment->setInitialPose( poseB );

            nc->calculateNormalsAndSet( fragMesh );

            m_Fragment->createCorrespondenceFinder( *m_TargetMesh );

            PRINTVBL2( "rms", m_Fragment->getCorrespondenceFinder( )->getRmsError( ) );

            T rmsError = m_Fragment->getCorrespondenceFinder( )->getRmsError( );

            if ( rmsError < m_BestRms )
            {
               DEBUGMACRO( "*" );
               m_BestRms = m_Fragment->getCorrespondenceFinder( )->getRmsError( );
               m_BestPose = poseB;
            }
         }
      }

      // Pop so that we can look at the next one...
      m_PriorityQueue.pop( );
      
      if ( ++m_NumberOfTries >= 200 )
      {
         m_NextState = STATE_PICKBEST;
      }
      else
      {
         m_NextState = STATE_TRYPOSITION;
      }

      return true;
   }

   bool stepPickBest( )
   {
      DEBUGLINE;

      PRINTVBL( m_BestRms );
      PRINTVBL( m_BestPose->getMatrix() );

DEBUGLINE;

      m_Fragment->setInitialPose( m_BestPose );

DEBUGLINE;

      m_NextState = STATE_FINISHED;

      return true;
   }

   bool stepFinished( )
   {
      m_NextState = STATE_FINISHED;
      return true;
   }

   bool isRunning( ) const
   {
      return !isError( ) && m_CurrentState != STATE_FINISHED;
   }

   bool isError( ) const
   {
      return m_CurrentState == STATE_ERROR || m_CurrentState == STATE_UNDEFINED;
   }

   std::string getLastError( ) const
   {
      return m_LastError;
   }

   void setFragment( typename Fragment<T>::P frag )
   {
      m_Fragment = frag;
   }

   void setTargetMesh( typename Mesh<T>::P mesh )
   {
      m_TargetMesh = mesh;
   }

   void setMeasures( vnl_matrix<T>& meas )
   {
      m_Measures = meas;
   }

   void setSearchRadius( const T v )
   {
      m_SearchRadius = v;
   }

   State                          m_NextState;
   State                          m_CurrentState;

   std::string                    m_LastError;

   typename Fragment<T>::P        m_Fragment;
   typename Mesh<T>::P            m_TargetMesh;

   vnl_matrix<T>                  m_Measures;

   typedef std::pair<T,MeshIndex> NCCPair;

   std::priority_queue<NCCPair>   m_PriorityQueue;
   unsigned int                   m_NumberOfTries;

   MeshIndex                      m_SelectedVertex;

   T                              m_SearchRadius;

   StateMachineStack::P           m_Stack;

   T                              m_BestRms;
   typename Pose<T>::P            m_BestPose;
};

template <class T>
AWT::MoghariStateMachine<T>::MoghariStateMachine( AWT::StateMachineStack::P stack )
{
   m_D = new D;

   m_D->m_Stack = stack;
}

template <class T>
AWT::MoghariStateMachine<T>::~MoghariStateMachine( )
{
   delete m_D;
}

template <class T>
typename AWT::MoghariStateMachine<T>::P AWT::MoghariStateMachine<T>::getInstance( AWT::StateMachineStack::P stack )
{
   AUTOGETINSTANCE( AWT::MoghariStateMachine<T>, ( stack ) );
}

template <class T>
GETNAMEMACRO( AWT::MoghariStateMachine<T> );

template <class T>
AWT::StateMachineStack* AWT::MoghariStateMachine<T>::getStack( )
{
   return *m_D->m_Stack;
}

template <class T>
bool AWT::MoghariStateMachine<T>::step( )
{
   return m_D->step( );
}

template <class T>
bool AWT::MoghariStateMachine<T>::isRunning( ) const
{
   return m_D->isRunning( );
}

template <class T>
bool AWT::MoghariStateMachine<T>::isError( ) const
{
   return m_D->isError( );
}

template <class T>
std::string AWT::MoghariStateMachine<T>::getLastError( ) const
{
   return m_D->getLastError( );
}

template <class T>
std::string AWT::MoghariStateMachine<T>::getStateName( ) const
{
   return m_D->getStateName( );
}

template <class T>
void AWT::MoghariStateMachine<T>::setFragment( typename AWT::Fragment<T>::P frag )
{
   m_D->setFragment( frag );
}

template <class T>
void AWT::MoghariStateMachine<T>::setTargetMesh( typename AWT::Mesh<T>::P mesh )
{
   m_D->setTargetMesh( mesh );
}

template <class T>
void AWT::MoghariStateMachine<T>::setMeasureFilename( const T radius, const std::string& filename )
{
   m_D->setSearchRadius( radius );

   std::ifstream is( filename.c_str( ) );

   std::vector<T> vect;

   T val;

   while ( true )
   {
      is >> val;

      if ( !is.good( ) || is.bad( ) )
         break;
      
      vect.push_back( val );
   }

   if ( vect.size( ) / 9 == m_D->m_TargetMesh->getNumberOfVertices( ) )
   {
      vnl_matrix<T> as( static_cast<unsigned int>( vect.size( ) ) / 9, 9 );

      for ( unsigned int r = 0; r < as.rows( ); ++r )
         for ( unsigned int c = 0; c < as.cols( ); ++c )
            as(r,c) = vect[ 9*r + c ];

      m_D->m_Measures = as;
   }

   is.close( );
}

template class AWT::MoghariStateMachine<double>;
template class AWT::MoghariStateMachine<float>;