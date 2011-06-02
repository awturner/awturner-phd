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
#include "CImg.h"

#include <iostream>
#include <limits>
#include <queue>
#include <string>

using namespace cimg_library;

typedef unsigned char Label;
typedef float Distance;
typedef float Force;

typedef CImg<Label>    LabelImage;
typedef CImg<Distance> DistanceImage;
typedef CImg<Force>    ForceImage;

const Label LABEL_FAR    = 0;
const Label LABEL_TRIAL  = 1;
const Label LABEL_ACTIVE = 2;

template <class T>
struct Point3D
{
   Point3D() { }
   Point3D(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

   T x;
   T y;
   T z;
};

Distance calculateNewDistanceCase(const Distance* us, const Distance* spacing2, const Force s, const bool* useCases)
{
   Distance a = 0;
   Distance b = 0;
   Distance c = 0;

   for (int d = 0; d < 3; ++d)
   {
      if (!useCases[d]) continue;

      a += 1 / spacing2[d];
      b += us[d] / spacing2[d];
      c += us[d]*us[d] / spacing2[d];
   }

   b *= -2;
   c -= s*s;

   Distance det = b*b - 4*a*c;

   if (det >= 0)
   {
      return (-b + sqrt(det)) / (2*a);
   }
   else
   {
      return std::numeric_limits<Distance>::infinity();
   }
}

Distance calculateNewDistance(const Distance* us, const Distance* spacing, const Force s)
{
   const Distance spacing2[] = { spacing[0]*spacing[0], spacing[1]*spacing[1], spacing[2]*spacing[2] };

   bool useCases[] = { 
      true, true, true,    // All three
      true, true, false,   // Just two
      true, false, true,
      false, true, true,
      true, false, false,  // Just one
      false, true, false,
      false, false, true,
   };

   Distance u;

   // Firstly, try the "all three" case
   u = calculateNewDistanceCase(us, &spacing2[0], s, &useCases[ 0]);
   if (u != std::numeric_limits<Distance>::infinity())
      return u;

   // Now try the "pick two" cases
   u = std::numeric_limits<Distance>::infinity();
   u = std::min(u, calculateNewDistanceCase(us, &spacing2[0], s, &useCases[ 3]));
   u = std::min(u, calculateNewDistanceCase(us, &spacing2[0], s, &useCases[ 6]));
   u = std::min(u, calculateNewDistanceCase(us, &spacing2[0], s, &useCases[ 9]));
   if (u != std::numeric_limits<Distance>::infinity())
      return u;

   // Now try the "pick one" cases
   u = std::numeric_limits<Distance>::infinity();
   u = std::min(u, calculateNewDistanceCase(us, &spacing2[0], s, &useCases[12]));
   u = std::min(u, calculateNewDistanceCase(us, &spacing2[0], s, &useCases[15]));
   u = std::min(u, calculateNewDistanceCase(us, &spacing2[0], s, &useCases[18]));

   return u;
}

void update(DistanceImage& dist, LabelImage& label, ForceImage& force, Point3D<int>& p, std::vector<int>& trialPoints)
{
   if (!dist.contains(p.x, p.y, p.z) || label(p.x, p.y, p.z) == LABEL_ACTIVE)
      return;

   const int w = dist.dimx();
   const int h = dist.dimy();
   const int d = dist.dimz();

   Distance ux, uy, uz, u;

   // Get the x-neighbour's distance
   if (p.x > 0 && p.x < w-1)
      ux = std::min<Distance>(dist(p.x-1, p.y, p.z), dist(p.x+1, p.y, p.z));
   else if (p.x > 0)
      ux = dist(p.x-1, p.y, p.z);
   else if (p.x < w-1)
      ux = dist(p.x+1, p.y, p.z);
   else
      ux = std::numeric_limits<Distance>::infinity();
  
   // Get the y-neighbour's distance
   if (p.y > 0 && p.y < h-1)
      uy = std::min<Distance>(dist(p.x, p.y-1, p.z), dist(p.x, p.y+1, p.z));
   else if (p.y > 0)
      uy = dist(p.x, p.y-1, p.z);
   else if (p.y < h-1)
      uy = dist(p.x, p.y+1, p.z);
   else
      uy = std::numeric_limits<Distance>::infinity();

   // Get the z-neighbour's distance
   if (p.z > 0 && p.z < d-1)
      uz = std::min<Distance>(dist(p.x, p.y, p.z-1), dist(p.x, p.y, p.z+1));
   else if (p.z > 0)
      uz = dist(p.x, p.y, p.z-1);
   else if (p.z < d-1)
      uz = dist(p.x, p.y, p.z+1);
   else
      uz = std::numeric_limits<Distance>::infinity();

   const Distance spacing[] = { 1, 1, 1 };
   const Distance distances[] = { ux, uy, uz };

   u = calculateNewDistance(&distances[0], &spacing[0], force(p.x, p.y, p.z));

   // Let's see if the distance has decreased
   if (dist(p.x, p.y, p.z) > u)
   {
      dist(p.x, p.y, p.z) = u;

      if (label(p.x, p.y, p.z) != LABEL_TRIAL)
      {
         // The point isn't already in our priority queue...
         // Add this to our list of points to be checked
         label(p.x, p.y, p.z) = LABEL_TRIAL;

         int idx = p.x + dist.dimx()*p.y + dist.dimx()*dist.dimy()*p.z;

         //std::cerr << "Pushing (" << p.x << ", " << p.y << ", " << p.z << ") -> " << idx << std::endl;

         // Push the index onto the back of the list of trial points
         trialPoints.push_back(idx);
      }
      else
      {
         // The point is already in the queue somewhere...
      }
   }
}

void backtrace(const CImg<Distance>& dist, const Point3D<Distance>& start, const Point3D<Distance>& target, std::vector<Point3D<Distance> >& nodes)
{
   Point3D<Distance> p = start;

   Distance dx, dy, dz;

   CImg<Distance> I(3, 3, 3, 1);

   Distance alpha, beta, gamma;
   Distance vs[8];

   unsigned int nx, px, ny, py, nz, pz;

   int iter = 0;
   while (abs(p.x - target.x) > 0.5 || abs(p.y - target.y) > 0.5 || abs(p.z - target.z) > 0.5)
   {
      nodes.push_back(p);

      // These are the coordinates
      nx = static_cast<unsigned int>(floor(p.x)); px = nx + 1;
      ny = static_cast<unsigned int>(floor(p.y)); py = ny + 1;
      nz = static_cast<unsigned int>(floor(p.z)); pz = nz + 1;

      if (nx < 0) nx == 0;
      if (ny < 0) ny == 0;
      if (nz < 0) nz == 0;

      if (px >= dist.dimx()) px = dist.dimx() - 1;
      if (py >= dist.dimy()) py = dist.dimy() - 1;
      if (pz >= dist.dimz()) pz = dist.dimz() - 1;

      // These are the fractional parts of the position
      alpha = p.x - floor(p.x);
      beta  = p.y - floor(p.y);
      gamma = p.z - floor(p.z);

      // These are the distance values at the corners of the cell
      vs[0] = dist(nx, ny, nz);
      vs[1] = dist(px, ny, nz);
      vs[2] = dist(nx, py, nz);
      vs[3] = dist(px, py, nz);
      vs[4] = dist(nx, ny, pz);
      vs[5] = dist(px, ny, pz);
      vs[6] = dist(nx, py, pz);
      vs[7] = dist(px, py, pz);

      dx = (1-gamma) * (
               +(1-beta) * (vs[1] - vs[0])
               + beta    * (vs[3] - vs[2])
        ) + gamma * (
               +(1-beta) * (vs[5] - vs[4])
               + beta    * (vs[7] - vs[6])
        );

      dy = (1-gamma) * (
               +(1-alpha) * (vs[2] - vs[0])
               + alpha    * (vs[3] - vs[1])
        ) + gamma * (
               +(1-alpha) * (vs[6] - vs[4])
               + alpha    * (vs[7] - vs[5])
        );

      dz = 
         -(1-alpha)*(1-beta) * vs[0]
         - alpha   *(1-beta) * vs[1]
         -(1-alpha)* beta    * vs[2]
         - alpha   * beta    * vs[3]
         +(1-alpha)*(1-beta) * vs[4]
         + alpha   *(1-beta) * vs[5]
         +(1-alpha)* beta    * vs[6]
         + alpha   * beta    * vs[7];

      Distance norm = sqrt(dx*dx + dy*dy + dz*dz) * 10;

      p.x -= dx / norm;
      p.y -= dy / norm;
      p.z -= dz / norm;

      p.x = std::min<Distance>(std::max<Distance>(0, p.x), static_cast<Distance>(dist.dimx()-1));
      p.y = std::min<Distance>(std::max<Distance>(0, p.y), static_cast<Distance>(dist.dimy()-1));
      p.z = std::min<Distance>(std::max<Distance>(0, p.z), static_cast<Distance>(dist.dimz()-1));
      
      if (++iter > 10000)
      {
         iter = 0;

         // Give it a boot
         p.x += (rand() + 0.0) / (RAND_MAX + 0.0) - 0.5;
         p.y += (rand() + 0.0) / (RAND_MAX + 0.0) - 0.5;

         //std::cerr << norm << std::endl;
         //break;
      }
   }
}

void initialize(DistanceImage& distanceImage, LabelImage& labelImage, ForceImage& forceImage, Point3D<Distance>& pinit, std::vector<int>& trialPoints)
{
   // Make everything far far away
   distanceImage.fill(std::numeric_limits<Distance>::infinity());
   labelImage.fill(LABEL_FAR);

   // ... except the starting position
   distanceImage(pinit.x, pinit.y, pinit.z) = 0;
   labelImage(pinit.x, pinit.y, pinit.z) = LABEL_ACTIVE;

   Point3D<int> p;

   // Update the neighbours of the starting point as trial
   p.x = pinit.x-1; p.y = pinit.y; p.z = pinit.z; update(distanceImage, labelImage, forceImage, p, trialPoints);
   p.x = pinit.x+1; p.y = pinit.y; p.z = pinit.z; update(distanceImage, labelImage, forceImage, p, trialPoints);

   p.x = pinit.x; p.y = pinit.y-1; p.z = pinit.z; update(distanceImage, labelImage, forceImage, p, trialPoints);
   p.x = pinit.x; p.y = pinit.y+1; p.z = pinit.z; update(distanceImage, labelImage, forceImage, p, trialPoints);

   p.x = pinit.x; p.y = pinit.y; p.z = pinit.z-1; update(distanceImage, labelImage, forceImage, p, trialPoints);
   p.x = pinit.x; p.y = pinit.y; p.z = pinit.z+1; update(distanceImage, labelImage, forceImage, p, trialPoints);
}

void fastMarch(DistanceImage& distanceImage, LabelImage& labelImage, ForceImage& forceImage, std::vector<int>& trialPoints)
{
   Point3D<int> p(-1, -1, -1);
   
   int trialPointsPointer = 0;
   int x, y, z, idx;

   const int width = distanceImage.dimx();
   const int area  = distanceImage.dimy() * width;

   for (unsigned int i = trialPointsPointer; i < trialPoints.size(); ++i)
   {
      idx = trialPoints[i];
      x = idx % width;
      y = (idx % area) / width;
      z = idx / (area);

      labelImage(x, y, z) = LABEL_TRIAL;
   }

   for (int iters = 0; ; ++iters)
   {
      p.x = -1; p.y = -1; p.z = -1;
      int trialCount = 0;
      Distance bestDistance = std::numeric_limits<Distance>::infinity();

      // This could be done faster using a priority queue... but this is
      // just about fast enough anyway
      {
         int idx, x, y, z;
         const int width = distanceImage.dimx();
         const int area  = distanceImage.dimy() * width;

         int bestIdx = -1;
         for (unsigned int i = trialPointsPointer; i < trialPoints.size(); ++i)
         {
            idx = trialPoints[i];
            x = idx % width;
            y = (idx % area) / width;
            z = idx / (area);

            if (labelImage(x, y, z) == LABEL_TRIAL)
            {
               ++trialCount;

               if (distanceImage(x, y, z) < bestDistance)
               {
                  bestDistance = distanceImage(p.x = x, p.y = y, p.z = z);
                  bestIdx = i;
               }
            }
         }

         if (bestIdx == -1)
            break;

         int tmp = trialPoints[trialPointsPointer];
         trialPoints[trialPointsPointer] = trialPoints[bestIdx];
         trialPoints[bestIdx] = tmp;

         ++trialPointsPointer;
      }

      // No point was found, so break
      if (p.x == -1)
         break;

      labelImage(p.x, p.y, p.z) = LABEL_ACTIVE;

      int poss[6][3] = {
         { p.x-1, p.y, p.z },
         { p.x+1, p.y, p.z },
         { p.x, p.y-1, p.z },
         { p.x, p.y+1, p.z },
         { p.x, p.y, p.z-1 },
         { p.x, p.y, p.z+1 },
      };

      for (int neigh = 0; neigh < 6; ++neigh)
      {
         p.x = poss[neigh][0]; p.y = poss[neigh][1]; p.z = poss[neigh][2];
         update(distanceImage, labelImage, forceImage, p, trialPoints);
      }
   }
}

void smoothForceImage(ForceImage& origForce, Distance offset, Distance minRadius)
{
   const int w = origForce.dimx();
   const int h = origForce.dimy();
   const int d = origForce.dimz();

   // Minimum radius = inf (origForce) / sup(|| grad origForce ||)
   Force desiredGrad = std::max<Force>(0, (offset + origForce.min()) / minRadius);

   std::cerr << "Desired grad = " << desiredGrad << std::endl;

   ForceImage forceImage(w, h, d, 1);
   // This parameter depends on the min desired radius
   forceImage.fill(desiredGrad);

   LabelImage labelImage(w, h, d, 1);
   DistanceImage distanceImage(w, h, d, 1);

   // Flip it so that the largest force gets assigned 
   // the zero distance
   distanceImage = origForce.max() - offset - origForce;

   std::vector<int> trialPoints;

   // Initialize the label image - flag any point on the inside of a boundary as "trial"
   labelImage.fill(LABEL_FAR);
   cimg_forXYZ(labelImage, x, y, z)
   {
      bool flag = false;

      if (origForce.contains(x-1,y,z) && origForce(x,y,z) != origForce(x-1,y,z))
         flag = true;
      else if (!flag && origForce.contains(x+1,y,z) && origForce(x,y,z) > origForce(x+1,y,z))
         flag = true;
      else if (!flag && origForce.contains(x,y-1,z) && origForce(x,y,z) > origForce(x,y-1,z))
         flag = true;
      else if (!flag && origForce.contains(x,y+1,z) && origForce(x,y,z) > origForce(x,y+1,z))
         flag = true;
      else if (!flag && origForce.contains(x,y,z-1) && origForce(x,y,z) > origForce(x,y,z-1))
         flag = true;
      else if (!flag && origForce.contains(x,y,z+1) && origForce(x,y,z) > origForce(x,y,z+1))
         flag = true;

      if (flag)
      {
         labelImage(x, y, z) = LABEL_TRIAL;
         trialPoints.push_back(x + w*y + w*h*z);
      }
   }

   fastMarch(distanceImage, labelImage, forceImage, trialPoints);

   // Copy the distance image back into the force image
   origForce = origForce.max() - distanceImage;
}

int main(int argc, char** argv)
{
   if (argc < 2)
   {
      std::cerr << "Must provide force filename!" << std::endl;
      exit(1);
   }

   ForceImage    origForceImage;
   origForceImage.load(argv[1]);

   ForceImage    forceImage = origForceImage;
   Distance maxx = origForceImage.max();
   Distance minn = origForceImage.min();

   //smoothForceImage(forceImage, static_cast<Distance>(1), static_cast<Distance>(0));

   CImg<unsigned char> forceAppearance(forceImage.dimx(), forceImage.dimy(), 1, 3);

   if (argc > 2)
   {
      forceAppearance.load(argv[2]);

      if (argc > 3)
      {
         smoothForceImage(forceImage, static_cast<Distance>(atof(argv[3])), static_cast<Distance>(atof(argv[4])));
      }
   }
   else
   {
      cimg_forXYV(forceAppearance, x, y, v)
         forceAppearance(x, y, 0, v) = static_cast<unsigned char>((origForceImage(x, y, 0, 0) - minn) / (maxx - minn) * 255);
   }

   const int w = forceImage.dimx();
   const int h = forceImage.dimy();
   const int d = forceImage.dimz();

   LabelImage    labelImage(w, h, d);
   DistanceImage distanceImage(w, h, d);

   Distance xinit = 0;
   Distance yinit = 0;
   Distance zinit = 0;

   std::vector<int> trialPoints;
   initialize(distanceImage, labelImage, forceImage, Point3D<Distance>(xinit, yinit, zinit), trialPoints);
   fastMarch(distanceImage, labelImage, forceImage, trialPoints);

   CImg<unsigned char> outputImage(distanceImage.dimx(), distanceImage.dimy(), 1, 3);
   outputImage.fill(0);

   CImgDisplay disp(outputImage, "Distance");

   int lastX = -1, lastY = -1;
   bool redraw = true;

   while (!disp.is_closed)
   {
      if (disp.button != 0)
      {
         xinit = static_cast<Distance>(disp.mouse_x);
         yinit = static_cast<Distance>(disp.mouse_y);
         zinit = 0;

         trialPoints.clear();
         initialize(distanceImage, labelImage, forceImage, Point3D<Distance>(xinit, yinit, zinit), trialPoints);
         fastMarch(distanceImage, labelImage, forceImage, trialPoints);
         
         disp.button = 0;

         redraw = true;
      }

      if (disp.mouse_x != lastX || disp.mouse_y != lastY)
      {
         redraw = true;
      }

      if (redraw)
      {
         cimg_forXYV(outputImage, x, y, v)
            outputImage(x, y, 0, v) = forceAppearance(x, y, 0, v);
            //outputImage(x, y, 0, v) = static_cast<unsigned char>((origForceImage(x, y, 0, 0) - minn) / (maxx - minn) * 255);

         int xx = disp.mouse_x, yy = disp.mouse_y;

         std::vector<Point3D<Distance> > trace;

         Point3D<Distance> start(disp.mouse_x, disp.mouse_y, 0);
         Point3D<Distance> target(xinit, yinit, zinit);

         backtrace(distanceImage, start, target, trace);
         
         for (unsigned int i = 0; i < trace.size(); ++i)
         {
            int x = static_cast<int>(0.5f + trace[i].x);
            int y = static_cast<int>(0.5f + trace[i].y);
            int z = static_cast<int>(0.5f + trace[i].z);

            for (int yy = std::max(y-1,0); yy <= std::min(y+1, outputImage.dimy()); ++yy)
               for (int xx = std::max(x-1,0); xx <= std::min(x+1, outputImage.dimx()); ++xx)
                  for (int v = 0; v < 3; ++v)
                     outputImage(xx,yy,0,v) = outputImage(xx,yy,0,v) / 2;

         }

         for (unsigned int i = 0; i < trace.size(); ++i)
         {
            int x = static_cast<int>(0.5f + trace[i].x);
            int y = static_cast<int>(0.5f + trace[i].y);
            int z = static_cast<int>(0.5f + trace[i].z);

            outputImage(x, y, z, 0) = 255;
            outputImage(x, y, z, 1) =   0;
            outputImage(x, y, z, 2) = 255;
         }

         outputImage(static_cast<unsigned int>(xinit), static_cast<unsigned int>(yinit), 0, 0) = 255;
         outputImage(disp.mouse_x, disp.mouse_y, 0, 0) = 255;
         
         outputImage.display(disp);
      }

      if (disp.key == cimg::keyS)
      {
         std::string filename;
         std::cerr << "Enter a filename: ";
         std::cin >> filename;

         outputImage.save_bmp(filename.c_str());
         disp.flush();
      }

      cimg::wait(20);
      redraw = false;
   }
}
