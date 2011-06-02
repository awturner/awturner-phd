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
#include "Search.h"

#include "MeshFunctions.h"

#include "TreeData.h"
#include "NearestSearch.h"
#include "RaySearch.h"

#include "Useful/ProfTimer.h"

#include <queue>

#include "ObjectEKDTree/OEKDTree.h"

using namespace AWT::SimpleMesh;

struct AWT::SimpleMesh::Search::D
{
   Mesh::P mesh;

   OEKDTree::OEKDTree<double,3>::P tree;

   typedef AWT::OEKDTree::OEKDTreeBranch<double,3> Branch;
   typedef AWT::OEKDTree::SqDistBounds<double> SqDistBounds;

   typedef std::pair<SqDistBounds, Branch*> BranchPair;
   typedef std::priority_queue<BranchPair,std::vector<BranchPair>,std::greater<BranchPair> > BranchQueue;

   void checkExhaustive(OEKDNearestSearcher<double,3>* searcher, Branch* in_Branch)
   {
      for (int i = 0, imax = in_Branch->getSize(); i < imax; i++)
         searcher->checkObject(in_Branch->getOriginalIndex(i));
   }

   void checkQueue(SearchAgent::P agent)
   {
      // Create a new queue to check for this
      BranchQueue q;

      OEKDTree::SqDistBounds<double> initBounds = { 0, FAR_FAR_AWAY };

      agent->setMesh(mesh);

      SearchAgent* pAgent = *agent;

      {
         ///AWT::TimedBlock block("Searching...");

         OEKDTreeBranch<double,3>* root = tree->getRootBranch();
         q.push(BranchPair(initBounds, root));
         while (!q.empty())
         {
            BranchPair pair = q.top();
            q.pop();

            if (agent->shouldCheck(pair.second, pair.first))
               handleBranch(pAgent, pair.second, q);

            // Return this item, whether or not it was checked
            OEKDTreeBranch<double,3>::checkIn(pair.second);
         }
      }

      agent->setMesh(0);
   }

   void handleBranch(OEKDNearestSearcher<double,3>* searcher, Branch* in_Branch, BranchQueue& q)
   {
      if (in_Branch == 0)
         return;

      Branch* children[2];

      children[0] = in_Branch->getChild(0);
      children[1] = in_Branch->getChild(1);

      SqDistBounds bounds;

      if (children[0] == 0 && children[1] == 0)
      {
         // Check this branch exhaustively if there are no children
         checkExhaustive(searcher, in_Branch);

         return;
      }
      else
      {
         for (int i = 0; i < 2; i++)
         {
            if (children[i] != 0)
            {
               searcher->calculateBoxDistanceBounds2(children[i], bounds);
               q.push(BranchPair(bounds, children[i]));
            }
         }
      }
   }
};

AWT::SimpleMesh::Search::Search(AWT::SimpleMesh::Mesh::P mesh, const bool useFaces)
{
   m_D = new D;

   m_D->mesh = mesh;

   TreeData::P treedata = TreeData::getInstance(mesh, useFaces);

   m_D->tree = OEKDTree::OEKDTree<double,3>::getInstance(treedata, 10);
}

AWT::SimpleMesh::Search::~Search()
{
   delete m_D;
}

AWT::SimpleMesh::Search::P AWT::SimpleMesh::Search::getInstance(Mesh::P mesh, const bool useFaces)
{
   AUTOGETINSTANCE(AWT::SimpleMesh::Search, (mesh, useFaces));
}

GETNAMEMACRO(AWT::SimpleMesh::Search);

Mesh::P AWT::SimpleMesh::Search::getMesh()
{
   return m_D->mesh;
}

void AWT::SimpleMesh::Search::search(SearchAgent::P agent)
{
   m_D->checkQueue(agent);
}

PointIndexWeights AWT::SimpleMesh::Search::search(const Point testPoint, const Point testNormal, SearchFilter::P filter)
{
#if 1
   NearestSearch::P searchAgent = NearestSearch::getInstance();

   searchAgent->reset();
   searchAgent->setTestPoint(testPoint);
   searchAgent->setSearchFilter(filter);
#else
   RaySearch::P searchAgent = RaySearch::getInstance();

   searchAgent->reset();
   searchAgent->setTestPoint(testPoint);
   searchAgent->setTestNormal(testNormal);
   searchAgent->setSearchFilter(filter);
#endif

   search(searchAgent);
   
   if (searchAgent->isPointValid())
      return searchAgent->getNearestPoint();
   else
      return PointIndexWeights();
}
