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
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <cmath>
#include "../DICOMHeader/DICOMHeaderReader.h"

#include "Useful/Iterator.h"

#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations

namespace BoostFS = boost::filesystem;

class DICOMFileIterator : public AWT::Container::Iterator<std::string>
{
public:
   DICOMFileIterator( char* path )
   {
      BoostFS::path pth( path );
      if ( !BoostFS::exists( pth ) )
      {
         m_NoMore = true;
         return;
      }

      m_Itr = BoostFS::directory_iterator( pth );

      m_NoMore    = false;
      m_FoundNext = false;
   }

   virtual std::string getClassName( ) const
   {
      return "DICOMFileIterator";
   }

   bool hasNext( )
   {
      if ( m_NoMore )    return false;
      if ( m_FoundNext ) return true;

      char magicBuffer[5];
      magicBuffer[4] = 0;

      while ( m_Itr != m_End )
      {
         if ( !BoostFS::is_directory( m_Itr->status( ) ) )
         {
            std::ostringstream fullpath;
            fullpath << m_Itr->path( );

            std::ifstream in( fullpath.str( ).c_str( ), std::ios_base::binary );
            // Check the current file for the magic number

            in.seekg( 128 );
            in.read( magicBuffer, 4 );

            in.close( );

            if ( 0 == memcmp( magicBuffer, "DICM", 4 ) )
            {
               return m_FoundNext = true;
            }

            
         }

         ++m_Itr;
      }
      
      m_NoMore = true;
      return m_FoundNext = false;
   }

   std::string next( )
   {
      if ( m_FoundNext || hasNext( ) )
      {
         std::ostringstream fullpath;
         fullpath << m_Itr->path( );

         m_FoundNext = false;
         ++m_Itr;
         return fullpath.str( );
      }

      throw "hasNext( ) == false";
   }

protected:
   bool m_NoMore;
   bool m_FoundNext;

   BoostFS::directory_iterator m_End;
   BoostFS::directory_iterator m_Itr;
};

class SliceInfoReader : public AWT::DICOMTagIdentifierConsumer
{
public:
   SliceInfoReader( )
   {
      m_TagsToFind = 6;

      m_WidthFound =
         m_HeightFound =
         m_PixelSpacingFound =
         m_SliceThicknessFound =
         m_PositionFound =
         m_OrientationFound = false;
   }

   virtual bool wantTag( AWT::DICOMTagIdentifier ge )
   {
      if ( m_TagsToFind == 0 )
         return false;

      switch ( ge.group )
      {
      case 0x0028:
         switch ( ge.element )
         {
         case 0x0010:
         case 0x0011:
         case 0x0030:
            return true;
         }
         break;
      case 0x0020:
         switch ( ge.element )
         {
         case 0x000E:
         case 0x0032:
         case 0x0037:
            return true;
         }
         break;
      case 0x0018:
         switch ( ge.element )
         {
         case 0x0050:
            return true;
         }
         break;
      }
      return false;
   }

   std::string tagName( AWT::DICOMTagIdentifier ge )
   {
      switch ( ge.group )
      {
      case 0x0028:
         switch ( ge.element )
         {
         case 0x0010:
            return std::string("Width");
         case 0x0011:
            return std::string("Height");
         case 0x0030:
            return std::string("Spacing");
         }
         break;
      case 0x0020:
         switch ( ge.element )
         {
         case 0x000E:
            return std::string("Series Instance UID");
         case 0x0032:
            return std::string("Image Position");
         case 0x0037:
            return std::string("Image Orientation");
         }
         break;
      case 0x0018:
         switch ( ge.element )
         {
         case 0x0050:
            return std::string("Slice Thickness");
         }
         break;
      }

      throw "!!";
   }

   virtual bool explicitDataCallback( AWT::DICOMTagIdentifier ge, char* vr, char* buffer, int length )
   {
      //std::cerr << tagName( ge ) << ": ";
      if ( 0 == memcmp( vr, "US", 2 ) )
      {
         unsigned short us = (buffer[0]) + (buffer[1] << 8);
         //std::cerr << us << std::endl;

         switch ( ge.group )
         {
         case 0x0028:
            switch ( ge.element )
            {
            case 0x0010:
               m_Width = us;
               m_WidthFound = true;
               --m_TagsToFind;
               break;
            case 0x0011:
               m_Height = us;
               m_HeightFound = true;
               --m_TagsToFind;
               break;
            }
            break;
         }
      }
      else if ( 0 == memcmp( vr, "DS", 2 ) )
      {
         std::stringstream str;
         int nparts = 1;

         for ( int i = 0; i < length; ++i )
         {
            str << buffer[i];
            if ( buffer[i] == '\\' )
               ++nparts;
         }

         float* parts = new float[nparts];
         char ch;

         int i;
         for ( i = 0; i < nparts; ++i )
         {
            str >> parts[i];

            if ( i+1 < nparts )
               str >> ch;
         }

         switch ( ge.group )
         {
         case 0x0018:
            switch ( ge.element )
            {
            case 0x0050:
               m_SliceThickness = parts[0];
               m_SliceThicknessFound = true;
               --m_TagsToFind;
               break;
            }
            break;
         case 0x0028:
            switch ( ge.element )
            {
            case 0x0030:
               m_PixelSpacing[0] = parts[0];
               m_PixelSpacing[1] = parts[1];
               m_PixelSpacingFound = true;
               --m_TagsToFind;
               break;
            }
            break;
         case 0x0020:
            switch ( ge.element )
            {
            case 0x0032:
               for ( i = 0; i < 3; ++i )
                  m_Position[i] = parts[i];
               m_PositionFound = true;
               --m_TagsToFind;
               break;
            case 0x0037:
               for ( i = 0; i < 6; ++i )
                  m_Orientation[i] = parts[i];
               m_OrientationFound = true;
               --m_TagsToFind;
               break;
            }
            break;
         }
      }
      else if ( 0 == memcmp( vr, "UI", 2 ) )
      {
         std::stringstream str;
         int nparts = 1;

         for ( int i = 0; i < length; ++i )
         {
            str << buffer[i];
         }

         switch ( ge.group )
         {
         case 0x0020:
            switch ( ge.element )
            {
            case 0x000E:
               m_SeriesInstanceUID = str.str( );
               m_SeriesInstanceUIDFound = true;
               break;
            }
            break;
         }
      }

      return ( m_TagsToFind > 0 );
   }

   unsigned short getWidth( ) { return m_Width; }
   unsigned short getHeight( ) { return m_Height; }

   float* getPixelSpacing( ) { return m_PixelSpacing; }
   float  getSliceThickness( ) { return m_SliceThickness; }

   float* getPosition( ) { return m_Position; }
   float* getOrientation( ) { return m_Orientation; }

   std::string getSeriesInstanceUID( ) { return m_SeriesInstanceUID; }

protected:
   unsigned short m_Width;
   unsigned short m_Height;

   float m_PixelSpacing[2];
   float m_SliceThickness;

   float m_Position[3];
   float m_Orientation[6];

   std::string m_SeriesInstanceUID;

   bool m_WidthFound;
   bool m_HeightFound;
   bool m_PixelSpacingFound;
   bool m_SliceThicknessFound;
   bool m_PositionFound;
   bool m_OrientationFound;
   bool m_SeriesInstanceUIDFound;

   int m_TagsToFind;
};

template <class T>
T min( T a, T b ) { return ( a < b ) ? a : b; }

template <class T>
T max( T a, T b ) { return ( a > b ) ? a : b; }

int main( int argc, char** argv )
{
   if ( argc < 2 )
   {
      std::cerr << "Must provide a directory argument!" << std::endl;
      DEBUGLINEANDEXIT( 1 );
   }

   DICOMFileIterator dfi(argv[1]);

   std::ofstream out("out.m");
   out << "hold on; axis equal; set(gcf, 'Renderer', 'OpenGL')" << std::endl;

   std::vector<std::string>  seriesUIDs;
   std::vector<float*>       seriesCols;
   std::vector<unsigned int> seriesCnts;
   std::vector<float*>       seriesBnds;

   while ( dfi.hasNext( ) )
   {
      std::string filename = dfi.next( );
      std::ifstream in( filename.c_str( ), std::ios_base::binary );

      SliceInfoReader        sir;
      AWT::DICOMHeaderReader::readStream( in, &sir );

      float col[3];
      int idx;
      for ( idx = 0; idx < seriesUIDs.size( ); ++idx )
      {
         if ( seriesUIDs[idx] == sir.getSeriesInstanceUID( ) )
         {
            break;
         }
      }

      if ( idx == seriesUIDs.size( ) )
      {
         seriesUIDs.push_back( sir.getSeriesInstanceUID( ) );
         seriesCnts.push_back( 0 );

         std::cerr << "Pushing back " << sir.getSeriesInstanceUID( ) << std::endl;

         float* newcol = new float[3];
         newcol[0] = (0.f + rand( )) / (0.f + RAND_MAX);
         newcol[1] = (0.f + rand( )) / (0.f + RAND_MAX);
         newcol[2] = (0.f + rand( )) / (0.f + RAND_MAX);
         seriesCols.push_back( newcol );

         float* newbnd = new float[6];
         newbnd[0] = newbnd[2] = newbnd[4] =  std::numeric_limits<float>::infinity( );
         newbnd[1] = newbnd[3] = newbnd[5] = -std::numeric_limits<float>::infinity( );
         seriesBnds.push_back( newbnd );
      }

      col[0] = seriesCols[idx][0];
      col[1] = seriesCols[idx][1];
      col[2] = seriesCols[idx][2];

      ++seriesCnts[idx];

      float* pos = sir.getPosition( );
      float* ori = sir.getOrientation( );
      float* spa = sir.getPixelSpacing( );
      float  thi = sir.getSliceThickness( );

      unsigned short w = sir.getWidth( );
      unsigned short h = sir.getHeight( );

      //   i       j       k
      // ori[0]  ori[1]  ori[2]
      // ori[3]  ori[4]  ori[5]

      float orientation[9];
      for ( int i = 0; i < 6; ++i )
         orientation[i] = ori[i];

      orientation[6] = ori[1]*ori[5] - ori[2]*ori[4];
      orientation[7] = ori[3]*ori[2] - ori[0]*ori[5];
      orientation[8] = ori[0]*ori[4] - ori[3]*ori[1];

      //out << "pt = [" << std::endl;
      for ( int i = 0; i < 8; ++i )
      {
         float x = (((i&1)?w:0) - 0.5f) * spa[0];
         float y = (((i&2)?h:0) - 0.5f) * spa[1];
         float z = (((i&4)?1:0) - 0.5f) *    thi;

         float xx = x*orientation[0] + y*orientation[3] + z*orientation[6] + pos[0];
         float yy = x*orientation[1] + y*orientation[4] + z*orientation[7] + pos[1];
         float zz = x*orientation[2] + y*orientation[5] + z*orientation[8] + pos[2];

         seriesBnds[idx][0] = min( seriesBnds[idx][0], xx );
         seriesBnds[idx][1] = max( seriesBnds[idx][1], xx );
         seriesBnds[idx][2] = min( seriesBnds[idx][2], yy );
         seriesBnds[idx][3] = max( seriesBnds[idx][3], yy );
         seriesBnds[idx][4] = min( seriesBnds[idx][4], zz );
         seriesBnds[idx][5] = max( seriesBnds[idx][5], zz );

         //out << xx << " " << yy << " " << zz << std::endl;
      }
      //out << "]'; h = drawCube( pt ); set(h, 'Color', [" << col[0] << " " << col[1] << " " << col[2] << "]);" << std::endl;
   }

   for ( int i = 0; i < seriesUIDs.size( ); ++i )
   {
      float* col = seriesCols[i];
      float* bnds = seriesBnds[i];

      out << "pt = [" << std::endl;
      out << "\t" << bnds[0] << " " << bnds[2] << " " << bnds[4] << std::endl;
      out << "\t" << bnds[1] << " " << bnds[2] << " " << bnds[4] << std::endl;
      out << "\t" << bnds[0] << " " << bnds[3] << " " << bnds[4] << std::endl;
      out << "\t" << bnds[1] << " " << bnds[3] << " " << bnds[4] << std::endl;
      out << "\t" << bnds[0] << " " << bnds[2] << " " << bnds[5] << std::endl;
      out << "\t" << bnds[1] << " " << bnds[2] << " " << bnds[5] << std::endl;
      out << "\t" << bnds[0] << " " << bnds[3] << " " << bnds[5] << std::endl;
      out << "\t" << bnds[1] << " " << bnds[3] << " " << bnds[5] << std::endl;
      out << "]';  h = drawCube( pt ); set(h, 'Color', [" << col[0] << " " << col[1] << " " << col[2] << "]);" << std::endl;
      out << "\t text( " << bnds[0] << "," << bnds[1] << "," << bnds[2] << ", '" << seriesUIDs[i] << "' );" << std::endl;
      out << "\tpause;" << std::endl;
      std::cerr << seriesUIDs[i] << " " << seriesCnts[i] << std::endl;
   }

   out.close( );

   
}