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
#include "Mesh.h"

struct AWT::SimpleMesh::Mesh::D
{
   Points vertices;
   TexCoords texcoords;
   Faces faces;
};

AWT::SimpleMesh::Mesh::Mesh(AWT::SimpleMesh::Index nverts, AWT::SimpleMesh::Index nfaces)
: nv(nverts), nf(nfaces)
{
   m_D = new D;

   m_D->vertices.set_size(4, nverts);
   m_D->vertices.fill(INVALID_COORDINATE);

   m_D->faces.set_size(3, nf);
   m_D->faces.fill(INVALID_INDEX);
}

AWT::SimpleMesh::Mesh::Mesh(const AWT::SimpleMesh::Points& verts, const AWT::SimpleMesh::Faces& faces)
: nv(verts.cols()), nf(faces.cols())
{
   if (verts.rows() != 4)
      throw "Verts must have 4 rows";

   if (faces.rows() != 3)
      throw "Faces must have 3 rows";

   m_D = new D;

   m_D->vertices = verts;

   m_D->faces = faces;
}

AWT::SimpleMesh::Mesh::~Mesh()
{
   delete m_D;
}

AWT::SimpleMesh::Mesh::P AWT::SimpleMesh::Mesh::getInstance(AWT::SimpleMesh::Index nverts, AWT::SimpleMesh::Index nf)
{
   AUTOGETINSTANCE(AWT::SimpleMesh::Mesh, (nverts, nf));
}

AWT::SimpleMesh::Mesh::P AWT::SimpleMesh::Mesh::getInstance(const AWT::SimpleMesh::Points& verts, const AWT::SimpleMesh::Faces& faces)
{
   AUTOGETINSTANCE(AWT::SimpleMesh::Mesh, (verts, faces));
}

GETNAMEMACRO(AWT::SimpleMesh::Mesh)

AWT::SimpleMesh::Point AWT::SimpleMesh::Mesh::getVertex(const Index i) const
{
   return m_D->vertices.get_column(i);
}

AWT::SimpleMesh::Face  AWT::SimpleMesh::Mesh::getFace(const Index i) const
{
   return m_D->faces.get_column(i);
}

AWT::SimpleMesh::Points& AWT::SimpleMesh::Mesh::getVertices() const
{
   return m_D->vertices;
}

AWT::SimpleMesh::TexCoords& AWT::SimpleMesh::Mesh::getTexCoords() const
{
   return m_D->texcoords;
}

AWT::SimpleMesh::Faces&  AWT::SimpleMesh::Mesh::getFaces() const
{
   return m_D->faces;
}
