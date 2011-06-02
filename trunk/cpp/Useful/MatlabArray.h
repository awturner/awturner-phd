#ifndef __MATLABARRAY_H__
#define __MATLABARRAY_H__

#include "ObjectManagement/ManagedObject.h"
#include "engine.h"
#include "vnl/vnl_matrix.h"

namespace AWT
{
   class MatlabEngine;

   class MatlabArray : public ManagedObject
   {
   public:
      typedef ManagedAutoPointer<MatlabArray> P;

   protected:
      MatlabArray( MatlabEngine* eng, const char* name );
      MatlabArray( const vnl_matrix<double>& matrix );

      virtual ~MatlabArray( );

   public:
      // Retrieves the variable with the given name from the engine's workspace
      static P getInstance( MatlabEngine* eng, const char* name );

      // Creates a new array and populate with the data in this matrix
      static P getInstance( const vnl_matrix<double>& matrix );

      virtual std::string getClassName( ) const;

      const unsigned int getNumberOfDimensions( );
      void getDimensions( unsigned int* dims );

      const char* getVariableName( );
      const char* getTypeName( );
      
      template <typename T>
      T* getData( );

      // Convenience method to copy variable back into matrix
      void getMatrix( vnl_matrix<double>& matrix );

      mxArray* getArray( );

   protected:
      struct D;
      D* m_D;

   };
}

#endif // __MATLABARRAY_H__