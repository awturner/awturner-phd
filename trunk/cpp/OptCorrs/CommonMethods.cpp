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
#include "CommonMethods.h"

#if WITH_MATLAB
#include "Useful/MatlabEngine.h"
#endif

#include "AlignParametric/TypeDefs.h"
#include "AlignParametric/NoneCostFunction.h"
#include "AlignParametric/DeterminantOfCovariance.h"
#include "AlignParametric/MinimumDescriptionLength.h"

using namespace AWT;
using namespace AWT::AlignParametric;

///////////////////////////////////////////////

ModelEigenvalueOptimizer::P    AWT::AlignParametric::eigOpt;
std::vector<SampledSurface::P> AWT::AlignParametric::sampledSurfaces;

void copyTuplesToValue( TuplesType::P tuples, XmlRpcValue& result )
{
   result.setSize( tuples->getNumberOfPoints() );

   const Idx M = tuples->getTupleSize();
   const Idx N = tuples->getNumberOfPoints();
   T* vals = new T[M];

   for ( Idx v = 0; v < N; ++v )
   {
      tuples->getPoint(v, vals);

      for ( Idx i = 0; i < M; ++i )
         result[v][i] = vals[i];
   }

   delete [] vals;
}

void remoteMethod( AWT::AlignParametric::CommonMethods::setCostFunction )
{
   const std::string type = params[0];

   if ( type == "detcov" )
   {
      DeterminantOfCovariance::P detcov = DeterminantOfCovariance::getInstance( );
      detcov->setEpsilon( params[1] );
      eigOpt->setCostFunction( detcov );
   }
   else if ( type == "mdl" )
   {
      MinimumDescriptionLength::P mdl = MinimumDescriptionLength::getInstance( );
      mdl->setLambdaCut( params[1] );
      eigOpt->setCostFunction( mdl );
   }
   else if ( type == "none" )
   {
      eigOpt->setCostFunction( NoneCostFunction::getInstance() );
   }
   else
   {
      throw XmlRpcException( "Unknown cost function type" );
   }
}

void remoteMethod( AWT::AlignParametric::CommonMethods::getScaleNormalization )
{
   switch ( eigOpt->getScaleNormalization() )
   {
   case ModelEigenvalueOptimizer::NRML_HYPERSPHERE:
      result = "hypersphere";
      break;
   case ModelEigenvalueOptimizer::NRML_HYPERPLANE:
      result = "hyperplane";
      break;
   default:
      result = "none";
   }
}

void remoteMethod( AWT::AlignParametric::CommonMethods::setScaleNormalization )
{
   const std::string scaleNorm = params[0];

   if ( scaleNorm == "none" )
   {
      eigOpt->setScaleNormalization( ModelEigenvalueOptimizer::NRML_NONE );
      DEBUGMACRO( "Setting scale normalization to NRML_NONE" );
   }
   else if ( scaleNorm == "hypersphere" )
   {
      eigOpt->setScaleNormalization( ModelEigenvalueOptimizer::NRML_HYPERSPHERE );
      DEBUGMACRO( "Setting scale normalization to NRML_HYPERSPHERE" );
   }
   else if ( scaleNorm == "hyperplane" )
   {
      eigOpt->setScaleNormalization( ModelEigenvalueOptimizer::NRML_HYPERPLANE );
      DEBUGMACRO( "Setting scale normalization to NRML_HYPERPLANE" );
   }
}

void remoteMethod( AWT::AlignParametric::CommonMethods::getMeanSamples )
{
   copyTuplesToValue( eigOpt->getMeanSamples(), result );
}

void remoteMethod( AWT::AlignParametric::CommonMethods::getSamples )
{
   copyTuplesToValue( eigOpt->getSamples( (int)params[0] ), result );
}

void remoteMethod( AWT::AlignParametric::CommonMethods::getParameters )
{
   MatrixType controls;
   SampledSurface::P surf = eigOpt->getSampledSurface( (int)params[0] );
   surf->getParameters( controls );

   result.setSize( controls.rows() );
   for ( Idx r = 0; r < controls.rows(); ++r )
   {
      XmlRpcValue& row = result[r];
      row.setSize( controls.cols() );

      for ( Idx c = 0; c < controls.cols(); ++c )
         row[c] = controls(r,c);
   }
}

void remoteMethod( AWT::AlignParametric::CommonMethods::getNumberOfSamplesPerSurface )
{
   result = (int)eigOpt->getNumberOfSamplesPerSurface();
}

void remoteMethod( AWT::AlignParametric::CommonMethods::getNumberOfSampledSurfaces )
{
   result = (int)eigOpt->getNumberOfSampledSurfaces();
}

void remoteMethod( AWT::AlignParametric::CommonMethods::calculateNow )
{
   DEBUGMACRO( "Calculating now..." );

   result = eigOpt->calculateNow();

   DEBUGMACRO( "Done.  Result = " << (double)result );
}

void remoteMethod( AWT::AlignParametric::CommonMethods::calculateMeasures )
{
   DEBUGMACRO( "Calculating measures..." );

   eigOpt->calculateMeasures();

   DEBUGMACRO( "Done." );
}

void remoteMethod( AWT::AlignParametric::CommonMethods::saveModel )
{
   const std::string filename = params[0];

   eigOpt->saveModel( filename );
   //PAUSE;
}

void remoteMethod( AWT::AlignParametric::CommonMethods::saveMatlab )
{
   const std::string filename = params[0];

   eigOpt->saveMatlab( filename );
   //PAUSE;
}

void remoteMethod( AWT::AlignParametric::CommonMethods::calculateGradients )
{
   DEBUGMACRO( "Calculating gradients..." );

   result = eigOpt->calculateGradients();

   DEBUGMACRO( "Done.  Result = " << (double)result );
}



void remoteMethod( AWT::AlignParametric::CommonMethods::applyUpdates )
{
   DEBUGMACRO( "Applying updates" );

   eigOpt->applyUpdates( params[0] );

   DEBUGMACRO( "Done." );
}

void remoteMethod( AWT::AlignParametric::CommonMethods::getEigenvalues )
{
   copyVectorIntoXmlRpcValue<double>( eigOpt->getEigenvalues(), result );
}

//////////

void remoteMethod( AWT::AlignParametric::CommonMethods::getSampleConvergenceThreshold )
{
   result = eigOpt->getSampleConvergenceThreshold( );
}

void remoteMethod( AWT::AlignParametric::CommonMethods::setSampleConvergenceThreshold )
{
   const double v = params[0];

   eigOpt->setSampleConvergenceThreshold( v );

   DEBUGMACRO( "Sample Convergence Threshold set to " << v );
}

//////////

void remoteMethod( AWT::AlignParametric::CommonMethods::getModelWeight )
{
   result = eigOpt->getModelWeight( );
}

void remoteMethod( AWT::AlignParametric::CommonMethods::setModelWeight )
{
   const double v = params[0];

   eigOpt->setModelWeight( v );

   DEBUGMACRO( "Model weight set to " << v );
}

//////////

void remoteMethod( AWT::AlignParametric::CommonMethods::getRegularizationWeight )
{
   result = eigOpt->getRegularizationWeight( );
}

void remoteMethod( AWT::AlignParametric::CommonMethods::setRegularizationWeight )
{
   const double v = params[0];

   eigOpt->setRegularizationWeight( v );

   DEBUGMACRO( "Regularization weight set to " << v );
}

//////////

///////////////////////////////////////////////

void remoteMethod( AWT::AlignParametric::CommonMethods::setFirstShapeFixed )
{
   const bool v = params[0];

   eigOpt->setFirstShapeFixed( v );

   DEBUGMACRO( "First shape fixed set to " << (v?"True":"False") );
   result = eigOpt->isFirstShapeFixed();
}

void remoteMethod( AWT::AlignParametric::CommonMethods::isFirstShapeFixed )
{
   result = eigOpt->isFirstShapeFixed( );
}

void remoteMethod( AWT::AlignParametric::CommonMethods::setPermittedAlignments )
{
   const bool r = params[0];
   const bool t = params[1];

   eigOpt->setRotationAllowed( r );
   eigOpt->setTranslationAllowed( t );
}

void remoteMethod( AWT::AlignParametric::CommonMethods::pause )
{
   PAUSE;
}

void remoteMethod( AWT::AlignParametric::CommonMethods::getPermittedAlignments )
{
   result[0] = eigOpt->isRotationAllowed( );
   result[1] = eigOpt->isTranslationAllowed( );
}

void AWT::AlignParametric::CommonMethods::reset( )
{
   eigOpt.set( 0 );
   sampledSurfaces.clear( );
}

#if WITH_MATLAB
void remoteMethod( AWT::AlignParametric::CommonMethods::showInMatlab )
{
   MatlabEngine::P eng = MatlabEngine::getInstance( );

   const std::string initFilename = params[0];
   const std::string currFilename = params[1];
   const std::string cmd = "visualizeModel( '" + initFilename + "', '" + currFilename + "' );";

   eng->eval( cmd.c_str() );
}
#endif


void AWT::AlignParametric::CommonMethods::addMethods( AWT::RemoteMethods& methods )
{
   methods.push_back( RemoteMethod( "getScaleNormalization", AWT::AlignParametric::CommonMethods::getScaleNormalization ) );
   methods.push_back( RemoteMethod( "setScaleNormalization", AWT::AlignParametric::CommonMethods::setScaleNormalization ) );
   methods.push_back( RemoteMethod( "setCostFunction", AWT::AlignParametric::CommonMethods::setCostFunction ) );
   methods.push_back( RemoteMethod( "setPermittedAlignments", AWT::AlignParametric::CommonMethods::setPermittedAlignments ) );
   methods.push_back( RemoteMethod( "getPermittedAlignments", AWT::AlignParametric::CommonMethods::getPermittedAlignments ) );
   methods.push_back( RemoteMethod( "getMeanSamples", AWT::AlignParametric::CommonMethods::getMeanSamples ) );
   methods.push_back( RemoteMethod( "getSamples", AWT::AlignParametric::CommonMethods::getSamples ) );
   methods.push_back( RemoteMethod( "getParameters", AWT::AlignParametric::CommonMethods::getParameters ) );
   methods.push_back( RemoteMethod( "getNumberOfSamplesPerSurface", AWT::AlignParametric::CommonMethods::getNumberOfSamplesPerSurface ) );
   methods.push_back( RemoteMethod( "getNumberOfSampledSurfaces", AWT::AlignParametric::CommonMethods::getNumberOfSampledSurfaces ) );
   methods.push_back( RemoteMethod( "calculateNow", AWT::AlignParametric::CommonMethods::calculateNow ) );
   methods.push_back( RemoteMethod( "calculateMeasures", AWT::AlignParametric::CommonMethods::calculateMeasures ) );
   methods.push_back( RemoteMethod( "saveModel", AWT::AlignParametric::CommonMethods::saveModel ) );
   methods.push_back( RemoteMethod( "saveMatlab", AWT::AlignParametric::CommonMethods::saveMatlab ) );
   methods.push_back( RemoteMethod( "calculateGradients", AWT::AlignParametric::CommonMethods::calculateGradients ) );
   methods.push_back( RemoteMethod( "applyUpdates", AWT::AlignParametric::CommonMethods::applyUpdates ) );
   methods.push_back( RemoteMethod( "getEigenvalues", AWT::AlignParametric::CommonMethods::getEigenvalues ) );
   methods.push_back( RemoteMethod( "getSampleConvergenceThreshold", AWT::AlignParametric::CommonMethods::getSampleConvergenceThreshold ) );
   methods.push_back( RemoteMethod( "setSampleConvergenceThreshold", AWT::AlignParametric::CommonMethods::setSampleConvergenceThreshold ) );
   methods.push_back( RemoteMethod( "getModelWeight", AWT::AlignParametric::CommonMethods::getModelWeight ) );
   methods.push_back( RemoteMethod( "setModelWeight", AWT::AlignParametric::CommonMethods::setModelWeight ) );
   methods.push_back( RemoteMethod( "getRegularizationWeight", AWT::AlignParametric::CommonMethods::getRegularizationWeight ) );
   methods.push_back( RemoteMethod( "setRegularizationWeight", AWT::AlignParametric::CommonMethods::setRegularizationWeight ) );
   methods.push_back( RemoteMethod( "setFirstShapeFixed", AWT::AlignParametric::CommonMethods::setFirstShapeFixed ) );
   methods.push_back( RemoteMethod( "isFirstShapeFixed", AWT::AlignParametric::CommonMethods::isFirstShapeFixed ) );

#if WITH_MATLAB
   methods.push_back( RemoteMethod( "showInMatlab", AWT::AlignParametric::CommonMethods::showInMatlab ) );
#endif

   methods.push_back( RemoteMethod( "pause", AWT::AlignParametric::CommonMethods::pause ) );
}

void AWT::AlignParametric::addMethodsToServer( XmlRpcServer& s, AWT::RemoteMethods& methods )
{
   for ( RemoteMethods::iterator it = methods.begin(), en = methods.end(); it != en; ++it )
   {
      std::cerr << "Adding method " << it->name() << std::endl;
      s.addMethod( &(*it) );
   }
}

