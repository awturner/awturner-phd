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
#include "include/MicroscribeLoggerForm.h"

#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>

#include <limits>

#include "ArmInRegion/MicroscribeArm.h"

#include "Useful/PrintMacros.h"
#include "Useful/ArrayVectorFunctions.h"

#include "include/PositionHistory.h"

using namespace AWT;

MicroscribeLoggerForm::MicroscribeLoggerForm(QWidget* in_Parent, Qt::WFlags in_Fl)
: QMainWindow(in_Parent, in_Fl), m_MicroscribeArm(0)
{
   setupUi(this);

   m_PositionHistory = new PositionHistory();

   connect(m_PositionHistory, SIGNAL(pointAdded(const double, const double, const double, const double)), 
      this, SLOT(updatePosition(const double, const double, const double, const double)));

   btnCalibrate->setEnabled(false);
   btnSetOrigin->setEnabled(false);
   btnSetPlusX->setEnabled(false);
   btnSetPlusY->setEnabled(false);

   btnStartExperiment->setEnabled(true);
   btnStopExperiment->setEnabled(false);

   connect(btnCalibrate, SIGNAL(pressed()), this, SLOT(btnCalibratePressed()));
   connect(btnSetOrigin, SIGNAL(pressed()), this, SLOT(btnSetOriginPressed()));
   connect(btnSetPlusX,  SIGNAL(pressed()), this, SLOT(btnSetPlusXPressed()));
   connect(btnSetPlusY,  SIGNAL(pressed()), this, SLOT(btnSetPlusYPressed()));

   connect(btnStartExperiment, SIGNAL(pressed()), this, SLOT(btnStartExperimentPressed()));
   connect(btnStopExperiment, SIGNAL(pressed()), this, SLOT(btnStopExperimentPressed()));

   for (int i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
         m_Matrix[i][j] = (i == j) ? 1 : 0;

      m_Origin[i] = 0;
   }

   m_LastSampleTime = -std::numeric_limits<double>::infinity();

   m_ArmTimer = new QTimer();
   m_ArmTimer->setInterval(1);
   connect(m_ArmTimer, SIGNAL(timeout()), this, SLOT(readArmState()));
}

MicroscribeLoggerForm::~MicroscribeLoggerForm()
{
   m_MicroscribeArm->disconnect();
}

void MicroscribeLoggerForm::setMicroscribeArm(MicroscribeArm* arm)
{
   if (m_MicroscribeArm != arm)
   {
      this->m_MicroscribeArm = arm;

      btnCalibrate->setEnabled(arm != 0);
   }
}

void MicroscribeLoggerForm::readArmState()
{
   double pos[3], ori[3], joi[6];

   //m_ProjectedPosition[0] = (rand() + 0.0) / RAND_MAX;
   //m_ProjectedPosition[1] = (rand() + 0.0) / RAND_MAX;
   //m_ProjectedPosition[2] = (rand() + 0.0) / RAND_MAX;

   const double time = m_MicroscribeArm->getPositionAndOrientation(pos, ori, joi);

   if (time != m_LastSampleTime)
   {
     for (int i = 0; i < 3; ++i)
     {
        m_ProjectedPosition[i] = m_Matrix[i][0]*(pos[0] - m_Origin[0])
           + m_Matrix[i][1]*(pos[1] - m_Origin[1])
           + m_Matrix[i][2]*(pos[2] - m_Origin[2]);
     }

     m_PositionHistory->addPoint(time, pos, joi);

     m_LastSampleTime = time;
   }
}

void MicroscribeLoggerForm::updatePosition(const double time, const double x, const double y, const double z)
{
   static int skip = 0;

   if (++skip % 100 == 0)
   {
      label_tx->setText(tr("%1").arg(x));
      label_ty->setText(tr("%1").arg(y));
      label_tz->setText(tr("%1").arg(z));
   }
}

void MicroscribeLoggerForm::btnCalibratePressed()
{
   btnCalibrate->setEnabled(true);
   btnSetOrigin->setEnabled(true);
   btnSetPlusX->setEnabled(false);
   btnSetPlusY->setEnabled(false);
}

void MicroscribeLoggerForm::btnSetOriginPressed()
{
   m_MicroscribeArm->getPosition(m_Calibration[0]);

   for (int i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
      {
         m_Matrix[i][j] = (i == j) ? 1 : 0;
      }

      m_Origin[i] = 0;
   }

   btnCalibrate->setEnabled(true);
   btnSetOrigin->setEnabled(true);
   btnSetPlusX->setEnabled(true);
   btnSetPlusY->setEnabled(false);
}

void MicroscribeLoggerForm::btnSetPlusXPressed()
{
   m_MicroscribeArm->getPosition(m_Calibration[1]);

   btnSetOrigin->setEnabled(true);
   btnSetPlusX->setEnabled(true);
   btnSetPlusY->setEnabled(true);
}

void MicroscribeLoggerForm::btnSetPlusYPressed()
{
   m_MicroscribeArm->getPosition(m_Calibration[2]);

   PRINTVEC(m_Calibration[0], 3);
   PRINTVEC(m_Calibration[1], 3);
   PRINTVEC(m_Calibration[2], 3);

   for (int ax = 0; ax < 3; ++ax)
   {
      m_Origin[ax] = m_Calibration[0][ax];

      m_Matrix[0][ax] = m_Calibration[1][ax] - m_Origin[ax];
      m_Matrix[1][ax] = m_Calibration[2][ax] - m_Origin[ax];
   }

   cross(m_Matrix[0], m_Matrix[1], m_Matrix[2]);
   cross(m_Matrix[2], m_Matrix[0], m_Matrix[1]);

   for (int ax = 0; ax < 3; ++ax)
      normalize(m_Matrix[ax], 3);

   PRINTVEC(m_Origin, 3);
   PRINTVEC(m_Matrix[0], 3);
   PRINTVEC(m_Matrix[1], 3);
   PRINTVEC(m_Matrix[2], 3);

   btnCalibrate->setEnabled(true);
   btnSetOrigin->setEnabled(false);
   btnSetPlusX->setEnabled(false);
   btnSetPlusY->setEnabled(false);
}

void MicroscribeLoggerForm::btnStartExperimentPressed()
{
   if (btnStartExperiment->isEnabled())
   {
      m_PositionHistory->clear();

      btnStartExperiment->setEnabled(false);
      btnStopExperiment->setEnabled(true);

      m_ArmTimer->start();
   }
}

#include <QTextStream.h>

void MicroscribeLoggerForm::btnStopExperimentPressed()
{
   if (btnStopExperiment->isEnabled())
   {
      m_ArmTimer->stop();

      btnStartExperiment->setEnabled(true);
      btnStopExperiment->setEnabled(false);

      while (true)
      {
         QString filename = QFileDialog::getSaveFileName(this, tr("End of Experiment"));

         if (filename.length() == 0)
         {
            if (QMessageBox::Ok == QMessageBox::question(this, tr("End of Experiment"), tr("No data will be saved.\nDo you wish to discard the data?"), QMessageBox::Ok | QMessageBox::Cancel))
               break;
         }
         else
         {

            QFile file(filename);

            if (file.open(QFile::WriteOnly))
            {
               QTextStream ts(&file);

               ts << tr("time,x,y,z\n");

               double pos[3], joi[6], time;

               const unsigned int imax = m_PositionHistory->getPointCount();
               for (unsigned int i = 0; i < imax; ++i)
               {
                  m_PositionHistory->getPoint(i, time, pos, joi);

                  ts << tr("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10\n").arg(time)
                      .arg(pos[0]).arg(pos[1]).arg(pos[2])
                      .arg(joi[0]).arg(joi[1]).arg(joi[2]).arg(joi[3]).arg(joi[4]).arg(joi[5]);
               }

               file.close();

               QMessageBox::information(this, tr("End of Experiment"), tr("Data was written to %1").arg(filename));
               break;
            }
            else
            {
               QMessageBox::warning(this, tr("End of Experiment"), tr("Could not write to %1").arg(filename));
            }
         }
      }
   }
}