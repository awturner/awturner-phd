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
#ifndef __SIMPLEMESHSEARCH_H__
#define __SIMPLEMESHSEARCH_H__

#include "ObjectManagement/ManagedObject.h"

#include "Mesh.h"
#include "SearchFilter.h"

#include "SearchAgent.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class Search : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<Search> P;

      protected:
         Search( Mesh::P mesh, const bool useFaces );
         virtual ~Search( );

      public:
         static P getInstance( Mesh::P mesh, const bool useFaces );
         virtual std::string getClassName( ) const;

         Mesh::P getMesh( );

         virtual void search( SearchAgent::P agent );
         virtual PointIndexWeights search( const Point testPoint, const Point testNormal, SearchFilter::P filter );

      protected:
         struct D;
         D* m_D;

      };
   }
}

#endif // __SIMPLEMESHSEARCH_H__