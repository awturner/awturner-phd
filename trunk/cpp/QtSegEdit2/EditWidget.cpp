#include "EditWidget.h"

#include "SliceDisplayWidget.h"
#include "MyScrollArea.h"

#include "Useful/PrintMacros.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QSlider>
#include <QHBoxLayout>
#include <QMouseEvent>

#include "SegmentationModel.h"
#include "SegmentationSlice.h"

EditWidget::EditWidget( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
   // Initialize variables
   this->m_Scale = 1;
   this->m_FitToContainer = false;
   this->m_Model = 0;

   // Create the UI
   this->m_ImageLabel = new SliceDisplayWidget;
   this->m_ImageLabel->setBackgroundRole(QPalette::Base);
   this->m_ImageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   this->m_ImageLabel->setScaledContents(true);

   this->m_ScrollArea = new MyScrollArea;
   this->m_ScrollArea->setBackgroundRole(QPalette::Dark);
   this->m_ScrollArea->setWidget(this->m_ImageLabel);

   this->m_Slider = new QSlider( Qt::Vertical );
   this->m_Slider->setMinimum( 0 );
   this->m_Slider->setMaximum( 1 );

   QHBoxLayout* box = new QHBoxLayout( this );
   
   this->m_Layout = box;
   this->m_Layout->addWidget( this->m_ScrollArea );
   this->m_Layout->addWidget( this->m_Slider );

   // Connect signals and slots
   connect( this->m_ScrollArea, SIGNAL( requestSliceChange( int ) ), this, SLOT( attemptSliceChange( int ) ) );
   connect( this->m_Slider, SIGNAL( sliderMoved( int ) ), this, SLOT( attemptSliceSet( int ) ) );
   connect( this->m_Slider, SIGNAL( valueChanged( int ) ), this, SLOT( attemptSliceSet( int ) ) );

   connect( this->m_ImageLabel, SIGNAL( mouseMoved( QMouseEvent* ) ), this, SLOT( mouseMoved( QMouseEvent* ) ) );
}

EditWidget::~EditWidget( )
{
   if ( m_Model != 0 )
      delete m_Model;
}

bool EditWidget::load( const QString& filename )
{
   SegmentationModel* model = new SegmentationModel( filename.toStdString( ).c_str( ) );

   if ( !model->loaded( ) )
   {
      QMessageBox::information(this, tr("Image Viewer"),
         tr("Cannot load %1.").arg(filename));
      return false;
   }

   if ( this->m_Model != 0 )
      delete this->m_Model;

   this->m_Model = model;

   this->attemptSliceSet( model->getDepth( ) / 2 );

   return true;
}

void EditWidget::setScale( const double newscale )
{
   Q_ASSERT( this->m_ImageLabel->pixmap());

   const double oldFactor = this->m_ImageLabel->getScale( );

   this->m_ImageLabel->setScale( newscale );
   this->m_Scale = this->m_ImageLabel->getScale( );
   
   adjustScrollBar(this->m_ScrollArea->horizontalScrollBar(), this->m_Scale / oldFactor);
   adjustScrollBar(this->m_ScrollArea->verticalScrollBar(),   this->m_Scale / oldFactor);
}

void EditWidget::setFitToContainer( const bool state )
{
   this->m_ScrollArea->setWidgetResizable(state);

   if (!state)
      setScale( 1.0 );
}

const double EditWidget::getScale( ) const
{
   return this->m_Scale;
}

const bool   EditWidget::isFitToContainer( ) const
{
   return this->m_FitToContainer;
}

const bool   EditWidget::isShowSegmentation( ) const
{
   return m_ImageLabel->isShowSegmentation( );
}

void   EditWidget::showSegmentation( const bool state )
{
   m_ImageLabel->setShowSegmentation( state );
}

const bool   EditWidget::isLoaded( ) const
{
   return m_Model->loaded( );
}

void EditWidget::mouseMoved( QMouseEvent* ev )
{
   DEBUGMACRO( ev->x() << "\t" << ev->y() );
}

void EditWidget::adjustScrollBar(QScrollBar *scrollBar, const double factor)
{
   scrollBar->setValue(int(factor * scrollBar->value()
      + ((factor - 1) * scrollBar->pageStep()/2)));
}

void EditWidget::attemptSliceChange( int delta )
{
   this->attemptSliceSet( this->m_Slider->value( ) + delta );
}

void EditWidget::attemptSliceSet( int val )
{
   // Check that the slider is set up correctly
   if ( m_Slider->maximum( ) != m_Model->getDepth( ) - 1 )
      m_Slider->setMaximum( m_Model->getDepth( ) - 1 );

   if ( val < 0 )
      val = 0;

   if ( val > m_Slider->maximum( ) )
      val = m_Slider->maximum( );

   SegmentationSlice* slice = new SegmentationSlice( m_Model, val );

   this->m_ImageLabel->setSlice( slice );

   this->m_Slider->setValue( val );
}