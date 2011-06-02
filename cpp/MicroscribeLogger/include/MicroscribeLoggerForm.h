#ifndef __MICROSCRIBELOGGERFORM_H__
#define __MICROSCRIBELOGGERFORM_H__

#include "ui_MicroscribeLoggerForm.h"

class QTimer;
class MicroscribeArm;
class PositionHistory;

class MicroscribeLoggerForm : public QMainWindow, public Ui::MicroscribeLoggerForm
{
   Q_OBJECT;

public:
   MicroscribeLoggerForm( QWidget* in_Parent = 0, Qt::WFlags in_Fl = 0 );
   virtual ~MicroscribeLoggerForm( );

   void setMicroscribeArm( MicroscribeArm* arm );
   MicroscribeArm* microscribeArm( );

protected slots:
   void readArmState( );

   void updatePosition( const double time, const double x, const double y, const double z );

   void btnCalibratePressed( );
   void btnSetOriginPressed( );
   void btnSetPlusXPressed( );
   void btnSetPlusYPressed( );

   void btnStartExperimentPressed( );
   void btnStopExperimentPressed( );

protected:
   QTimer*          m_ArmTimer;
   MicroscribeArm*  m_MicroscribeArm;

   PositionHistory* m_PositionHistory;

   double           m_Calibration[3][3];

   double           m_ProjectedPosition[3];
   double           m_Origin[3];
   double           m_Matrix[3][3];

   double           m_LastSampleTime;
};

#endif // __MICROSCRIBELOGGERFORM_H__