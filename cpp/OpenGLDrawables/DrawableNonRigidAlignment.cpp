#include "DrawableNonRigidAlignment.h"

#include "DrawableControlPoints.h"

#include "Mesh/MeshFunctions.h"

#include "NonRigidAlignment/MeshAlignmentData.h"
#include "NonRigidAlignment/NonRigidAlignment.h"
#include "NonRigidAlignment/NonRigidAlignmentListener.h"

#include "OpenGLKilgardMaterial.h"
#include "OpenGLDrawableCorrespondences.h"
#include "OpenGLDrawableMesh.h"
#include "OpenGLTransformation.h"

#include "OpenGLDrawables/OpenGLKilgardMaterial.h"
#include "Useful/PrintMacros.h"



#include <limits>


template <class T>
struct AWT::DrawableNonRigidAlignment<T>::D : public NonRigidAlignmentListener<T>
{
   virtual void fixedMeshSet( NonRigidAlignment<T>* nra )
   {
      if ( nra != m_Nra ) return;

      DrawMaterial::P mat = OpenGLKilgardMaterial::getInstance( "kilgard::jade" );
      
      OpenGLDrawableMesh<T>::P mesh = OpenGLDrawableMesh<T>::getInstance( nra->getMeshAlignment( )->getFixedPoints( ) );
      mesh->setMaterial( mat );
      m_SourceMesh = mesh;
      
      m_Nra->modified( );
   }

   virtual std::string getClassName( ) const;

   virtual void movingMeshSet( NonRigidAlignment<T>* nra )
   {
      m_Dnra->movingMeshSet( nra );
   }

   virtual void correspondencesFound( NonRigidAlignment<T>* nra )
   {
      m_Dnra->correspondencesFound( nra );
   }

   virtual void rigidParameterUpdate( NonRigidAlignment<T>* nra )
   {
      m_Dnra->rigidParameterUpdate( nra );
   }

   virtual void nonRigidParameterUpdate( NonRigidAlignment<T>* nra )
   {
      m_Dnra->nonRigidParameterUpdate( nra );
   }

   virtual void controlPointsChanged( NonRigidAlignment<T>* nra )
   {
      m_Dnra->controlPointsChanged( nra );
   }

   virtual void alignmentStart( NonRigidAlignment<T>* nra )
   {
      m_Dnra->alignmentStart( nra );
   }

   virtual void alignmentComplete( NonRigidAlignment<T>* nra )
   {
      m_Dnra->alignmentComplete( nra );
   }

   void setControlPoints( typename DrawableControlPoints<T>::P cps )
   {
      m_ControlPoints = cps;
   }

   D( typename DrawableNonRigidAlignment<T>::P dnra )
   {
      m_Dnra = dnra;
   }
   
   typename DrawableNonRigidAlignment<T>::P     m_Dnra;

   NonRigidAlignment<T>*             m_Nra;

   Drawable::P                                  m_SourceMesh;
   Drawable::P                                  m_TargetMesh;

   typename DrawableControlPoints<T>::P         m_ControlPoints;
   typename OpenGLDrawableCorrespondences<T>::P m_Correspondences;

   bool                              m_Visible;
   DrawableAssembly* m_Parent;
};

template <class T>
GETNAMEMACRO( AWT::DrawableNonRigidAlignment<T>::D );

template <class T>
AWT::DrawableNonRigidAlignment<T>::DrawableNonRigidAlignment( typename NonRigidAlignment<T>::P nra )
{
   m_D = new D( this );
   m_D->m_Parent = 0;

   m_D->m_Nra = *nra;

   m_D->m_Visible = true;

   nra->addListener( m_D );

   if ( *nra->getMeshAlignment( )->getFixedPoints( ) != 0 )
      m_D->fixedMeshSet( *nra );

   if ( *nra->getMeshAlignment( )->getMovingPoints( ) != 0 )
      m_D->movingMeshSet( *nra );

   if ( *nra->getControlPoints( ) != 0 )
      m_D->controlPointsChanged( *nra );
}

template <class T>
AWT::DrawableNonRigidAlignment<T>::~DrawableNonRigidAlignment( )
{
   m_D->m_Nra->removeListener( m_D );
   
   delete m_D;
}

template <class T>
GETNAMEMACRO( AWT::DrawableNonRigidAlignment<T> );

template <class T>
typename AWT::DrawableNonRigidAlignment<T>::P AWT::DrawableNonRigidAlignment<T>::getInstance( typename NonRigidAlignment<T>::P nra )
{
   AUTOGETINSTANCE( DrawableNonRigidAlignment<T>, ( nra ) );
}

template <class T>
bool AWT::DrawableNonRigidAlignment<T>::isVisible( ) const
{
   return m_D->m_Visible;
}

template <class T>
void AWT::DrawableNonRigidAlignment<T>::setVisible( const bool v )
{
   if ( v != m_D->m_Visible )
   {
      m_D->m_Visible = v;
      modified( );
   }
}

template <class T>
void AWT::DrawableNonRigidAlignment<T>::getBounds( double* out_Bounds )
{
   if ( *m_D->m_SourceMesh == 0 && *m_D->m_TargetMesh == 0 )
   {
      for ( int i = 0; i < 6; ++i )
         out_Bounds[i] = std::numeric_limits<T>::quiet_NaN( );
   }
   else if ( *m_D->m_SourceMesh != 0 && *m_D->m_TargetMesh == 0 )
   {
      m_D->m_SourceMesh->getBounds( out_Bounds );
   }
   else if ( *m_D->m_SourceMesh == 0 && *m_D->m_TargetMesh != 0 )
   {
      m_D->m_TargetMesh->getBounds( out_Bounds );
   }
   else
   {      
      double sourceBounds[6];
      double targetBounds[6];

      m_D->m_SourceMesh->getBounds( sourceBounds );
      m_D->m_TargetMesh->getBounds( targetBounds );

      for ( int i = 0; i < 3; ++i )
      {
         out_Bounds[2*i+0] = std::min<double>( sourceBounds[2*i+0], targetBounds[2*i+0] );
         out_Bounds[2*i+1] = std::max<double>( sourceBounds[2*i+1], targetBounds[2*i+1] );
      }
   }
}

template <class T>
void AWT::DrawableNonRigidAlignment<T>::draw( AWT::DrawContext::P context, const bool transparentPass )
{
   if ( *m_D->m_SourceMesh != 0 )
      m_D->m_SourceMesh->draw( context, transparentPass );
   
   if ( *m_D->m_TargetMesh != 0 )
      m_D->m_TargetMesh->draw( context, transparentPass );
   
   if ( *m_D->m_ControlPoints != 0 )
      m_D->m_ControlPoints->draw( context, transparentPass );

   if ( *m_D->m_Correspondences != 0 )
      m_D->m_Correspondences->draw( context, transparentPass );
}

template <class T>
void AWT::DrawableNonRigidAlignment<T>::fixedMeshSet( typename NonRigidAlignment<T>::P nra )
{
   if ( *nra != m_D->m_Nra ) return;

   DrawMaterial::P mat = OpenGLKilgardMaterial::getInstance( "kilgard::jade" );
   
   OpenGLDrawableMesh<T>::P mesh = OpenGLDrawableMesh<T>::getInstance( nra->getMeshAlignment( )->getFixedPoints( ) );
   mesh->setMaterial( mat );
   m_D->m_SourceMesh = mesh;
   
   modified( );
}

template <class T>
void AWT::DrawableNonRigidAlignment<T>::movingMeshSet( typename NonRigidAlignment<T>::P nra )
{
   if ( *nra != m_D->m_Nra ) return;

   Mesh<T>::P m = nra->getMeshAlignment( )->getMovingPoints( );

   DrawMaterial::P mat = OpenGLKilgardMaterial::getInstance( "kilgard::ruby" );
   
   OpenGLDrawableMesh<T>::P mesh = OpenGLDrawableMesh<T>::getInstance( m );
   mesh->setMaterial( mat );
   m_D->m_TargetMesh = mesh;

   modified( );
}

template <class T>
void AWT::DrawableNonRigidAlignment<T>::controlPointsChanged( typename NonRigidAlignment<T>::P nra )
{
   if ( *nra != m_D->m_Nra ) return;

   DrawableControlPoints<T>::P dcps = DrawableControlPoints<T>::getInstance( nra->getControlPoints( ) );
   m_D->m_ControlPoints = dcps;
   
   modified( );
}

template <class T>
void AWT::DrawableNonRigidAlignment<T>::correspondencesFound( typename NonRigidAlignment<T>::P nra )
{
   if ( *nra != m_D->m_Nra ) return;

   CorrespondenceFinder<T>::P corr = nra->getCorrespondences( );
   
   if ( *m_D->m_Correspondences == 0 )
   {
      m_D->m_Correspondences = OpenGLDrawableCorrespondences<T>::getInstance( corr );
   }
   else
   {
      m_D->m_Correspondences->setCorrespondences( corr );
   }

   modified( );
}

template <class T>
void AWT::DrawableNonRigidAlignment<T>::rigidParameterUpdate( typename NonRigidAlignment<T>::P nra )
{
   if ( *nra != m_D->m_Nra ) return;

   modified( );
}

template <class T>
void AWT::DrawableNonRigidAlignment<T>::nonRigidParameterUpdate( typename NonRigidAlignment<T>::P nra )
{
   if ( *nra != m_D->m_Nra ) return;

   modified( );
}

template <class T>
void AWT::DrawableNonRigidAlignment<T>::alignmentStart( typename NonRigidAlignment<T>::P nra )
{
   if ( *nra != m_D->m_Nra ) return;

   modified( );
}

template <class T>
void AWT::DrawableNonRigidAlignment<T>::alignmentComplete( typename NonRigidAlignment<T>::P nra )
{
   if ( *nra != m_D->m_Nra ) return;

   modified( );
}

template <class T>
void AWT::DrawableNonRigidAlignment<T>::setParent( DrawableAssembly* assm )
{
   m_D->m_Parent = assm;
   modified( );
}

template <class T>
AWT::DrawableAssembly* AWT::DrawableNonRigidAlignment<T>::getParent( )
{
   return m_D->m_Parent;
}


template class AWT::DrawableNonRigidAlignment<double>;
template class AWT::DrawableNonRigidAlignment<float>;