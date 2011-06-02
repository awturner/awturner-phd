#ifndef __IMAGEVISITOR_H__
#define __IMAGEVISITOR_H__

namespace AWT
{
   template <class T>
   class ImageVisitor : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<ImageVisitor> P;

   public:
      virtual bool visit( const unsigned int x, const unsigned int y, const unsigned int z, const T value ) = 0;
   };
}

#endif // __IMAGEVISITOR_H__