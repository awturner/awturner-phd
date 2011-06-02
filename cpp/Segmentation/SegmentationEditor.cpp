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
#include "AllSliceHoleFill.h"

#include "CImgDICOMReader/CImgDICOMReader.h"

#include "CImgDispInputEventDispatcher.h"
#include "ColumnFillTool.h"
#include "RowFillTool.h"
#include "Command.h"
#include "CommandManager.h"
#include "ConnectedRegions.h"
#include "Fill3DTool.h"
#include "FillTool.h"
#include "HoleFillTool.h"
#include "IntensityWindowGui.h"
#include "LassooTool.h"
#include "PenTool.h"
#include "PolygonTool.h"
#include "RectangularLassoo.h"
#include "RegionTool.h"
#include "SegmentationLUT.h"
#include "SegmentationLUTGui.h"
#include "Toolbox.h"
#include "ToolboxGui.h"

#include "Useful/PrintMacros.h"
#include "Useful/RunLengthEncode.h"

#include "WindowSet.h"
#include <CImg.h>
#include <fstream>
#include <iostream>
#include <iostream>
#include <sstream>
#include <vector>
#include <vector>

using namespace cimg_library;

typedef unsigned char SegmentationType;
typedef CImg<SegmentationType> SegmentationImage;

#define TYPED_DIV(Ty, a, b) (static_cast<Ty>(a) / static_cast<Ty>(b))

struct ImageDims {
   unsigned int size[3];   
   float        spacing[3];
} imagedims;

void tightenExtents(int* extent, CImg<short> img)
{
   for (int i = 0; i < 6; ++i)
      std::cerr << extent[i] << " ";
   std::cerr << std::endl;

   for (; extent[0] < extent[1]; ++extent[0])
   {
      int y, z;

      short first = img(extent[0], 0, 0);
      bool flag = false;
      cimg_forYZ(img, y, z)
      {
         if (img(extent[0], y, z) != first)
         {
            y = img.dimy();
            z = img.dimz();
            flag = true;
            break;
         }
      }

      if (flag)
         break;
   }
   for (; extent[0] < extent[1]; --extent[1])
   {
      int y, z;

      short first = img(extent[1], 0, 0);
      bool flag = false;
      cimg_forYZ(img, y, z)
      {
         if (img(extent[0], y, z) != first)
         {
            y = img.dimy();
            z = img.dimz();
            flag = true;
            break;
         }
      }

      if (flag)
         break;
   }

   for (; extent[2] < extent[3]; ++extent[2])
   {
      int x, z;

      short first = img(0, extent[2], 0);
      bool flag = false;
      cimg_forXZ(img, x, z)
      {
         if (img(x, extent[2], z) != first)
         {
            x = img.dimx();
            z = img.dimz();
            flag = true;
            break;
         }
      }

      if (flag)
         break;
   }
   for (; extent[2] < extent[3]; --extent[3])
   {
      int x, z;

      short first = img(0, extent[3], 0);
      bool flag = false;
      cimg_forXZ(img, x, z)
      {
         if (img(x, extent[3], z) != first)
         {
            x = img.dimx();
            z = img.dimz();
            flag = true;
            break;
         }
      }

      if (flag)
         break;
   }
 
   for (; extent[4] < extent[5]; ++extent[4])
   {
      int x, y;

      short first = img(0, 0, extent[4]);
      bool flag = false;
      cimg_forXY(img, x, y)
      {
         if (img(x, y, extent[4]) != first)
         {
            x = img.dimx();
            y = img.dimy();
            flag = true;
            break;
         }
      }

      if (flag)
         break;
   }
   for (; extent[4] < extent[5]; --extent[5])
   {
      int x, y;

      short first = img(0, 0, extent[5]);
      bool flag = false;
      cimg_forXY(img, x, y)
      {
         if (img(x, y, extent[5]) != first)
         {
            x = img.dimx();
            y = img.dimy();
            flag = true;
            break;
         }
      }

      if (flag)
         break;
   }

   for (int i = 0; i < 6; ++i)
      std::cerr << extent[i] << " ";
   std::cerr << std::endl;
}

template <class T>
void displayImage(CImg<T>& img, const char* title = 0)
{
   CImgDisplay disp(img, title);

   while (!disp.is_closed)
   {
      cimg::wait(20);
   }

   std::cerr << "Returning..." << std::endl;
}

template <class T>
CImg<T> createRandomPalette(int ncolours, int nchannels, T* mins, T* maxs)
{
   CImg<T> ret(ncolours, 1, 1, nchannels);

   int x, v;

   cimg_forXV(ret, x, v)
      ret(x, 0, 0, v) = mins[v] + static_cast<T>((maxs[v]-mins[v]) * static_cast<double>(rand()) / static_cast<double>(RAND_MAX));

   return ret;
}

class EventListener : 
   public AWT::MouseListener, public AWT::MouseMoveListener, public AWT::MouseWheelListener, public AWT::KeyListener
{
public:
   void mousePressed(int x, int y, int button)
   {
      //std::cerr << "Button " << button << " was pressed at " << x << "," << y << std::endl;
   }

   void mouseReleased(int x, int y, int button)
   {
      //std::cerr << "Button " << button << " was released at " << x << "," << y << std::endl;
   }

   void mouseMoved(int x, int y, int button)
   {
      //std::cerr << "Mouse was moved to " << x << "," << y << " with button " << button << std::endl;
   }

   void mouseWheeled(int amount)
   {
      //std::cerr << "Mouse was wheeled by " << amount << std::endl;
   }

   void keyPressed(int key)
   {
      std::cerr << key << " was pressed" << std::endl;
   }

   void keyReleased(int key)
   {
      std::cerr << key << " was released" << std::endl;
   }
};

bool fileExists(const char* fn)
{
   std::ifstream input;
   input.open(fn, std::ifstream::in);
   input.close();

   if (input.fail())
   {
      input.clear(std::ios::failbit);
      return false;
   }
   else
   {
      return true;
   }
}

std::string lastFilename;

SegmentationImage loadSegmentation(std::string& filename, bool& isOK)
{
   if (filename.length() == 0 && lastFilename.length() != 0)
      filename = lastFilename;

   if (filename.length() == 0)
   {
      isOK = false;
      return SegmentationImage(1, 1);
   }

   std::ifstream is(filename.c_str(), std::ifstream::binary);
   if (is.good())
   {
      std::cerr << "Reading file " << filename.c_str() << std::endl;
      SegmentationImage ret = runLengthDecode<SegmentationType>(is);
      std::cerr << "Done." << std::endl;

      is.close();

      lastFilename = filename;
      isOK = true;
      return ret;
   }
   else
   {
      is.close();

      std::cerr << "Bad file stream " << filename.c_str() << std::endl;

      isOK = false;

      lastFilename = "";
      return SegmentationImage(1, 1);
   }
}

SegmentationImage loadSegmentation(bool& isOK)
{
   std::cerr << "Enter the filename of the segmentation to load [" << lastFilename << "]: ";
   std::string filename;
   std::cin >> filename;

   return loadSegmentation(filename, isOK);
}

bool loadSegmentationAndCopyInto(SegmentationImage& segmentation, std::string& filename)
{
   bool isOK;

   SegmentationImage newseg = loadSegmentation(filename, isOK);

   if (isOK)
   {
      if (newseg.dimx() == segmentation.dimx() && newseg.dimy() == segmentation.dimy() && 
         newseg.dimz() == segmentation.dimz() && newseg.dimv() == segmentation.dimv())
      {
         segmentation = newseg;

         return true;
      }
      else
      {
         std::cerr << "Images are different sizes!" << std::endl;
      }
   }

   return false;
}


void saveFeatures(SegmentationType cc, CImg<short>& scan, SegmentationImage& im, double* spacing)
{
   /*
   std::cerr << "Enter the filename to save: ";
   std::string filename;
   std::cin >> filename;

   FeatureTool<SegmentationType,short,1> featureTool;
   
   std::vector<AWT::Point<int> > points;

   featureTool.setScanImage(&scan);
   featureTool.setCurrentImage(&im);

   std::ofstream os(filename.c_str());

   for (int z = 0; z < scan.dimz(); ++z)
   {
      featureTool.findPoints(cc, z, points);

      std::vector<AWT::Point<int> >::iterator       begin = points.begin();
      std::vector<AWT::Point<int> >::iterator       end   = points.end();

      DEBUGMACRO(z << " " << points.size());
      for (; begin != end; ++begin)
      {
         os        << (spacing[0] * (*begin).x) << " " << (spacing[1] * (*begin).y) << " " << (spacing[2] * z) << std::endl;
         //std::cerr << (spacing[0] * (*begin).x) << " " << (spacing[1] * (*begin).y) << " " << (spacing[2] * z) << std::endl;
      }

      points.clear();
   }

   os.close();
   std::cerr << "Saved" << std::endl;
   */
   std::cerr << "Commented out!" << std::endl;
}

void saveFile(SegmentationImage& im, bool prompt = true)
{
   std::string filename;
   if (prompt || !strcmp(lastFilename.c_str(), ""))
   {
      char cfilename[256];

      std::cerr << "Enter the filename to save [" << lastFilename << "]: ";
      std::cin.getline(cfilename, 256);
      //std::cin >> filename;

      filename = std::string(cfilename);

      if (filename.length() == 0 && lastFilename.length() != 0)
         filename = lastFilename;
   }
   else
   {
      filename = lastFilename;
   }

   if (filename.length() == 0)
      return;

   bool write = true;

   if (prompt && fileExists(filename.c_str()))
   {
      while (1)
      {
         std::cerr << "File \"" << filename << "\" already exists: do you want to overwrite [Y/[N]]" << std::endl;

         std::string response;
         std::cin >> response;

         if (stricmp(response.c_str(), "Y") == 0)
         {
            break;
         }
         else if (stricmp(response.c_str(), "N") == 0)
         {
            write = false;
            break;
         }
      }
   }

   if (write)
   {
      std::ofstream outstream;
      outstream.open(filename.c_str(), std::ofstream::binary);

      std::cerr << "Saving \"" << filename << "\"" << std::endl;
      runLengthEncode(im, outstream);
      std::cerr << "Done." << std::endl;

      outstream.close();

      lastFilename = filename;
   }
}



class SliceChangeEvent : public UndoableCommand
{
public:
   void init(int* sliceVar, int sliceTo)
   {
      doneState = false;
      sliceXor = *(sliceVarPtr = sliceVar) ^ sliceTo;
   }

   virtual bool execute()
   {
      if (!doneState)
      {
         *sliceVarPtr = *sliceVarPtr ^ sliceXor;
         doneState = true;

         return true;
      }
      else
      {
         return false;
      }
   }

   virtual bool undo()
   {
      if (doneState)
      {
         *sliceVarPtr = *sliceVarPtr ^ sliceXor;
         doneState = false;
      
         return true;
      }
      else
      {
         return false;
      }
   }

protected:
   bool doneState;
   int* sliceVarPtr;
   int sliceXor;
};

template <class T>
CImg<unsigned char> drawGraph(CImg<T>& data, unsigned int h)
{
   CImg<unsigned char> ret(data.dimx(), h, 1, 3);
   ret.fill(255);

   unsigned char col[] = { 255, 0, 0 };

   T minn = data.min();
   T maxx = data.max();

   T delta = maxx - minn;
   minn -= delta / 10;
   maxx += delta / 10;

   cimg_forY(data, y)
   {
      for (int x = 1; x < data.dimx(); ++x)
      {
         int y0 = h - static_cast<int>(h * (data(x-1,y) - minn + 0.0) / (maxx - minn + 0.0) + 0.5);
         int y1 = h - static_cast<int>(h * (data(x  ,y) - minn + 0.0) / (maxx - minn + 0.0) + 0.5);

         ret.draw_line(x-1, y0, x, y1, col);
      }
   }

   return ret;
}

CImg<float> calculateLocalStatistics(CImg<short>& scanImage)
{
   CImg<float> statImage(scanImage.dimx(), scanImage.dimy(), 1, 2);
   statImage.fill(0);

   cimg_forXYZ(scanImage, x, y, z)
   {
      statImage(x, y, 0, 0) += scanImage(x, y, z);
      statImage(x, y, 0, 1) += scanImage(x, y, z)*scanImage(x, y, z);
   }

   // Calculate the mean
   statImage.get_shared_channel(0) /= scanImage.dimz() + 0.f;

   // Calculate the variance;
   statImage.get_shared_channel(1) -= statImage.get_shared_channel(0).get_pow(2) * scanImage.dimz();

   return statImage;
}

float windowCentre;
float windowWidth;

template <class T>
T normalizeToWindow(T val)
{
   return static_cast<T>(std::max<T>(0, std::min<T>(1, (val - windowCentre - 0.5) / windowWidth + 0.5)));
}

void erode(SegmentationImage& im)
{
   SegmentationImage sel(3, 3, 3);
   sel.fill(1);

   im.erode(sel);
}

void dilate(SegmentationImage& im)
{
   SegmentationImage sel(3, 3, 3);
   sel.fill(1);

   im.dilate(sel);
}

void marchingCubes(SegmentationImage& im, SegmentationType label)
{
   SegmentationImage copy(im);

   cimg_forXYZ(im, x, y, z)
      copy(x, y, z) = (im(x,y,z) == label) ? 255 : 0;

   CImgList<> points;
   CImgList<> primitives;
   
   copy.marching_cubes(127.5, points, primitives);

   PRINTVBL(points.size);
   PRINTVBL(primitives.size);

   if (points.size > 0)
   {
      CImgList<> colors(primitives.size, 1, 3);
      for (int i = 0; i < primitives.size; ++i)
      {
         colors[i](0, 0) = 255;
         colors[i](0, 1) = 255;
         colors[i](0, 2) = 255;
      }

      CImgDisplay disp;
      disp.display_object3d(points, primitives, colors);

      while (!disp.is_closed)
         cimg::wait(20);
   }

}

unsigned char createColours(SegmentationLUT& segLut)
{
   unsigned char ncolours = 0;
   {
      segLut.setColour(++ncolours, 255,   0,   0);
      segLut.setColour(++ncolours,   0, 255,   0);
      segLut.setColour(++ncolours,   0,   0, 255);

      segLut.setColour(++ncolours, 255, 255,   0);
      segLut.setColour(++ncolours, 255,   0, 255);
      segLut.setColour(++ncolours,   0, 255, 255);
   }

   ++ncolours;

   return ncolours;
}

CImg<short> loadBinary(const char* filename, const ImageDims& imdims)
{
   CImg<short> ret(imdims.size[0], imdims.size[1], imdims.size[2]);

   std::ifstream is(filename, std::ios::binary);
   if (!is.good())
      DEBUGLINEANDEXIT(1);

   is.read(reinterpret_cast<char*>(ret.data), imdims.size[0]*imdims.size[1]*imdims.size[2]*2);

   is.close();

   return ret;
}

int main(int argc, char** argv)
{
   char* filename = argv[1];

   imagedims.size[0] = atoi(argv[2]);
   imagedims.size[1] = atoi(argv[3]);
   imagedims.size[2] = atoi(argv[4]);

   PRINTVBL(argc);
   PRINTVEC(imagedims.size, 3);

   if (argc > 5)
   {
      imagedims.spacing[0] = atof(argv[5]);
      imagedims.spacing[1] = atof(argv[6]);
      imagedims.spacing[2] = atof(argv[7]);
   }
   else
   {
      imagedims.spacing[0] = 1;
      imagedims.spacing[1] = 1;
      imagedims.spacing[2] = 1;
   }

   std::cerr << "Loading DICOM from " << filename << std::endl;

   double spacing[3];
   CImg<short> scanImage = loadBinary(filename, imagedims);

   spacing[0] = spacing[1] = spacing[2] = 1;

   int extent[6];
   extent[0] = extent[2] = extent[4] = 0;
   extent[1] = scanImage.dimx()-1;
   extent[3] = scanImage.dimy()-1;
   extent[5] = scanImage.dimz()-1;

   WindowSet windowSet;

   double scaledSpacing[3];
   double minSpacing = spacing[0];
   for (int i = 1; i < 2; ++i)
      minSpacing = min<double>(minSpacing, spacing[i]);
      
   for (int i = 0; i < 3; ++i)
      scaledSpacing[i] = spacing[i] / minSpacing;

   std::cerr << "Spacing = " << spacing[0] << " x " << spacing[1] << " x " << spacing[2] << std::endl;

   short imgMin = scanImage.min();
   short imgMax = scanImage.max();
   
   CImg<unsigned char> outputImage(scanImage.dimx(), scanImage.dimy(), 1, 3);

   DEBUGMACRO("Created outputImage");

   CImg<float> statImage = calculateLocalStatistics(scanImage);

   DEBUGMACRO("Calculated image statistics");

   CImgList<unsigned char> font = CImgList<unsigned char>::get_font(8);
   unsigned char white[] = { 255, 255, 255 };
   unsigned char black[] = {   0,   0,   0 };

   int sliceMin  = extent[4];
   int sliceMax  = extent[5];
   int slice     = 0;
   int sliceLast = -1;
   int sliceDelta = 1;

   windowCentre = (imgMin + imgMax) / 2.f;
   windowWidth  = imgMax - imgMin;

   if (argc > 2) windowCentre = atof(argv[2]);
   if (argc > 3) windowWidth = atof(argv[3]);

   std::cerr << "Min = " << imgMin << std::endl;
   std::cerr << "Max = " << imgMax << std::endl;
   std::cerr << "Window = [" << windowCentre << "," << windowWidth << "]" << std::endl;

   int x, y, z;
   float xx, yy, zz;
   unsigned char v;

   SegmentationLUT segLut;
   const unsigned char ncolours = createColours(segLut);

   tightenExtents(extent, scanImage);

   bool mouseLDown = false;
   bool mouseRDown = false;
   bool sliceMoving = false;

   int mouseDownAtX, mouseDownAtY;
   float centreDown;
   float windowDown;

   SegmentationImage segmentation(scanImage.dimx(), scanImage.dimy(), scanImage.dimz());

   loadSegmentationAndCopyInto(segmentation, /*std::string(filename) + "/*/std::string("") + "Segmentation.rle");

   CImgDisplay disp(segmentation.get_shared_plane(0), filename, 0);

   windowSet.add(&disp);

   EventListener ev;
   CImgDispInputEventDispatcher eventDispatcher;
   eventDispatcher.addMouseListener(&ev);
   eventDispatcher.addMouseMoveListener(&ev);
   eventDispatcher.addMouseWheelListener(&ev);
   eventDispatcher.addKeyListener(&ev);

   SegmentationType fg[] = { 1 };
   SegmentationType bg[] = { 0 };

   SegmentationLUTGui* segLutGui = new SegmentationLUTGui("Colour Picker", &segLut, &fg[0], &bg[0]);

   IntensityWindowGui* intWindGui = new IntensityWindowGui("Intensity Window Picker", &windowCentre, &windowWidth);
   intWindGui->setImage(scanImage);

   windowSet.add(segLutGui->getDisplay());
   windowSet.add(intWindGui->getDisplay());

   typedef DrawingTool<SegmentationType,short,1> SegmentationTool;

   SegmentationImage tempImage(segmentation.dimx(), segmentation.dimy(), 1, 1);

   CommandManager commandManager;

   Toolbox<SegmentationType,short,1>* toolbox = new Toolbox<SegmentationType,short,1>();

   toolbox->addTool(new PenTool<SegmentationType,short,1>(&commandManager));
   toolbox->addTool(new FillTool<SegmentationType,short,1>(&commandManager));
   toolbox->addTool(new PolygonTool<SegmentationType,short,1>(&commandManager));
   toolbox->addTool(new Fill3DTool<SegmentationType,short,1>(&commandManager));
   toolbox->addTool(new LassooTool<SegmentationType,short,1>(&commandManager));
   toolbox->addTool(new RectangularLassoo<SegmentationType,short,1>(&commandManager));
   toolbox->addTool(new RegionTool<SegmentationType,short,1>(&commandManager));
   toolbox->addTool(new HoleFillTool<SegmentationType,short,1>(&commandManager));
   toolbox->addTool(new AllSliceHoleFillTool<SegmentationType,short,1>(&commandManager));
   toolbox->addTool(new ColumnFillTool<SegmentationType,short,1>(&commandManager, DIR_DOWNWARDS));
   toolbox->addTool(new ColumnFillTool<SegmentationType,short,1>(&commandManager, DIR_UPWARDS));
   toolbox->addTool(new RowFillTool<SegmentationType,short,1>(&commandManager, DIR_RIGHT));
   toolbox->addTool(new RowFillTool<SegmentationType,short,1>(&commandManager, DIR_LEFT));

   ToolboxGui<SegmentationType,short,1>* toolboxGui = new ToolboxGui<SegmentationType,short,1>("Editing Tools", toolbox);
   windowSet.add(toolboxGui->getDisplay());
   
   toolbox->setCurrentToolIndex(0);
   SegmentationTool* currentTool = toolbox->getCurrentTool();
   currentTool->setForegroundColour(fg);

   //int currentToolIndex = 0;
   bool colourChange = true;

   int lastButton = 0;
   int lastMouseX = -1;
   int lastMouseY = -1;

   bool displaySegmentation = true;

   SegmentationImage beforeEdit(segmentation.dimx(), segmentation.dimy(), 1, segmentation.dimv());

   bool drawLegend = true;

   slice = scanImage.dimz() / 2;

   while (!disp.is_closed)
   {
      windowSet.show();

      
      bool redraw = false;

      if (disp.key == cimg::keyHOME)
      {
         disp.wheel = sliceMin - slice;
         disp.flush();
      }
      else if (disp.key == cimg::keyEND)
      {
         disp.wheel = sliceMax - slice;
         disp.flush();
      }
      else if (disp.key == cimg::keyPAGEUP)
      {
         disp.wheel =  10;
         disp.flush();
      }
      else if (disp.key == cimg::keyPAGEDOWN)
      {
         disp.wheel = -10;
         disp.flush();
      }

      switch (disp.key)
      {
      case cimg::keyU:
         {
            if (commandManager.canUndo())
               commandManager.undo();

            disp.flush();
         }
         break;

      case cimg::keyI:
         {
            if (commandManager.canRedo())
               commandManager.redo();

            disp.flush();
         }
         break;

      case cimg::keyM:
         {
            CImgDisplay meanDisplay(statImage.get_shared_channel(0).get_apply(normalizeToWindow<float>), "Mean", 3);
            CImgDisplay varDisplay((statImage.get_shared_channel(1) + 1).get_log(), "Variance", 3);

            while (!meanDisplay.is_closed && !varDisplay.is_closed)
               cimg::wait(20);
            
            disp.flush();
         }
         break;

      case cimg::keyR:
         {
            SegmentationImage reg = segmentation.get_shared_plane(slice).get_label_regions();
            //SegmentationImage reg = findConnectedRegions(segmentation.get_shared_plane(slice));

            unsigned char mins[] = { 127, 127, 127 };
            unsigned char maxs[] = { 255, 255, 255 };

            CImg<unsigned char> pal = createRandomPalette(256, 3, mins, maxs);
            CImgDisplay disp2(reg.get_LUTtoRGB(pal));

            while (!disp2.is_closed)
               cimg::wait(20);

            disp.flush();
         }
         break;

      case cimg::keyW:
         {
            windowSet.hide();

            std::cerr << "Enter window centre and width: ";
            std::cin >> windowCentre >> windowWidth;

            redraw = true;
            disp.flush();

            windowSet.show();
         }
         break;

      case cimg::keyC:
         {
            fg[0] = ++fg[0]%ncolours;
            redraw = true;
            colourChange = true;
            disp.flush();
         }
         break;

      case cimg::keyB:
         {
            bg[0] = ++bg[0]%ncolours;
            redraw = true;
            colourChange = true;
         }
         disp.flush();
         break;

      case cimg::keyV:
         {
            // Swap the foreground and background colours
            unsigned char tmp = fg[0];
            fg[0] = bg[0];
            bg[0] = tmp;
            redraw = true;
            colourChange = true;
         }
         disp.flush();
         break;

      case cimg::keyT:
         {
            windowSet.hide();
            short t;
            std::string maskFilename;
            int all;

            std::cerr << "Enter the threshold [" << imgMin << "->" << imgMax << "]: ";
            std::cin >> t;
            std::cerr << "Load threshold mask? [Enter # for none]";
            std::cin >> maskFilename;
            std::cerr << "Apply to all slices? ";
            std::cin >> all;

            CImg<short> maskImage(scanImage.dimx(), scanImage.dimy(), 1, 1);

            if (maskFilename != std::string("#"))
            {
               maskImage = maskImage.load(maskFilename.c_str());
            }
            else
            {
               maskImage.fill(1);
            }
            
            if (all)
            {
               cimg_forXYZ(scanImage, x, y, z)
                  segmentation(x, y, z) = (scanImage(x, y, z) > t && maskImage(x, y));
            }
            else
            {
               cimg_forXY(scanImage, x, y)
                  segmentation(x, y, slice) = (scanImage(x, y, slice) > t && maskImage(x, y) );
            }

            redraw = true;
            disp.flush();

            windowSet.show();
         }
         break;

      case cimg::keyY:
         {
            cimg_for_insideXYZ(segmentation, x, y, z, 1)
            {
               if (segmentation(x-1,y,z) == segmentation(x+1,y,z) && segmentation(x,y,z) != segmentation(x+1,y,z))
                  segmentation(x,y,z) = segmentation(x+1,y,z);
               else if (segmentation(x,y-1,z) == segmentation(x,y+1,z) && segmentation(x,y,z) != segmentation(x,y+1,z))
                  segmentation(x,y,z) = segmentation(x,y+1,z);
               else if (segmentation(x,y,z-1) == segmentation(x,y,z+1) && segmentation(x,y,z) != segmentation(x,y,z+1))
                  segmentation(x,y,z) = segmentation(x,y,z+1);
            }

            redraw = true;
         }
         disp.flush();
         break;

      case cimg::keyARROWUP:
         {
            disp.wheel = 1;
            redraw = true;
         }
         break;

      case cimg::keyARROWDOWN:
         {
            disp.wheel = -1;
            redraw = true;
         }
         break;
      
      case cimg::keyARROWLEFT:
         {
            toolbox->setCurrentToolIndex(toolbox->getCurrentToolIndex() - 1 + toolbox->getNumberOfTools());
            redraw = true;
            disp.flush();
         }
         break;

      case cimg::keyARROWRIGHT:
         {
            toolbox->setCurrentToolIndex(toolbox->getCurrentToolIndex() + 1 + toolbox->getNumberOfTools());
            redraw = true;
            disp.flush();
         }
         break;

      case cimg::key1:
         {
            erode(segmentation);
            redraw = true;
            disp.flush();
         }
         break;

      case cimg::key2:
         {
            dilate(segmentation);
            redraw = true;
            disp.flush();
         }
         break;
      
      case cimg::keyA:
         {
            saveFile(segmentation, false);
         }
         break;

      case cimg::keyS:
         {
            windowSet.hide();
            
            saveFile(segmentation);
            disp.flush();

            windowSet.show();
         }
         break;

      case cimg::keyG:
         {
            std::stringstream sstr;

            system("SegmentationToVTK Segmentation.rle 1 1 1 1 m Segmentation.vtk");
            system("DisplayPolyData Segmentation.vtk");
         }
         break;

      case cimg::keyL:
         {
            windowSet.hide();

            bool isOK;
            SegmentationImage newseg = loadSegmentation(isOK);
            disp.show();

            if (isOK)
            {
               std::cerr << "Seems OK..." << std::endl;
               if (newseg.dimx() == segmentation.dimx() && newseg.dimy() == segmentation.dimy() &&
                  newseg.dimz() == segmentation.dimz() && newseg.dimv() == segmentation.dimv())
               {
                  std::cerr << "Copying over." << std::endl;
                  segmentation = newseg;
                  redraw = true;
               }
               else
               {
                  std::cerr << "Images not same size!" << std::endl;
                  std::cerr << "Current Image: " << segmentation.dimx() << " x " << segmentation.dimy() << " x " << segmentation.dimz() << " x " << segmentation.dimv() << std::endl;
                  std::cerr << "Loaded Image : " << newseg.dimx() << " x " << newseg.dimy() << " x " << newseg.dimz() << " x " << newseg.dimv() << std::endl;
               }
            }
            disp.flush();

            windowSet.show();
         }
         break;

      case cimg::keySPACE:
         {
            displaySegmentation ^= true;
            redraw = true;
            disp.flush();
         }
         break;

      case cimg::keyTAB:
         {
            drawLegend ^= true;
            redraw = true;
            disp.flush();
         }
         break;

      case cimg::keyJ:
         {
            sliceMoving ^= true;
            redraw = true;
            disp.flush();
         }
         break;
      }

      if (disp.wheel != 0)
      {
         SliceChangeEvent* sce = new SliceChangeEvent();

         const int delta = disp.wheel;
         int nextSlice = std::min<int>(sliceMax, std::max<int>(sliceMin, slice + delta));
         
         sce->init(&slice, nextSlice);
         commandManager.execute(sce);

         disp.wheel = 0;
      }

      if (disp.is_resized)
      {
         float iaspect = TYPED_DIV(float, outputImage.width,  outputImage.height);
         float waspect = TYPED_DIV(float, disp.window_dimx(), disp.window_dimy());

         // Keep the aspect ratio
         if (iaspect > waspect)
         {
            disp.resize(disp.window_dimx(), (int)(disp.window_dimx() / iaspect));
         }
         else if (iaspect < waspect)
         {
            disp.resize((int)(disp.window_dimy() * iaspect), disp.window_dimy());
         }
         else
         {
            disp.resize();
         }
      }

      // This conversion takes fence posting into account properly 
      const int mx = static_cast<const int>((disp.mouse_x + 0.5f) * TYPED_DIV(float, segmentation.dimx(), disp.window_dimx()) - 0.5f);
      const int my = static_cast<const int>((disp.mouse_y + 0.5f) * TYPED_DIV(float, segmentation.dimy(), disp.window_dimy()) - 0.5f);

      {
         bool toolChanged = false;

         if (currentTool != toolbox->getCurrentTool())
         {
            currentTool = toolbox->getCurrentTool();

            currentTool->activate(&tempImage);
            currentTool->setScanImage(&scanImage);
            sliceLast = -1;
            toolChanged = true;
         }

         if ((colourChange || toolChanged) && currentTool)
         {
            currentTool->setForegroundColour(fg);
            currentTool->setBackgroundColour(bg);
         }

         if (toolChanged || slice != sliceLast)
         {
            currentTool->setCurrentImage(&segmentation);
            currentTool->onMouseMove(mx, my, slice, disp.button);
            redraw = true;
         }

         bool updateState = false;
         
         if (disp.button != lastButton)
         {
            redraw |= currentTool->onMouseButton(mx, my, slice, disp.button);
            updateState = true;

            if (!lastButton)
            {
               // Record the current slice's segmentation
               beforeEdit = segmentation.get_shared_plane(slice);
            }
            else if (!disp.button)
            {
               bool changed = false;
               cimg_forXY(beforeEdit, x, y)
               {
                  beforeEdit(x, y) ^= segmentation(x, y, slice);

                  if (beforeEdit(x, y))
                  {
                     changed = true;
                     beforeEdit(x, y) = 255;
                  }
               }

               if (changed)
               {
                  WriteRunLengthCallback<SegmentationType>* cb = new FooRunLengthCallback<SegmentationType>();
                  //runLengthEncode(beforeEdit, cb);
                  delete cb;
               }
            }
         }
         else
         {
            if (lastMouseX != mx || lastMouseY != my)
            {
               redraw |= currentTool->onMouseMove(mx, my, slice, disp.button);
               updateState = true;
            }
         }

         if (updateState)
         {
            lastButton = disp.button;
            lastMouseX = mx;
            lastMouseY = my;
         }

      }

      redraw |= toolboxGui->refresh();
      redraw |= segLutGui->refresh();
      redraw |= intWindGui->refresh();

      if (redraw)
      {
         // Make sure that the borders remain clear
         cimg_forXY(segmentation, x, y)
         {
            for (int z = 0; z < extent[4]; ++z)
               segmentation(x, y, z) = 0;

            for (int z = extent[5]+1; z < segmentation.dimz(); ++z)
               segmentation(x, y, z) = 0;
         }
         
         cimg_forXZ(segmentation, x, z)
         {
            for (int y = 0; y < extent[2]; ++y)
            {
               segmentation(x, y, z) = 0;
            }

            for (int y = extent[3]+1; y < segmentation.dimy(); ++y)
               segmentation(x, y, z) = 0;
         }

         cimg_forYZ(segmentation, y, z)
         {
            for (int x = 0; x < extent[0]; ++x)
               segmentation(x, y, z) = 0;

            for (int x = extent[1]+1; x < segmentation.dimx(); ++x)
               segmentation(x, y, z) = 0;
         }
         
         float vv;
         unsigned char col[3];

         cimg_forXY(outputImage, x, y)
         {
            xx = (x) / scaledSpacing[0];
            yy = (y) / scaledSpacing[1];
            zz = slice;

            // Scale the image values according to the current window centre/width
            // to the range [-0.5..0.5]
            vv = (scanImage.linear_pix3d(xx, yy, zz) - windowCentre + 0.f) / (windowWidth + 0.f);
            
            // Now scale and clamp to the range [0..1]
            vv = std::min<float>(std::max<float>(vv + 0.5, 0), 1);

            SegmentationType seg = displaySegmentation ? segmentation((int)xx, (int)yy, (int)zz) : 0;

            segLut.getColour(vv, seg, col);

            outputImage(x, y, 0, 0) = col[0];
            outputImage(x, y, 0, 1) = col[1];
            outputImage(x, y, 0, 2) = col[2];
         }

         if (displaySegmentation) 
         {
            cimg_forXY(tempImage, x, y)
            {
               if (tempImage(x, y))
               {
                  // 50% opacity
                  outputImage(x, y, 0, 0) = (outputImage(x, y, 0, 0) / 2) + (tempImage(x, y) / 2);
                  outputImage(x, y, 0, 1) = (outputImage(x, y, 0, 1) / 2) + (tempImage(x, y) / 2);
                  outputImage(x, y, 0, 2) = (outputImage(x, y, 0, 2) / 2) + (tempImage(x, y) / 2);
               }
            }
         }

         disp.set_title("%s: slice [%d/%d]; window [%.1f/%.1f]", filename, slice+1, sliceMax-sliceMin+1, windowCentre, windowWidth);

         if (drawLegend)
         {
            std::ostringstream buffer;
            
            buffer << "Current Tool: ";
            if (currentTool)
               buffer << currentTool->getName() << ": " << *currentTool;
            else
               buffer << "<none>";
            buffer << std::endl;

            outputImage.draw_text(buffer.str().c_str(), 0, 0, white, black, font);

            {
               unsigned char col[3];

               for (int i = 0; i < ncolours; ++i)
               {
                  segLut.getColour(1.f, i, col);   

                  int x = outputImage.dimx()-20*(ncolours-i);
                  outputImage.draw_rectangle(x, 0, x+20, 20, col);
               }

               segLut.getColour(1.f, fg[0], col);
               outputImage.draw_text("F", 5 + outputImage.dimx()-20*(ncolours-fg[0]), 0, black, col, font);

               segLut.getColour(1.f, bg[0], col);
               outputImage.draw_text("B", 5 + outputImage.dimx()-20*(ncolours-bg[0]), 10, black, col, font);
            }
         }

         outputImage.display(disp);
         sliceLast = slice;
      }

      cimg::wait(20);

      if (sliceMoving)
      {
         slice += sliceDelta;
         if (slice < sliceMin)
         {
            sliceDelta = 1;
            slice = sliceMin+1;
         }
         else if (slice > sliceMax)
         {
            sliceDelta = -1;
            slice = sliceMax-1;
         }
      }
   }

   delete segLutGui;
   delete intWindGui;

   return 0;
}
