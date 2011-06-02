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
#include "MicroscribeArm.h"

#include <windows.h>
#include <armdll32.h>

#include <iostream>

#include <time.h>
#include <cmath>

#include <map>

int MicroscribeArm::connectedCount = 0;
long MicroscribeArm::clocksPerUpdate = 0;
long MicroscribeArm::lastUpdate = 0;

double MicroscribeArm::PIBY180 = 4.0 * atan(1.0) / 180.0;

MicroscribeArm* MicroscribeArm::getInstance()
{
   return new MicroscribeArm(10);
}

MicroscribeArm::MicroscribeArm(int _updatesPerSecond)
: updatesPerSecond(_updatesPerSecond), secondsPerClock(1.0 / CLOCKS_PER_SEC)
{
   this->connected        = false;
   this->lastUpdate       = 0;

   this->clocksPerUpdate  = static_cast<clock_t>((CLOCKS_PER_SEC + 0.0) / (updatesPerSecond + 0.0));

   int nResult = ArmStart(NULL);
	if (ARM_SUCCESS != nResult) 
	{ 
      std::cerr << "Unable to Start ArmDll32" << std::endl; 
      canConnect = false;
		return;
	}

   ArmEnd();

   canConnect = true;
}

MicroscribeArm::~MicroscribeArm()
{
   std::cerr << "Deleting me!" << std::endl;

   if (isConnected())
      disconnect();
}

int MicroscribeArm::getUpdateFrequency()
{
   return updatesPerSecond;
}

bool MicroscribeArm::isAtLeastOneInstanceConnected()
{
   return connectedCount != 0;
}

void MicroscribeArm::disconnectAll()
{
   connectedCount = 0;
   ArmEnd();
}

bool MicroscribeArm::isConnected()
{
   return connected = connected && isAtLeastOneInstanceConnected();
}

bool MicroscribeArm::connect()
{
   if (!canConnect)
      return false;

   std::cerr << "There are already " << connectedCount << " clients connected." << std::endl;
   std::cerr << "Arm object " << this << " is trying to connect." << std::endl;
   std::cerr << this << "->" << canConnect << std::endl;
   // If this instance is already connected, the programmer should have
   // checked this first
   if (isConnected())
   {
      throw "Arm is already connected!";
   }

   // If there is at least one other connected, then we don't need to do
   // anything
   if (isAtLeastOneInstanceConnected())
   {
      connectedCount++;
      connected = true;
      return true;
   }

   int nResult;

   nResult = ArmStart(NULL);
	if (ARM_SUCCESS != nResult) 
	{ 
      std::cerr << "Unable to Start ArmDll32" << std::endl; 
      canConnect = false;
		return false;
	}

   nResult = ArmSetErrorHandlerFunction(NO_HCI_HANDLER, NULL);
	nResult = ArmSetErrorHandlerFunction(BAD_PORT_HANDLER, NULL);
	nResult = ArmSetErrorHandlerFunction(CANT_OPEN_HANDLER, NULL);
	nResult = ArmSetErrorHandlerFunction(CANT_BEGIN_HANDLER, NULL);

   nResult = ArmConnect(0,0); 
	if (ARM_SUCCESS != nResult) 
	{ 
      std::cerr << "Unable to Connect ArmDll32" << std::endl; 
		disconnectAll();
      canConnect = false;
		return false;
	}

   nResult = ArmSetUpdateEx(ARM_FULL, updatesPerSecond); 
	if (ARM_SUCCESS != nResult) 
	{ 
      std::cerr << "Unable to set Update type for ArmDll32" << std::endl; 
		ArmDisconnect();

		disconnectAll();
		
      return false;
	}

   ArmSetLengthUnits(ARM_MM);
   ArmSetAngleUnits(ARM_DEGREES);

   connected = true;
   connectedCount++;
   return true;
}

bool MicroscribeArm::disconnect()
{
   std::cerr << "There are " << connectedCount << " clients connected." << std::endl;
   std::cerr << "Arm object " << this << " is trying to disconnect." << std::endl;
   
   if (!isConnected())
   {
      throw "This arm is not connected!";
   }

   connectedCount--;
   connected = false;

   if (!isAtLeastOneInstanceConnected())
   {
      std::cerr << "Last one out, switch off..." << std::endl;
      ArmDisconnect();

	   disconnectAll();
   }

   return true;
}

bool MicroscribeArm::needsToUpdate()
{
   clock_t currentTime = clock();

   return (currentTime - lastUpdate > clocksPerUpdate);
}

void MicroscribeArm::doUpdateIfNeeded()
{
   if (needsToUpdate())
   {   
      //std::cerr << "An update is needed!" << std::endl;
      
      if (!isConnected())
      {
         //std::cerr << "Arm isn't connected: connecting" << std::endl;
         this->connect();
      }

      if (ArmGetTipPosition(&position) == ARM_NOT_CONNECTED 
        || ArmGetTipOrientation(&orientation) == ARM_NOT_CONNECTED 
        || ArmGetJointAngles(ARM_DEGREES, joints) == ARM_NOT_CONNECTED)
      {
         //std::cerr << "Connection lost!" << std::endl;
         this->disconnect();
	   }
   
      lastUpdate = clock();
      //std::cerr << "Successfully updated";
   }
   else
   {
      //std::cerr << "No need for update!" << std::endl;
   }
}

void MicroscribeArm::zeroPositionAndOrientation()
{
   this->orientation.x = this->orientation.y = this->orientation.z = 0;
   this->position.x    = this->position.y    = this->position.z    = 0;
}

double MicroscribeArm::getPosition(double *pos)
{
   doUpdateIfNeeded();

   pos[0] = this->position.x;
   pos[1] = this->position.y;
   pos[2] = this->position.z;

   return lastUpdate * secondsPerClock;
}

double MicroscribeArm::getOrientation(double *ori)
{
   doUpdateIfNeeded();

   ori[0] = this->orientation.x;
   ori[1] = this->orientation.y;
   ori[2] = this->orientation.z;

   return lastUpdate * secondsPerClock;
}

double MicroscribeArm::getPositionAndOrientation(double* pos, double *ori, double* joints)
{
   doUpdateIfNeeded();

   pos[0] = this->position.x;
   pos[1] = this->position.y;
   pos[2] = this->position.z;

   ori[0] = this->orientation.x;
   ori[1] = this->orientation.y;
   ori[2] = this->orientation.z;

  if (joints)
  {
    for (int i = 0; i < 6; ++i)
      joints[i] = this->joints[i];
  }

   //std::cerr << lastUpdate << "\t" << pos[0] << "\t" << pos[1] << "\t" << pos[2] << std::endl;

   return lastUpdate * secondsPerClock;
}

template <class T>
void MicroscribeArm::swap(T& a, T& b)
{
   const T tmp = a;
   a = b;
   b = tmp;
}

void MicroscribeArm::transposeMatrix(double* mat)
{
   swap(mat[ 1], mat[ 4]);
   swap(mat[ 2], mat[ 8]);
   swap(mat[ 3], mat[12]);

   swap(mat[ 6], mat[ 9]);
   swap(mat[ 7], mat[13]);

   swap(mat[11], mat[14]);
}

void MicroscribeArm::getMatrixRowMajor(double* mat)
{
   getMatrixColMajor(mat);

   transposeMatrix(mat);
}

void MicroscribeArm::getMatrixColMajor(double* mat)
{
   doUpdateIfNeeded();

   // Return 4x4 matrix in a column-major order, like OpenGL

   // [  mat[ 0]  mat[ 4]  mat[ 8]  mat[12]
   // [  mat[ 1]  mat[ 5]  mat[ 9]  mat[13]
   // [  mat[ 2]  mat[ 6]  mat[10]  mat[14]
   // [  mat[ 3]  mat[ 7]  mat[11]  mat[15]

   // Avoid repeated calculations...
   double cx = cos(this->orientation.x * PIBY180);
   double cy = cos(this->orientation.y * PIBY180 );
   double cz = cos(this->orientation.z * PIBY180 );

   double sx = sin(this->orientation.x * PIBY180 );
   double sy = sin(this->orientation.y * PIBY180 );
   double sz = sin(this->orientation.z * PIBY180 );

   mat[ 0] = cz * cy;
   mat[ 4] = cz * sy * sx - sz * cx;
   mat[ 8] = cz * sy * cx + sz * sx;
   
   mat[ 1] = sz * cy;
   mat[ 5] = sz * sy * sx + cz * cx;
   mat[ 9] = sz * sy * cx - cz * sx;
   
   mat[ 2] = -sy;
   mat[ 6] = cy * sx;
   mat[10] = cy * cx;
   
   mat[ 3] = mat[ 7] = mat[11] = 0.0;
   mat[15] = 1.0;
   
   mat[12] = this->position.x;
   mat[13] = this->position.y;
   mat[14] = this->position.z;

}