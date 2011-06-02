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
   MicroscribeLoggerForm(QWidget* in_Parent = 0, Qt::WFlags in_Fl = 0);
   virtual ~MicroscribeLoggerForm();

   void setMicroscribeArm(MicroscribeArm* arm);
   MicroscribeArm* microscribeArm();

protected slots:
   void readArmState();

   void updatePosition(const double time, const double x, const double y, const double z);

   void btnCalibratePressed();
   void btnSetOriginPressed();
   void btnSetPlusXPressed();
   void btnSetPlusYPressed();

   void btnStartExperimentPressed();
   void btnStopExperimentPressed();

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