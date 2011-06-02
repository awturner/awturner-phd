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
#include "Aligner.h"

#include "Connectivity.h"
#include "MeshIO.h"
#include "MeshFunctions.h"
#include "UniformSampler.h"
#include "Sampler.h"
#include "Search.h"
#include "AllSearchFilters.h"
#include "Useful/Noise.h"

#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_matlab_filewrite.h"
#include <algorithm>

using namespace AWT::SimpleMesh;

struct AWT::SimpleMesh::Aligner::D
{
   typedef std::vector<FragmentObject::P> FragmentObjects;

   FragmentObjects fragObjs;

   ModelObject::P modelObj;
   VisitedFaceSearchFilter::P visitedFacesFilter;

   Correspondences corrs;
   
   double lambda;

   double sigmaNoise;

   CostFunctionType cft;

   LameckerMeasures lameckerMeasures;

   double trim;
      
   Correspondence findModelCorrespondence( const PointIndexWeights& piw )
   {
      double bestDistance2 = FAR_FAR_AWAY;
      Correspondence bestCorr;
      bestCorr.modelPoint = piw;
      
      bestCorr.whichFragment = INVALID_INDEX;

      // Calculate the normal at this face, and record it for later
      Point nml = calculateFaceNormal( modelObj->getMesh( ), piw.i );
      bestCorr.modelNormal = nml;

      // Create a normal search filter
      NormalSearchFilter::P nmlFilt = NormalSearchFilter::getInstance( );
      
      // Now, go through all of the fragments and find the closest point
      for ( Index i = 0, imax = static_cast<Index>( fragObjs.size() ); i < imax; ++i )
      {
         FragmentObject::P fragObj = fragObjs[i];
         
         Transformation& trans = fragObj->getTransformation();
         Transformation& inv   = fragObj->getInverseTransformation();

         // Set the normal filter test point/normal
         nmlFilt->setTestPoint( inv * piw.p );
         nmlFilt->setTestNormal( inv * nml );

         // Get the fragment edge search filter
         MultiSearchFilter::P filt = MultiSearchFilter::getInstance( 2 );
         filt->addFilter( nmlFilt );

         PointIndexWeights piwFound = fragObj->search( inv*piw.p, inv*nml, filt );
         const double thisDistance2 = ( trans * piwFound.p - piw.p ).squared_magnitude( );

         if ( thisDistance2 < bestDistance2 )
         {
            // Do the edge filtering here
            SearchFilter::P edgeFilter = fragObj->getEdgeFilter();

            // Only accept if not on the edge
            if ( edgeFilter->check( piwFound.p, piwFound.i ) )
            {
               // Record in the fragment coordinate frame
               bestDistance2 = thisDistance2;
               bestCorr.fragPoint = piwFound;
               bestCorr.whichFragment = i;

               bestCorr.lengthSquared = thisDistance2;
            }
         }
      }

      return bestCorr;
   }

   // For a sample on a fragment, find the closest correspondence on the model
   Correspondence findFragmentCorrespondence( const Index fragi, const PointIndexWeights& piw )
   {
      Point testPoint = fragObjs[fragi]->getTransformation() * piw.p;
      Point testNormal;

      // Construct a chain of filters
      MultiSearchFilter::P filt = MultiSearchFilter::getInstance( 2 );

      // Filter based on already marked faces
      filt->addFilter( visitedFacesFilter );

      // Do normal compatibility from model, not the fragment itself
      OtherNormalSearchFilter::P othNormal = OtherNormalSearchFilter::getInstance( modelObj->getMesh( ) );
      othNormal->setTestPoint( testPoint );
      filt->addFilter( othNormal );
      
      Correspondence ret;
      ret.whichFragment = fragi;
      ret.fragPoint = piw;
      ret.modelPoint = modelObj->search( testPoint, testNormal, filt );

      // Make sure that we did find a point
      if ( ret.valid() )
      {
         // Record the normal for later
         Point nml = calculateFaceNormal( modelObj->getMesh( ), ret.modelPoint.i );
         ret.modelNormal = nml;

         // Calculate the length
         ret.lengthSquared = ( ret.modelPoint.p - fragObjs[fragi]->getTransformation() * piw.p ).squared_magnitude();
      }

      return ret;
   }

   Index findCorrespondences( Correspondences& corrs )
   {
      corrs.clear();

      // Get the fragment correspondences for all model samples
      PIWs& modelSamples = modelObj->getSamples();
      for ( PIWs::iterator it = modelSamples.begin(), en = modelSamples.end(); it != en; ++it )
      {
         Correspondence corr = findModelCorrespondence( *it );

         if ( corr.valid( ) )
            corrs.push_back( corr );
      }

      // Clear all previously visited faces
      visitedFacesFilter->reset( );

      // For each fragment, get the model correspondence for each sample
      for ( Index i = 0; i < fragObjs.size(); ++i )
      {
         for ( PIWs::iterator it = fragObjs[i]->getSamples().begin(), en = fragObjs[i]->getSamples().end(); it != en; ++it )
         {
            Correspondence corr = findFragmentCorrespondence( i, *it );

            if ( corr.valid( ) )
               corrs.push_back( corr );
         }
         
         // Mark faces so that they can't be used for subsequent fragments
         visitedFacesFilter->markFaces( );

         //visitedFacesFilter->writeUnvisitedMesh( "unvisited.smesh" );
         //PAUSE;
      }

      // Chuck away the longest quartile - apparently, this is called Trimmed ICP
      sort( corrs.begin(), corrs.end() );
      const size_t ntrim = static_cast<size_t>( ceil(trim * corrs.size()) );
      corrs.erase( corrs.begin() + ntrim, corrs.end() );

      return corrs.size();
   }

   void findModelVertexCorrespondences( Correspondences& corrs )
   {
      // For each model vertex, find the closest point on any fragment
      corrs.clear( );

      Mesh::P weightedModel = modelObj->getModel( )->getMesh( modelObj->getParameters( ) );
      Connectivity::P conn = Connectivity::getInstance( weightedModel );

      for ( Index v = 0; v < weightedModel->nv; ++v )
      {
         Point p = weightedModel->getVertex( v );
         Index f = conn->aFaceUsingVertex( v );

         ConvexWeights ws = calculateFaceWeights( weightedModel, f, p );

         PointIndexWeights piw;
         piw.p = p;
         piw.i = f;
         piw.w = ws;

         Correspondence corr = findModelCorrespondence( piw );

         corrs.push_back( corr );
      }
   }

   void findFragmentVertexCorrespondences( const Index fragi, Correspondences& corrs )
   {
      // For each model vertex, find the closest point on any fragment
      corrs.clear( );

      Mesh::P model = fragObjs[fragi]->getMesh(  );
      Connectivity::P conn = Connectivity::getInstance( model );

      for ( Index v = 0; v < model->nv; ++v )
      {
         Point p = model->getVertex( v );
         Index f = conn->aFaceUsingVertex( v );

         ConvexWeights ws = calculateFaceWeights( model, f, p );

         PointIndexWeights piw;
         piw.p = p;
         piw.i = f;
         piw.w = ws;

         visitedFacesFilter->reset( );
         Correspondence corr = findFragmentCorrespondence( fragi, piw );

         corrs.push_back( corr );
      }
   }

   LameckerMeasures calculateCostFunction( const Correspondences& corrs, const double thresh = FAR_FAR_AWAY )
   {
      LameckerMeasures ret;
      ret.mean = ret.rms = ret.max = ret.aod = 0;

      std::vector<double> corrLengths;

      // Go through all the correspondences and work out where the points are now
      Mesh::P modelMesh = modelObj->getMesh( );
      for ( Correspondences::const_iterator it = corrs.begin(), en = corrs.end(); it != en; ++it )
      {
         const Correspondence& corr = *it;
         FragmentObject::P fragObj = fragObjs[corr.whichFragment];

         // Where is the model point?
         const Point modelPointPos = calculatePoint( modelMesh, corr.modelPoint.i, corr.modelPoint.w );

         // Where is the fragment point?
         const Point fragPointPos = fragObj->getTransformation() * corr.fragPoint.p;

         // This is the residual between the points
         const Point r = fragPointPos - modelPointPos;

         double thisCorr;

         switch ( cft )
         {
         case CF_POINTTOPOINT:
            thisCorr = r.squared_magnitude();
            break;

         case CF_POINTTOPLANE:
            // Need to dot the residual with the normal to the model
            double dott = inner_product<double>( corr.modelNormal, r );
            thisCorr = dott*dott;

            break;
         }

         const double sq = sqrt( thisCorr );

         ret.rms  += thisCorr;
         ret.mean += sq;
         ret.max   = std::max<double>( ret.max, thisCorr );
         ret.aod  += ( sq > thresh ) ? 1 : 0;
      }

      // Correct the working values
      ret.mean = ret.mean / corrs.size();
      ret.rms  = sqrt( ret.rms / corrs.size() );
      ret.max  = sqrt(ret.max);
      ret.aod  = ret.aod / corrs.size();

      const Vector params = modelObj->getParameters();
      ret.modelScale = ( params.size() > 0 ) ? params[0] : 1;

      return ret;
   }

   void writeModels( const std::string& saveFormat )
   {
      char buffer[256];
      char buffer2[256];
      Point modelPointPos, fragPointPos;

      {
         sprintf_s( buffer, saveFormat.c_str(), "corrs.mat" );
         vnl_matlab_filewrite mfw( buffer );
            
         // Write out the transformed model and mesh
         Mesh::P weightedModel = modelObj->getModel( )->getMesh( modelObj->getParameters( ) );
         sprintf_s( buffer, saveFormat.c_str(), "fittedmodel.smesh" );
         MeshIO::saveMesh( std::ofstream( buffer ), weightedModel );

         for ( Index i = 0; i < fragObjs.size( ); ++i )
         {
            Mesh::P fragMesh = fragObjs[i]->getMesh( );

            Points tverts = fragMesh->getVertices();
            affineTransform( tverts, fragObjs[i]->getTransformation( ) );

            Mesh::P tmesh = Mesh::getInstance( fragMesh->nv, fragMesh->nf );
            tmesh->getVertices().update( tverts );
            tmesh->getFaces().update( fragMesh->getFaces() );

            sprintf_s( buffer2, "tfrag%d.smesh", i );
            sprintf_s( buffer, saveFormat.c_str(), buffer2 );
            MeshIO::saveMesh( std::ofstream( buffer ), tmesh );
         }

         const Index ncorrs = static_cast<Index>( corrs.size() );
         Matrix modelCorrs( 4, ncorrs );
         Matrix tfragCorrs( 4, ncorrs );

         Index m = 0;

         for ( Correspondences::const_iterator it = corrs.begin(), en = corrs.end(); it != en; ++it )
         {
            const Correspondence& corr = *it;
            calculateCorrespondencePoints( corr, weightedModel, modelPointPos, fragPointPos );

            modelCorrs.set_column( m, modelPointPos );
            tfragCorrs.set_column( m, fragPointPos );

            ++m;
         }
         mfw.write( modelCorrs, "corrs_model" );
         mfw.write( tfragCorrs, "corrs_tfrag" );

         {
            Matrix modelDistances( 1, modelObj->getMesh()->nv );
            Correspondences modelVertCorrs;
            findModelVertexCorrespondences( modelVertCorrs );

            m = 0;
            for ( Correspondences::const_iterator it = modelVertCorrs.begin(), en = modelVertCorrs.end(); it != en; ++it )
            {
               const Correspondence& corr = *it;
               calculateCorrespondencePoints( corr, weightedModel, modelPointPos, fragPointPos );

               const double r = ( modelPointPos - fragPointPos ).magnitude( );

               modelDistances( 0, m ) = r;
               ++m;
            }
            mfw.write( modelDistances, "vdist_model" );
         }

         for ( Index i = 0; i < fragObjs.size(); ++i )
         {
            Matrix modelDistances( 1, fragObjs[i]->getMesh()->nv );
            Correspondences modelVertCorrs;
            findFragmentVertexCorrespondences( i, modelVertCorrs );

            m = 0;
            for ( Correspondences::const_iterator it = modelVertCorrs.begin(), en = modelVertCorrs.end(); it != en; ++it )
            {
               const Correspondence& corr = *it;
               calculateCorrespondencePoints( corr, weightedModel, modelPointPos, fragPointPos );

               const double r = ( modelPointPos - fragPointPos ).magnitude( );

               modelDistances( 0, m ) = r;
               ++m;
            }
            sprintf_s( buffer, "vdist_frag%d", i );
            mfw.write( modelDistances, buffer );
         }
      }

      DEBUGMACRO( "Models written to file." );
   }

   void calculateCorrespondencePoints( const Correspondence& corr, Mesh::P weightedModel, Point& modelPointPos, Point& fragPointPos )
   {
      if ( corr.valid() )
      {
         FragmentObject::P fragObj = fragObjs[corr.whichFragment];

         // Where is the model point?
         modelPointPos = calculatePoint( weightedModel, corr.modelPoint.i, corr.modelPoint.w );

         // Where is the fragment point?
         fragPointPos = fragObj->getTransformation() * corr.fragPoint.p;
      }
      else
      {
         modelPointPos.fill( INVALID_COORDINATE );
         fragPointPos.fill( INVALID_COORDINATE );
      }
   }

   Point perturbation( )
   {
      Point ret;

      if ( sigmaNoise == 0 )
      {
         ret.fill(0);
      }
      else
      {
         ret[0] = Noise<double>::randn( 0, sigmaNoise );
         ret[1] = Noise<double>::randn( 0, sigmaNoise );
         ret[2] = Noise<double>::randn( 0, sigmaNoise );
         ret[3] = 0;
      }

      return ret;
   }

   double stepUpdate( )
   {
      PRINTVBL( cft );
      //bool converged;

      std::cerr << "==============================================================" << std::endl;

      //findCorrespondences( corrs );

      // Record the cost function before-hand, so we can see if the LM update is an improvement
      const LameckerMeasures lameckerBefore = calculateCostFunction( corrs, FAR_FAR_AWAY );

      // Now, go through all of the correspondences and form the Jacobian appropriately
      const Index nfrags  = static_cast<Index>( fragObjs.size() );
      const Index nmodes  = modelObj->getModel()->getNumberOfModes();
      const Index nparams = nmodes + 7*nfrags;

      Matrix modelBlock( 4, nmodes );
      Matrix fragBlock( 4, 7 );

      // These are the matrices which will be used to solve
      Matrix JtJ( nparams, nparams );
      Vector Jtr( nparams );
      Matrix J;

      // J is different size, depending on cost function type
      switch ( cft )
      {
      case CF_POINTTOPOINT:
         J.set_size( 4, nparams );
         break;
      case CF_POINTTOPLANE:
         J.set_size( 1, nparams );
         break;
      }

      Index* countCorrespondences = new Index[ nfrags ];
      for ( Index i = 0; i < nfrags; ++i )
         countCorrespondences[i] = 0;

      JtJ.fill( 0 );
      Jtr.fill( 0 );

      // Calculate the un-damped matrix
      for ( Correspondences::iterator it = corrs.begin(), en = corrs.end(); it != en; ++it )
      {
         const Correspondence& corr = *it;
         FragmentObject::P fragObj = fragObjs[corr.whichFragment];

         // There's a correspondence on this fragment, so
         ++countCorrespondences[corr.whichFragment];

         // This is the transformed point on the fragment - need this to calculate the residual
         const Point tfragpoint = fragObj->getTransformation() * corr.fragPoint.p;

         //const Point r = tfragpoint - corr.modelPoint.p;

         // Difference between the two points, plus a gaussian perturbation
         const Point r = tfragpoint - corr.modelPoint.p + perturbation( );

         // Calculate the two parts of the Jacobian
         // These Jacobians are variation in the point's position
         fragObj->calculateJacobian( corr.fragPoint, fragBlock );
         modelObj->calculateJacobian( corr.modelPoint, modelBlock );

         // Put the Jacobians with the correct sign in the correct block
         J.fill( 0 );

         switch ( cft )
         {
         case CF_POINTTOPOINT:
            J.set_columns( 0,                             -modelBlock );
            J.set_columns( nmodes + 7*corr.whichFragment,  fragBlock  );

            Jtr += J.transpose( ) * r;

            break;
         case CF_POINTTOPLANE:
            // Pre-multiply the Jacobian by the transposed normal
            Matrix nmlT( 1, 4 );
            nmlT.set_row( 0, corr.modelNormal );

            J.set_columns( 0,                             -nmlT*modelBlock );
            J.set_columns( nmodes + 7*corr.whichFragment,  nmlT*fragBlock  );

            Jtr += ( J.transpose( ) * inner_product<double>( corr.modelNormal, r ) ).get_column( 0 );

            break;
         }

         JtJ += J.transpose( ) * J;
      }

      PRINTVBL( nfrags );
      PRINTVBL( corrs.size() );
      // Print some info about the number of correspondences on each fragment
      DEBUGMACRO( "Number of correspondences on each fragment:" );
      for ( Index i = 0; i < nfrags; ++i )
         DEBUGMACRO( "\t" << "Fragment " << (1*i) << ": " << countCorrespondences[i] );

      // Get rid of this count array, it's no longer needed
      delete [] countCorrespondences;

      // Save the current state, in case we need to roll back
      const Vector modelParams = modelObj->getParameters( );
      std::vector<Vector> fragParams;
      for ( Index i = 0; i < nfrags; ++i )
         fragParams.push_back( fragObjs[i]->getParameters( ) );

      PRINTVBL2( "Model current", modelParams );
      for ( Index i = 0; i < nfrags; ++i )
         DEBUGMACRO( "Frag " << i << " current: " << fragParams[i] );

      // This is a damping term, which will be added with a given weight to the JtJ
      // to make the solution non-singular
      DiagMatrix damper( nparams, nparams );

      while ( true )
      {
         PRINTVBL( lambda );
         // Now then, it should just be a case of solving this bad boy.
         // This should take very little time, since there aren't really very many parameters
         //    -> (nmodes + 7*nfrags)
         damper.fill_diagonal( lambda );
         Vector delta = vnl_svd<double>( JtJ + damper ).pinverse( ) * -Jtr;

         // Now, need to apply this update
         modelObj->setParameters( modelParams + delta.extract( nmodes, 0 ) );
         
         for ( Index i = 0; i < nfrags; ++i )
            fragObjs[i]->setParameters( fragParams[i] + delta.extract( 7, nmodes + 7*i ) );

         // What is the value of the cost function now?   
         const LameckerMeasures lameckerAfter = calculateCostFunction( corrs );
         PRINTVBL( lameckerAfter.rms );

         if ( lameckerAfter.rms < lameckerBefore.rms )
         {
            // Cost function has improved - yay!
            // Decrease damping to make it more Gauss-Newton-like for next iteration and finish
            if ( lambda > 1e-8 )
               lambda /= 10;
            
            lameckerMeasures = lameckerAfter;

            //converged = false;

            break;
         }
         else
         {
            // Cost function has got worse - boo!
            // Increase damping to make it more like gradient descent, and re-evaluate
            lambda *= 10;

            if ( lambda > 1e8 || lameckerAfter.rms < 1e-8 )
            {
               //converged = true;
               break;
            }
         }
      }

      //if ( lameckerBefore.rms - lameckerMeasures.rms < 1e-5 )
      //   converged = true;

      BLANKLINE;
      PRINTVBL( lameckerBefore.rms );
      PRINTVBL( lameckerMeasures.rms );
      PRINTVBL( lameckerBefore.rms-lameckerMeasures.rms );
      BLANKLINE;
      PRINTVBL( modelObj->getParameters( ) );
      for ( Index i = 0; i < nfrags; ++i )
      {
         PRINTVBLNL( fragObjs[i]->getParameters( ) );
         PRINTVBLNL( fragObjs[i]->getTransformation( ) );
      }

      lameckerMeasures = calculateCostFunction( corrs );

      return lameckerBefore.rms - lameckerMeasures.rms;
   }
};

AWT::SimpleMesh::Aligner::Aligner( Aligner::CostFunctionType cft )
{
   m_D = new D;

   m_D->cft               = cft;
   m_D->lambda            = 1e-4;

   m_D->sigmaNoise        = 0;

   m_D->trim = 1;

   m_D->lameckerMeasures.mean = INVALID_COORDINATE;
   m_D->lameckerMeasures.rms = INVALID_COORDINATE;
   m_D->lameckerMeasures.max = INVALID_COORDINATE;
   m_D->lameckerMeasures.modelScale = INVALID_COORDINATE;
}

AWT::SimpleMesh::Aligner::~Aligner( )
{
   delete m_D;
}

AWT::SimpleMesh::Aligner::P AWT::SimpleMesh::Aligner::getInstance( Aligner::CostFunctionType cft )
{
   AUTOGETINSTANCE( AWT::SimpleMesh::Aligner, ( cft ) );
}

GETNAMEMACRO( AWT::SimpleMesh::Aligner );

void AWT::SimpleMesh::Aligner::setModel( ShapeModel::P model, SamplerFactory::P fact, const Index nsamples, Vector params )
{
   // Create the model object and set the parameters
   m_D->modelObj = ModelObject::getInstance( model, fact, nsamples );
   m_D->modelObj->setParameters( params );
   m_D->visitedFacesFilter = VisitedFaceSearchFilter::getInstance( m_D->modelObj->getMesh( ) );
}

void AWT::SimpleMesh::Aligner::addFragment( Mesh::P frag, Sampler::P samp, const Index nsamples, Vector params )
{
   // Create the model object and set the parameters
   FragmentObject::P fragObj = FragmentObject::getInstance( frag, samp, nsamples );
   fragObj->setParameters( params );

   m_D->fragObjs.push_back( fragObj );
}

void AWT::SimpleMesh::Aligner::resample( )
{
   m_D->modelObj->resample( );

   const Index nfrags = static_cast<Index>( m_D->fragObjs.size( ) );
   for ( Index i = 0; i < nfrags; ++i )
      m_D->fragObjs[i]->resample( );
}

double AWT::SimpleMesh::Aligner::stepUpdate( )
{
   return m_D->stepUpdate( );
}

AWT::SimpleMesh::Index AWT::SimpleMesh::Aligner::findCorrespondences( )
{
   const Index ret = m_D->findCorrespondences( m_D->corrs );

   m_D->lameckerMeasures = m_D->calculateCostFunction( m_D->corrs );

   return ret;
}

const Vector AWT::SimpleMesh::Aligner::getModelParameters( ) const
{
   return m_D->modelObj->getParameters( );
}

Index AWT::SimpleMesh::Aligner::getNumberOfFragments( ) const
{
   return static_cast<Index>( m_D->fragObjs.size( ) );
}

const Vector AWT::SimpleMesh::Aligner::getFragmentPose( const Index i ) const
{
   return m_D->fragObjs[i]->getParameters( );
}

const Transformation AWT::SimpleMesh::Aligner::getFragmentTransformation( const Index i ) const
{
   return m_D->fragObjs[i]->getTransformation( );
}

void AWT::SimpleMesh::Aligner::writeModels( const std::string& v )
{
   m_D->writeModels( v );
}

AWT::SimpleMesh::LameckerMeasures AWT::SimpleMesh::Aligner::getLameckerMeasures( )
{
   return m_D->lameckerMeasures;
}

void AWT::SimpleMesh::Aligner::setFragmentNoise( const double v )
{
   if ( v != m_D->sigmaNoise )
   {
      m_D->sigmaNoise = v;
      modified();
   }
}

double AWT::SimpleMesh::Aligner::getFragmentNoise( ) const
{
   return m_D->sigmaNoise;
}

AWT::SimpleMesh::Aligner::CostFunctionType AWT::SimpleMesh::Aligner::getCostFunctionType( ) const
{
   return m_D->cft;
}

void AWT::SimpleMesh::Aligner::setCostFunctionType( const AWT::SimpleMesh::Aligner::CostFunctionType& v )
{
   if ( v != m_D->cft )
   {
      m_D->cft = v;
      modified();
   }
}

void AWT::SimpleMesh::Aligner::setModelParameters(const AWT::SimpleMesh::Vector &vec)
{
   m_D->modelObj->setParameters( vec );
   modified();
}

void AWT::SimpleMesh::Aligner::setFragmentPose(const AWT::SimpleMesh::Index i, const AWT::SimpleMesh::Vector &vec)
{
   m_D->fragObjs[i]->setParameters( vec );
   modified();
}

void AWT::SimpleMesh::Aligner::setNumberOfModes( const AWT::SimpleMesh::Index i )
{
   DEBUGLINE;
   m_D->modelObj->setNumberOfModes( i );
}

void AWT::SimpleMesh::Aligner::activateAllModes( )
{
   DEBUGLINE;
   m_D->modelObj->activateAllModes( );
}

AWT::SimpleMesh::Index AWT::SimpleMesh::Aligner::getNumberOfModes( ) const
{
   DEBUGLINE;
   return m_D->modelObj->getNumberOfModes( );
}

double AWT::SimpleMesh::Aligner::getTrimFactor() const
{
   return m_D->trim;
}

void AWT::SimpleMesh::Aligner::setTrimFactor( const double v )
{
   if ( v != m_D->trim )
   {
      m_D->trim = v;
      modified();
   }
}

const Vector AWT::SimpleMesh::Aligner::getTargetRegistrationError( const Index i, const Index j ) const
{
   // Calculate the difference in pose between the true and actual poses
   const Transformation tTrue = poseToMatrix(getFragmentPose(j));
   const Transformation tCalc = poseToMatrix(getFragmentPose(i));

   Transformation tRel = tCalc * vnl_svd<double>(tTrue).inverse();

   // Subtract the identity so that the difference vector is just tRel * vertex
   for (Index i = 0; i < 4; ++i)
      tRel(i,i) -= 1;

   // Now calculate the difference in vertex positions
   Mesh::P mesh = m_D->fragObjs[i]->getMesh();

   Vector ret(mesh->nv);
   for (Index i = 0; i < mesh->nv; ++i)
   {
      const Vector delta = tRel * mesh->getVertex(i);
      ret[i] = delta.magnitude();
   }

   return ret;
}