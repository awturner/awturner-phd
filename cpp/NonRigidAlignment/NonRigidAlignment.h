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
#ifndef __NONRIGIDALIGNMENT_H__
#define __NONRIGIDALIGNMENT_H__

#include "ObjectManagement/ManagedObject.h"
#include "Mesh/MeshTypeDefs.h"

#include "MeshAlignmentDataListener.h"

#include "Mesh/Mesh.h"
#include "ControlPointSet.h"
#include "MeshAlignmentData.h"
#include "Mesh/Tuples.h"
#include "CorrespondenceFinder.h"
#include "NonRigidAlignmentListener.h"

namespace AWT
{
   // Fired when the fixed mesh is set
   enum NonRigidAlignmentSignal
   {
      NONRIGIDALIGNMENT_FIXEDMESHSET,
      NONRIGIDALIGNMENT_MOVINGMESHSET,
      NONRIGIDALIGNMENT_CORRESPONDENCESFOUND,
      NONRIGIDALIGNMENT_RIGIDPARAMETERUPDATE,
      NONRIGIDALIGNMENT_NONRIGIDPARAMETERUPDATE,
      NONRIGIDALIGNMENT_CONTROLPOINTSCHANGED,
      NONRIGIDALIGNMENT_ALIGNMENTSTART,
      NONRIGIDALIGNMENT_ALIGNMENTCOMPLETE,
   };

   template <class T>
   class NonRigidAlignment : public virtual ManagedObject, public virtual MeshAlignmentDataListener<T>
   {
   public:
      typedef ManagedAutoPointer<NonRigidAlignment<T>> P;

   protected:
      NonRigidAlignment( );

      virtual ~NonRigidAlignment( );

   protected:
      static bool badNumber( T val );

   public:
      static P getInstance( );

      void nonRigidAlignmentStep( );

      void rigidAlignmentStep( const bool allowScale );

      void updateSourcePoints( );

      void updateTargetPoints( );

      void setSourceMesh( typename Mesh<T>::P mesh );

      typename Mesh<T>::P getSourceMesh( ) const;

      void setTargetMesh( typename Mesh<T>::P mesh );

      typename Mesh<T>::P getTargetMesh( ) const;

      typename Mesh<T>::P getProjected( ) const;

      typename ControlPointSet<T>::P getControlPoints( );

      void setControlPoints( typename ControlPointSet<T>::P cps );

      typename MeshAlignmentData<T>::P getMeshAlignment( );

      typename CorrespondenceFinder<T>::P getCorrespondences( );

      void updateCorrespondences( );

      T getRmsMove( ) const;

      T getMaxMove( ) const;

      T getRmsError( ) const;

      T getMaxError( ) const;

      void setRigidScale( const T in_Scale );

      void setRigidTransformation( const T in_Transform[3][4] );

      void getRotation( T out_Rotation[3][3] ) const;

      T getScale( ) const;

      void getTranslation( T out_Translation[3] ) const;

      void addListener( typename NonRigidAlignmentListener<T>::P nral );
      void removeListener( typename NonRigidAlignmentListener<T>::P nral );

      virtual void fixedPointsSet( typename MeshAlignmentData<T>::P mad );

      virtual void movingPointsSet( typename MeshAlignmentData<T>::P mad );

      virtual std::string getClassName( ) const;

   protected:
      struct D;

      D* m_D;
   };
}

#endif //__NONRIGIDALIGNMENT_H__