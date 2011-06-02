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
#include "MeshIO.h"

using namespace AWT::SimpleMesh;

struct AWT::SimpleMesh::MeshIO::D
{
   static const double SCALE;

   template <class T>
   static void readMatrix(std::istream& is, const Index cs, vnl_matrix<T>& mat, const Index rs = INVALID_INDEX)
   {
      const Index rrs = (rs == INVALID_INDEX) ? mat.rows() : rs;
      
      for (Index c = 0; c < cs; ++c)
      {
         for (Index r = 0; r < rrs; ++r)
            is >> mat(r,c);

         if (is.bad() || !is.good())
            throw "Could not read vertices";
      }
   }

   static void processV(std::istream& is, Matrix& mat)
   {
      Index n;
      is >> n;

      mat.set_size(4, n);
      readMatrix<double>(is, n, mat, 3);
      mat *= SCALE;
      mat.set_row(3, 1);
   }

   static void processT(std::istream& is, Matrix& mat)
   {
      Index n;
      is >> n;

      mat.set_size(2, n);
      readMatrix<double>(is, n, mat);
   }

   static void processF(std::istream& is, Faces& mat)
   {
      Index n;
      is >> n;

      mat.set_size(3, n);
      readMatrix<Index>(is, n, mat);
   }

   static void skip(std::istream& is, const Index ncomponents)
   {
      double val;
      Index n;
      is >> n;

      n *= ncomponents;
      do
         is >> val;
      while (--n > 0);
   }
};

const double AWT::SimpleMesh::MeshIO::D::SCALE = 1;

Mesh::P AWT::SimpleMesh::MeshIO::loadMesh(std::istream& is)
{
   Matrix vertices(4, 0);
   Faces faces(3, 0);
   TexCoords texcoords(2, 0);

   std::string tag;

   while (true)
   {
      is >> tag;

      if (tag == "v")
         D::processV(is, vertices);
      else if (tag == "f")
         D::processF(is, faces);
      else if (tag == "t")
         D::processT(is, texcoords);
      else if (tag == "e")
         break;
      else
      {
         PRINTVBL(tag);
         throw "Unknown tag";
      }
   }

   Mesh::P ret = Mesh::getInstance(vertices.cols(), faces.cols());
   ret->getVertices().update(vertices);
   ret->getFaces().update(faces);
   
   TexCoords& tc = ret->getTexCoords();
   tc.set_size(texcoords.rows(), texcoords.cols());
   tc.update(texcoords);

   return ret;
}

ShapeModel::P AWT::SimpleMesh::MeshIO::loadModel(std::istream& is)
{
   Points vertices(4, 0);
   Faces faces(3, 0);
   TexCoords texcoords(2, 0);
   std::vector<Points> modes;

   std::string tag;

   DEBUGLINE;
   while (is.good())
   {
      is >> tag;
      if (is.bad() || !is.good())
         break;

      PRINTVBL(tag);

      if (tag == "v")
      {
         Matrix mode;
         D::processV(is, mode);
         modes.push_back(mode);
      }
      else if (tag == "f")
         D::processF(is, faces);
      else if (tag == "t")
         D::processT(is, texcoords);
      else if (tag == "e")
         break;
      else
      {
         PRINTVBL(tag);
         throw "Unknown tag";
      }
   }
   DEBUGLINE;

   typedef std::vector<Points> Pointss;

   Pointss::iterator it = modes.begin();
   Pointss::iterator en = modes.end();

   if (it == en)
      throw "Model has no vertex data";

   Mesh::P meanMesh = Mesh::getInstance(it->cols(), faces.cols());
   meanMesh->getVertices().update(*it);
   meanMesh->getFaces().update(faces);

   ShapeModel::P model = ShapeModel::getInstance(meanMesh);

   while (++it != en)
      model->addMode(*it);

   DEBUGLINE;
   return model;
}

void AWT::SimpleMesh::MeshIO::saveMesh(std::ostream& os, Mesh::P mesh)
{
   // Write the vertices
   os << "v" "\t" << mesh->nv << std::endl;
   Points& vertices = mesh->getVertices();
   for (Index v = 0; v < mesh->nv; ++v)
      os << vertices(0,v)/D::SCALE << "\t" << vertices(1,v)/D::SCALE << "\t" << vertices(2,v)/D::SCALE << "\n";

   // Write the vertices
   os << "f" "\t" << mesh->nf << std::endl;
   Faces& faces = mesh->getFaces();
   for (Index f = 0; f < mesh->nf; ++f)
      os << faces(0,f) << "\t" << faces(1,f) << "\t" << faces(2,f) << "\n";

   os << "e" << std::endl;
}
