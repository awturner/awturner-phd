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
#ifndef __SAMPLEDTUPLES_H__
#define __SAMPLEDTUPLES_H__

#include "Tuples.h"
#include "SamplePoints.h"

namespace AWT
{
   template <class T>
   class SampledTuples : public Tuples<T>
   {
   public:
      typedef ManagedAutoPointer<SampledTuples<T> > P;

   protected:
      SampledTuples(typename Tuples<T>::P tuples, MeshIndex npts)
      {
         m_Tuples = tuples;

         m_SamplePoints = SamplePoints::getInstance(npts);
      }

      SampledTuples(typename Tuples<T>::P tuples, typename SamplePoints::P sampPts)
      {
         m_Tuples = tuples;

         m_SamplePoints = sampPts;
      }
      
      virtual ~SampledTuples()
      {
      }

   public:
      static typename SampledTuples<T>::P getInstance(typename Tuples<T>::P tuples, SamplePoints::P sampPts)
      {
         AUTOGETINSTANCE(SampledTuples<T>, (tuples, sampPts));
      }

      static typename SampledTuples<T>::P getInstance(typename Tuples<T>::P tuples, MeshIndex npts)
      {
         AUTOGETINSTANCE(SampledTuples<T>, (tuples, npts));
      }

      virtual std::string getClassName() const
      {
         return "AWT::SampledTuples<T>";
      }

      virtual ModifiedTime getModifiedTime() const
      {
         ModifiedTime ret = Tuples<T>::getModifiedTime();

         ret = std::max<ModifiedTime>(ret, m_Tuples->getModifiedTime());
         ret = std::max<ModifiedTime>(ret, m_SamplePoints->getModifiedTime());

         return ret;
      }

      void addSamplePoint(MeshIndex i)
      {
         m_SamplePoints->add(i);
         this->modified();
      }

      TupleIndex getTupleSize() const
      {
         return m_Tuples->getTupleSize();
      }

      MeshIndex getNumberOfPoints() const
      {
         return m_SamplePoints->getNumberOfSamples();
      }

      T    getPointElement(const MeshIndex in_Index, const TupleIndex i) const
      {
         MeshIndex mapIdx = m_SamplePoints->getSampleIndex(in_Index);

         return m_Tuples->getPointElement(mapIdx, i);
      }

      void getPoint(const MeshIndex in_Index, T* out_Point) const
      {
         MeshIndex mapIdx = m_SamplePoints->getSampleIndex(in_Index);

         m_Tuples->getPoint(mapIdx, out_Point);
      }

      virtual void addPoint(const T* in_Point)
      {
         setPoint(getNumberOfPoints(), in_Point);
      }

      void setPointElement(const MeshIndex in_Index, const TupleIndex i, const T in_Value)
      {
         MeshIndex mapIdx = m_SamplePoints->getSampleIndex(in_Index);

         m_Tuples->setPointElement(mapIdx, i, in_Value);
         this->modified();
      }

      void setPoint(const MeshIndex in_Index, const T* in_Point)
      {
         MeshIndex mapIdx = m_SamplePoints->getSampleIndex(in_Index);

         m_Tuples->setPoint(mapIdx, in_Point);
         this->modified();
      }

      void ensureSize(MeshIndex size)
      {
         m_Tuples->ensureSize(size);
         m_SamplePoints->ensureSize(size);
         this->modified();
      }

      void clear()
      {
         m_SamplePoints->clear();
      }

      void lock(void* obj)
      {
         m_Tuples->lock(obj);
      }

      void unlock(void* obj)
      {
         m_Tuples->unlock(obj);
      }

      bool isLocked() const
      {
         return m_Tuples->isLocked();
      }

      SamplePoints* getSamplePoints() const
      {
         return m_SamplePoints;
      }

      const T* getDefault() const
      {
         return m_Tuples->getDefault();
      }

   protected:
      typename Tuples<T>::P m_Tuples;
      SamplePoints::P       m_SamplePoints;
   };
}

#endif // __SAMPLEDTUPLES_H__