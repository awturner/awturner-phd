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
#ifndef __SIMPLEMESH_H__
#define __SIMPLEMESH_H__

#include "ObjectManagement/ManagedObject.h"

#include "TypeDefs.h"

namespace AWT
{
   namespace SimpleMesh
   {
      class Mesh : public ManagedObject
      {
      public:
         typedef ManagedAutoPointer<Mesh> P;

      protected:
         Mesh( Index nverts, Index nf );
         Mesh( const Points& verts, const Faces& faces );
         virtual ~Mesh( );

      public:
         static P getInstance( Index nverts, Index nf );
         static P getInstance( const Points& verts, const Faces& faces );

         virtual std::string getClassName( ) const;

         const Index nv;
         const Index nf;

         Point getVertex( const Index i ) const;
         Face  getFace( const Index i ) const;

         Points& getVertices( ) const;
         TexCoords& getTexCoords( ) const;

         Faces&  getFaces( ) const;

      protected:
         struct D;
         D* m_D;
      };
   }
}

#endif // __SIMPLEMESH_H__