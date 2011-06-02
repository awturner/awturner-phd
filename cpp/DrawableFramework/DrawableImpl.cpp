
#include "DrawableImpl.h"

#include "DrawableFramework/DrawMaterial.h"


struct AWT::DrawableImpl::D
{
   bool              m_Visible;

   DrawableAssembly* m_Parent;
};

AWT::DrawableImpl::DrawableImpl( )
{
   m_D = new D;
   m_D->m_Visible = true;
   m_D->m_Parent = 0;
}

AWT::DrawableImpl::~DrawableImpl( )
{
   delete m_D;
}

void AWT::DrawableImpl::draw( AWT::DrawContext::P context, const bool transparentPass )
{
   if ( !m_D->m_Visible ) return;

   DrawMaterial::P mat = getMaterial( );

   if ( *mat != 0 )
   {
      if ( mat->isOpaque( ) ^ transparentPass )
      {
         mat->prepare( );
         drawImpl( context );
         mat->unprepare( );
      }
   }
   else
   {
      // Only draw if we aren't transparent
      if ( !transparentPass )
         drawImpl( context );
   }
}

bool AWT::DrawableImpl::isVisible( ) const
{
   return m_D->m_Visible;
}

void AWT::DrawableImpl::setVisible( const bool vis )
{
   this->m_D->m_Visible = vis;
   modified( );
}

void AWT::DrawableImpl::setParent( AWT::DrawableAssembly* p )
{
   m_D->m_Parent = p;
   modified( );
}

AWT::DrawableAssembly* AWT::DrawableImpl::getParent( )
{
   return m_D->m_Parent;
}
