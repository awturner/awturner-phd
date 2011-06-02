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
#ifndef __PICKEDPOINT_H__
#define __PICKEDPOINT_H__

#include <QObject>
#include <QString>

class PickedPoint : public QObject
{
   Q_OBJECT

public:
   PickedPoint( );
   PickedPoint( const PickedPoint& other );
   PickedPoint( const QString& name );
   PickedPoint( const QString& name, const double pnt[3] );

   PickedPoint& operator=( const PickedPoint& other );

   virtual ~PickedPoint( );

   void getPoint( double pnt[3] ) const;
   void setPoint( const double pnt[3] );

   QString getName( ) const;
   void setName( const QString& name );

protected:
   QString m_Name;
   double m_Pnt[3];
};

#include <QVector>

typedef QVector<PickedPoint> PickedPoints;

#endif // __PICKEDPOINT_H__