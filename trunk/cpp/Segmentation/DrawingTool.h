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
#ifndef __DRAWINGTOOL_H__
#define __DRAWINGTOOL_H__

namespace cimg_library
{
   template <class T>
   class CImg;
}

using namespace cimg_library;

#include <ostream>
#include "Useful/Exception.h"

#include "CommandManager.h"

namespace AWT
{
   template <class T, class ScanT, unsigned int V>
   class DrawingTool
   {
   public:
      DrawingTool(CommandManager* commandManager);

      // The image which this tool can draw on temporarily
      CImg<T>* getOverlayImage();
      void setOverlayImage(CImg<T>* im);

      // The image which this tool will operate on
      CImg<T>* getCurrentImage();
      virtual void setCurrentImage(CImg<T>* im);

      // The scan image
      CImg<ScanT>* getScanImage();
      virtual void setScanImage(CImg<ScanT>* im);

      // The foreground and background colours
      void getForegroundColour(T* col);
      void setForegroundColour(T* col);

      void getBackgroundColour(T* col);
      void setBackgroundColour(T* col);

      virtual char* getName() const = 0;

      virtual void printState(std::ostream& os) const
      {
         os << "OK";
      }

      bool activate(CImg<T>* overlayImage, CImg<T>* currentImage = 0) {
         setOverlayImage(overlayImage);

         if (currentImage)
            setCurrentImage(currentImage);

         return onActivate();
      }

      virtual bool onMouseButton(const int x, const int y, const int z, const int button)
      {
         return false;
      }

      virtual bool onMouseMove(const int x, const int y, const int z, const int button)
      {
         return false;
      }

      virtual bool onActivate()
      {
         if (m_OverlayImage)
            m_OverlayImage->fill(0);

         return true;
      }
      
      static friend std::ostream& operator<<(std::ostream& os, DrawingTool& rhs)
      {
         rhs.printState(os);
         return os;
      }

   protected:
      void copyArray(T* src, T* dst, int n);

      CImg<ScanT>*          m_ScanImage;
      CImg<T>*              m_CurrentImage;
      CImg<T>*              m_OverlayImage;
      T                     m_Foreground[V];
      T                     m_Background[V];

      const CommandManager* m_CommandManager;
   };
}

#include <CImg.h>

using namespace AWT;

template <class T, class ScanT, unsigned int V>
AWT::DrawingTool<T,ScanT,V>::DrawingTool(AWT::CommandManager* _com)
: m_CommandManager(_com)
{
   m_CurrentImage = m_OverlayImage = 0;
}

template <class T, class ScanT, unsigned int V>
CImg<T>* AWT::DrawingTool<T,ScanT,V>::getOverlayImage()
{
   return m_OverlayImage;
}

template <class T, class ScanT, unsigned int V>
void AWT::DrawingTool<T,ScanT,V>::setOverlayImage(CImg<T> *im)
{
   if (im != m_OverlayImage)
   {
      m_OverlayImage = im;
   }
}

template <class T, class ScanT, unsigned int V>
CImg<ScanT>* AWT::DrawingTool<T,ScanT,V>::getScanImage()
{
   return m_ScanImage;
}

template <class T, class ScanT, unsigned int V>
void AWT::DrawingTool<T,ScanT,V>::setScanImage(CImg<ScanT>* im)
{
   if (im != m_ScanImage)
   {
      m_ScanImage = im;
   }
}

template <class T, class ScanT, unsigned int V>
CImg<T>* AWT::DrawingTool<T,ScanT,V>::getCurrentImage()
{
   return m_CurrentImage;
}

template <class T, class ScanT, unsigned int V>
void AWT::DrawingTool<T,ScanT,V>::setCurrentImage(CImg<T> *im)
{
   if (im != m_CurrentImage)
   {
      if (im->dimv() != V)
         AWTEXCEPTIONTHROW("Image has wrong number of planes!");

      m_CurrentImage = im;

   }
}

template <class T, class ScanT, unsigned int V>
void AWT::DrawingTool<T,ScanT,V>::copyArray(T* src, T* dst, int n)
{
   while (--n >= 0)
      *dst++ = *src++;
}

template <class T, class ScanT, unsigned int V>
void AWT::DrawingTool<T,ScanT,V>::getBackgroundColour(T *col)
{
   copyArray(m_Background, col, V);
}

template <class T, class ScanT, unsigned int V>
void AWT::DrawingTool<T,ScanT,V>::getForegroundColour(T *col)
{
   copyArray(m_Foreground, col, V);
}

template <class T, class ScanT, unsigned int V>
void AWT::DrawingTool<T,ScanT,V>::setBackgroundColour(T *col)
{
   copyArray(col, m_Background, V);
}

#include <iostream>

template <class T, class ScanT, unsigned int V>
void AWT::DrawingTool<T,ScanT,V>::setForegroundColour(T *col)
{
   copyArray(col, m_Foreground, V);
}


#endif // __DRAWINGTOOL