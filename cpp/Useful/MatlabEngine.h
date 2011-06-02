#ifndef __MATLABENGINE_H__
#define __MATLABENGINE_H__

#include "ObjectManagement/ManagedObject.h"
#include "engine.h" // MATLAB Engine include
#include "MatlabArray.h"

namespace AWT
{
   class MatlabEngine : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MatlabEngine> P;

   protected:
      MatlabEngine( );
      virtual ~MatlabEngine( );

   public:
      static P getInstance( );
      virtual std::string getClassName( ) const;

      Engine* getEngine( );

      int      eval( const char* cmd );

      MatlabArray::P getVariable( const char* name );
      int            putVariable( const char* name, MatlabArray::P );

   protected:
      struct D;
      D* m_D;
   };
}

#endif // __MATLABENGINE_H__