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
#include <iostream>
#include <vector>

#include "vtkPolyDataReader.h"
#include "vtkOBJReader.h"
#include "vtkPLYReader.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkMatrix4x4.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkAxesActor.h"

#include "SimpleVTKAppFramework/SimpleVTKAppFramework.h"
#include "Useful/PrintMacros.h"
#include "Useful/HueWheelSequence.h"

class DisplayPolyData : public AWT::SimpleVTKAppFramework
{
};

int main(int argc, char** argv)
{
   DisplayPolyData displayPolyData;
   displayPolyData.setWindowSize(640, 480);

   vtkRenderer* ren = displayPolyData.getRenderer();

   vtkActor* lastActor = 0;

   double x, y, z, w;
   double overallBounds[6];
   int i;
   bool showAxis = false;

   std::vector<vtkActor*> actors;
   std::vector<bool>      actorsOwnColour;

	for (i = 1; i < argc; i++)
	{
      if (!strcmp(argv[i], "-nocull"))
      {
         if (lastActor != 0)
            lastActor->GetProperty()->BackfaceCullingOff();
      }
      else if (!strcmp(argv[i], "-fix"))
      {
         if (lastActor != 0)
            lastActor->SetPickable(false);
      }
      else if (!strcmp(argv[i], "-axis"))
      {
         showAxis = true;
      }
      else if (!strcmp(argv[i], "-c"))
      {
         if (lastActor != 0)
         {
            x = atof(argv[++i]);
            y = atof(argv[++i]);
            z = atof(argv[++i]);
            w = atof(argv[++i]);

            lastActor->GetProperty()->SetColor(x, y, z);
            lastActor->GetProperty()->SetOpacity(w);
            actorsOwnColour.back() = true;
         }
      }
      else if (!strcmp(argv[i], "-t"))
      {
         if (lastActor != 0)
         {
            x = atof(argv[++i]);
            y = atof(argv[++i]);
            z = atof(argv[++i]);

            lastActor->SetPosition(x, y, z);
         }
      }
      else if (!strcmp(argv[i], "-r"))
      {
         if (lastActor != 0)
         {
            x = atof(argv[++i]);
            y = atof(argv[++i]);
            z = atof(argv[++i]);

            lastActor->SetOrientation(x, y, z);

            lastActor->GetMatrix()->Print(std::cerr);
         }
      }
      else if (!strcmp(argv[i], "-m"))
      {
         if (lastActor != 0)
         {
            vtkMatrix4x4* mat = vtkMatrix4x4::New();

            for (int e = 0; e < 12; ++e)
               (*mat)[ e%4 ][ e/4 ] = atof(argv[++i]);

            lastActor->SetUserMatrix(mat);

            lastActor->GetUserMatrix()->Print(std::cerr);
         }
      }
      else
      {
         std::string name(argv[i]);
         vtkAlgorithm* reader = 0;

         if (0 == name.compare(name.length()-4, 4, ".vtk"))
         {
            vtkPolyDataReader* r = vtkPolyDataReader::New();
            r->SetFileName(argv[i]);
            reader = r;
         }
         else if (0 == name.compare(name.length()-4, 4, ".ply"))
         {
            vtkPLYReader* r = vtkPLYReader::New();
            r->SetFileName(argv[i]);
            reader = r;
         }
         else if (0 == name.compare(name.length()-4, 4, ".obj"))
         {
            vtkOBJReader* r = vtkOBJReader::New();
            r->SetFileName(argv[i]);
            reader = r;
         }

         if (reader == 0)
         {
            DEBUGMACRO("No reader known for " << name << "; skipping.");
         }
         else
         {
            vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
            vtkActor*          actor  = vtkActor::New();

            actor->GetProperty()->BackfaceCullingOn();

            mapper->SetInputConnection(reader->GetOutputPort());
            actor->SetMapper(mapper);

            actor->GetProperty()->BackfaceCullingOff();

            std::cout << "Loading model " << argv[i] << std::endl;
            mapper->Update();
            std::cout << "Done." << std::endl;

            double bounds[6];
            actor->GetBounds(bounds);

            if (lastActor == 0)
            {
               for (int i = 0; i < 6; ++i)
                  overallBounds[i] = bounds[i];
            }
            else
            {
               for (int i = 0; i < 6; i+=2)
               {
                  overallBounds[i+0] = std::min<double>(overallBounds[i+0], bounds[i+0]);
                  overallBounds[i+1] = std::max<double>(overallBounds[i+1], bounds[i+1]);
               }
            }

            ren->AddActor(actor);
            lastActor = actor;

            actors.push_back(actor);
            actorsOwnColour.push_back(false);

            actor->Delete();
            mapper->Delete();
            reader->Delete();
         }
      }
	}

   PRINTVBL(actors.size());
   AWT::ColourSequence::P cols = AWT::HueWheelSequence::getInstance(actors.size());

   std::vector<vtkActor*>::iterator iter     = actors.begin();
   std::vector<vtkActor*>::iterator iterEnd  = actors.end();
   std::vector<bool>::iterator      iterBool = actorsOwnColour.begin();
   
   for (; iter != iterEnd; ++iter, ++iterBool)
   {      
      if (*iterBool)
         continue;

      double rgb[4];
      cols->nextColour(rgb);
      //PRINTVEC(rgb, 4);
      (*iter)->GetProperty()->SetColor(rgb);

      // So I can work out the colours!
      for (unsigned int i = 0; i < 4; ++i)
         rgb[i] = floor(rgb[i]*255 + 0.5);

      PRINTVEC(rgb, 4);
   }

   if (showAxis)
   {
      double axisLength = 0;

      for (int i = 0; i < 6; i += 2)
      {
         axisLength = std::max(axisLength, overallBounds[i+1]-overallBounds[i+0]);
      }

      vtkAxesActor* ax = vtkAxesActor::New();
      ax->SetTotalLength(axisLength, axisLength, axisLength);

      ren->AddActor(ax);

      ax->Delete();
   }

   displayPolyData.start();
}