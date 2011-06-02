#ifndef __MESHTYPEDEFS_H__
#define __MESHTYPEDEFS_H__

namespace AWT
{
   typedef unsigned int MeshIndex;

   enum MeshSearchType
   {
      SEARCH_RECURSIVE, SEARCH_EXHAUSTIVE
   };

   template <class T>
   class Mesh;
}

#endif //  __MESHTYPEDEFS_H__

   