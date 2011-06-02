#ifndef __DEFORMABLEMESHFITTINGPARSER_H__
#define __DEFORMABLEMESHFITTINGPARSER_H__

#include "ObjectManagement/ManagedObject.h"

namespace AWT
{
   template <class T>
   class DeformableMeshFittingStateMachine;

   template <class T>
   class DeformableMeshFittingParser : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer< DeformableMeshFittingParser<T> > P;

   protected:
      DeformableMeshFittingParser( );
      virtual ~DeformableMeshFittingParser( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      bool parse( const std::string& text, DeformableMeshFittingStateMachine<T>* dmfsm );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __DEFORMABLEMESHFITTINGPARSER_H__