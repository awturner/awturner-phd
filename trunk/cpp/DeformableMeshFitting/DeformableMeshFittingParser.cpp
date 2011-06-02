#include "DeformableMeshFittingParser.h"

#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/escape_char.hpp>
#include <boost/spirit/utility/confix.hpp>
#include <iostream>
#include <vector>
#include <map>

#include "DeformableMeshFitting/DeformableMeshFittingStateMachine.h"

#include "Mesh/Mesh.h"
#include "Mesh/RandomMeshSampler.h"
#include "Mesh/SamplePoints.h"
#include "Mesh/VTKMeshLoader.h"
#include "Mesh/Pose.h"

#include "DeformableMesh/DeformableMesh.h"
#include "DeformableMesh/DeformableMeshLoader.h"

using namespace boost::spirit;

template <class T>
struct AWT::DeformableMeshFittingParser<T>::D
{
   class DeformableMeshFittingParseDataHolder
   {
   public:
      DeformableMeshFittingParseDataHolder( DeformableMeshFittingStateMachine<T>* dmfsm )
         : m_Dmfsm( dmfsm ), m_MoghariRadius( -1 ) /* This is a nonsensical value */
      {
      }

      void loadDeformableMesh( )
      {
         //std::cerr << "I will load the deformable model from " << m_DeformableFilename << std::endl;

         m_Dmfsm->setDeformableFilename( m_DeformableFilename );

         if ( m_MoghariFilename.length( ) > 0 )
         {
            m_Dmfsm->setMoghariFilename( static_cast<T>( m_MoghariRadius ), m_MoghariFilename );
         }

         if ( m_DeformableParameters.size( ) > 0 )
         {
            T* params = new T[ m_DeformableParameters.size( ) ];

            for ( unsigned int i = 0; i < m_DeformableParameters.size( ); ++i )
               params[i] = static_cast<T>( m_DeformableParameters[i] );

            m_Dmfsm->setInitialParameters( params );

            //PRINTVEC( params, m_DeformableParameters.size( ) );

            delete[] params;
         }
         else
         {
            //std::cerr << " but no parameters were set" << std::endl;
         }
      }

      void startFragment( )
      {
         m_FragmentFilename = "";
         m_FragmentPose.clear( );
      }

      void createFragment( ) 
      {
         DEBUGLINE;
         //Mesh<T>* mesh = VTKMeshLoader<T>::load( m_FragmentFilename.c_str( ) );

         typename Pose<T>::PoseMatrix poseMatrix;

         poseMatrix.set_identity( );

         //std::cerr << "I will load a fragment from " << m_FragmentFilename;

         if ( m_FragmentPose.size( ) == 12 )
         {
            for ( unsigned int r = 0; r < 3; ++r )
            {
               for ( unsigned int c = 0; c < 3; ++c )
                  poseMatrix[r][c] = static_cast<T>( m_FragmentPose[ 4*r + c ] );

               poseMatrix[r][3] = static_cast<T>( m_FragmentPose[ 4*r + 3 ] );
            }

            //std::cerr << " and will set its pose to" << std::endl;

            //for ( unsigned int r = 0; r < 3; ++r )
            //{
            //   for ( unsigned int c = 0; c < 4; ++c )
            //      std::cerr << m_FragmentPose[ 4*r + c ] << " ";

            //   std::cerr << std::endl;
            //}
         }
         else
         {
            //std::cerr << " with identity pose" << std::endl;
         }

         TargetIndex i = m_Dmfsm->getNumberOfTargetFilenames( );

         typename Pose<T>::P pose = Pose<T>::getInstance( );
         
         pose->setMatrix( poseMatrix );

         m_Dmfsm->addTargetFilename( m_FragmentFilename );
         
         m_Dmfsm->setInitialPose( i, pose );
      }

      void evalOnly( )
      {
         m_Dmfsm->evalOnly( );
      }

      DeformableMeshFittingStateMachine<T>* m_Dmfsm;

      std::string                           m_DeformableFilename;
      std::vector<double>                   m_DeformableParameters;

      std::string                           m_MoghariFilename;
      double                                m_MoghariRadius;

      std::string                           m_FragmentFilename;
      std::vector<double>                   m_FragmentPose;
   };

   struct DeformableMeshFittingGrammar : public grammar< DeformableMeshFittingGrammar >
   {
      DeformableMeshFittingGrammar( DeformableMeshFittingParseDataHolder* pdh )
         : m_Pdh( pdh )
      {};

      struct functorAssignString
      {
         functorAssignString( std::string& str_ )
            : str( str_ ) {};

         template<typename IteratorT >
         void operator( )( IteratorT first, IteratorT last ) const
         {
            str.assign(first, last);
         }

         std::string& str;
      };

      struct functorEndDeformable
      {
         functorEndDeformable( DeformableMeshFittingParseDataHolder* pdh )
         {
            this->pdh = pdh;
         }

         template <typename ScannerT>
         void operator( )( ScannerT /*first*/, ScannerT /*last*/ ) const
         {
            pdh->loadDeformableMesh( );
         }

         DeformableMeshFittingParseDataHolder* pdh;
      };

      struct functorStartFragment
      {
         functorStartFragment( DeformableMeshFittingParseDataHolder* pdh )
         {
            this->pdh = pdh;
         }

         template <typename ScannerT>
         void operator( )( ScannerT first, ScannerT last ) const
         {
            pdh->startFragment( );
         }

         DeformableMeshFittingParseDataHolder* pdh;
      };

      struct functorEndFragment
      {
         functorEndFragment( DeformableMeshFittingParseDataHolder* pdh )
         {
            this->pdh = pdh;
         }

         template <typename ScannerT>
         void operator( )( ScannerT first, ScannerT last ) const
         {
            pdh->createFragment( );
         }

         DeformableMeshFittingParseDataHolder* pdh;
      };

      struct functorEvalOnly
      {
         functorEvalOnly( DeformableMeshFittingParseDataHolder* pdh )
         {
            this->pdh = pdh;
         }

         template <typename ScannerT>
         void operator( )( ScannerT first, ScannerT last ) const
         {
            std::cerr << "Setting eval only" << std::endl;
            pdh->evalOnly( );
         }

         DeformableMeshFittingParseDataHolder* pdh;
      };

      template <typename ScannerT>
      struct definition
      {
         definition( DeformableMeshFittingGrammar const& self )
         { 
            pose = 
               str_p("pose") >>
               ch_p('=') >>
               str_p("[") >>
               (
                  real_p[ push_back_a( self.m_Pdh->m_FragmentPose ) ] >>
                  real_p[ push_back_a( self.m_Pdh->m_FragmentPose ) ] >>
                  real_p[ push_back_a( self.m_Pdh->m_FragmentPose ) ] >>
                  real_p[ push_back_a( self.m_Pdh->m_FragmentPose ) ] >>
                  !ch_p(";") >>
                  real_p[ push_back_a( self.m_Pdh->m_FragmentPose ) ] >>
                  real_p[ push_back_a( self.m_Pdh->m_FragmentPose ) ] >>
                  real_p[ push_back_a( self.m_Pdh->m_FragmentPose ) ] >>
                  real_p[ push_back_a( self.m_Pdh->m_FragmentPose ) ] >>
                  !ch_p(";") >>
                  real_p[ push_back_a( self.m_Pdh->m_FragmentPose ) ] >>
                  real_p[ push_back_a( self.m_Pdh->m_FragmentPose ) ] >>
                  real_p[ push_back_a( self.m_Pdh->m_FragmentPose ) ] >>
                  real_p[ push_back_a( self.m_Pdh->m_FragmentPose ) ]
               ) >>
               str_p("]");

            modelParams = 
               str_p("params") >>
               ch_p('=') >>
               str_p("[") >>
               ( *real_p[ push_back_a( self.m_Pdh->m_DeformableParameters ) ] ) >>
               str_p("]");

            moghariFile = 
               str_p("moghari") >>
               real_p[ assign_a( self.m_Pdh->m_MoghariRadius ) ] >>
               ( confix_p( '"', (+c_escape_ch_p )[ functorAssignString( self.m_Pdh->m_MoghariFilename) ], '"' ) );

            line = 
               as_lower_d[ str_p("deformable") ] >>
                  ( confix_p( '"', (+c_escape_ch_p )[ functorAssignString( self.m_Pdh->m_DeformableFilename) ], '"' ) ) >>
                  !( moghariFile ) >>
                  !( modelParams ) >>
               str_p("end")[ functorEndDeformable( self.m_Pdh ) ] >>
               *(
                  as_lower_d[ str_p( "fragment" ) ][ functorStartFragment( self.m_Pdh ) ] >>
                     ( confix_p( '"', (+c_escape_ch_p )[ functorAssignString( self.m_Pdh->m_FragmentFilename ) ], '"' ) ) >>
                     !( pose ) >> 
                  str_p( "end" )[ functorEndFragment( self.m_Pdh ) ]
               ) >>
               !str_p("evalonly")[ functorEvalOnly( self.m_Pdh ) ] >>
               *( comment_p("//") | comment_p("/*", "*/") ) >>
               lexeme_d[ *space_p ] /* Trim trailing whitespace */
               ;
         }			

         rule<ScannerT> line, pose, modelParams, moghariFile;

         rule<ScannerT> const& start( ) const { return line; }
      };

      DeformableMeshFittingParseDataHolder* m_Pdh;
   };

   parse_info<> info;
};

template <class T>
AWT::DeformableMeshFittingParser<T>::DeformableMeshFittingParser( )
{
   m_D = new D;
}

template <class T>
AWT::DeformableMeshFittingParser<T>::~DeformableMeshFittingParser( )
{
   delete m_D;
}

template <class T>
typename AWT::DeformableMeshFittingParser<T>::P AWT::DeformableMeshFittingParser<T>::getInstance( )
{
   AUTOGETINSTANCE( AWT::DeformableMeshFittingParser<T>, ( ) );
}

template <class T>
bool AWT::DeformableMeshFittingParser<T>::parse(const std::string& str, DeformableMeshFittingStateMachine<T>* dmfsm )
{
   D::DeformableMeshFittingParseDataHolder* pdh = new D::DeformableMeshFittingParseDataHolder( dmfsm );

   D::DeformableMeshFittingGrammar parser( pdh );
   m_D->info = boost::spirit::parse( str.c_str( ), parser, boost::spirit::space_p );

   delete pdh;

   return m_D->info.full;
};

template <class T>
GETNAMEMACRO( AWT::DeformableMeshFittingParser<T> );

template class AWT::DeformableMeshFittingParser<double>;
template class AWT::DeformableMeshFittingParser<float>;