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
#ifndef __POSITIONHISTORY_H__
#define __POSITIONHISTORY_H__

#include <QVector>
#include <QObject>

class PositionHistory : public QObject
{
   Q_OBJECT;

public:
   PositionHistory();
   virtual ~PositionHistory();

   unsigned int getPointCount() const;

   void getPoint(const unsigned int i, double& time, double pos[3], double joints[6]) const;
   void addPoint(const double time, const double pos[3], const double joints[6]);

   void getBounds(double bounds[6]) const;

   void clear();

signals:
   void pointAdded(const double time, const double x, const double y, const double z);

protected:
   struct timedouble3
   {
      double time;
      double x;
      double y;
      double z;

      double joints[6];
   };
   typedef QVector<timedouble3> PointVector;

   PointVector m_History;

   double m_Bounds[6];
};

#endif // __POSITIONHISTORY_H__