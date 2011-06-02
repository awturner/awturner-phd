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
#ifndef __LMICP_ALLMOVABLE_H__
#define __LMICP_ALLMOVABLE_H__

#include "ObjectManagement/ManagedObject.h"

#include "vnl/vnl_least_squares_function.h"

#include "Mesh/Tuples.h"
#include "vnl/vnl_matrix_fixed.h"

#include "LMICP.h"

namespace AWT
{
   class LMICP_AllMovable : public ManagedObject, public vnl_least_squares_function
   {
   public:
      typedef ManagedAutoPointer<LMICP_AllMovable> P;

   protected:
      LMICP_AllMovable( const unsigned int nparams, const unsigned int nresiduals );
      virtual ~LMICP_AllMovable( );

   public:
      std::string getClassName( ) const;
      static P getInstance( const unsigned int nparams, const unsigned int nresiduals );

      void addPoints( Tuples<double>::P pointSet );
      void addCorrespondence( const MeshIndex psA, const MeshIndex pntA, const MeshIndex psB, const MeshIndex pntB );

      virtual void f( const vnl_vector<double>& x, vnl_vector<double>& residuals );

      virtual void debugOn( );
      virtual void debugOff( );


   protected:
      struct D;
      D* m_D;
   };
}

#endif // __LMICP_ALLMOVABLE_H__