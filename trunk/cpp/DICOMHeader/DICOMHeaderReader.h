#ifndef __DICOMHEADERREADER_H__
#define __DICOMHEADERREADER_H__

#include <istream>

namespace AWT
{
   typedef unsigned short GroupType;
   typedef unsigned short ElementType;

   class DICOMTagIdentifier
   {
   public:
      DICOMTagIdentifier( );
      DICOMTagIdentifier( GroupType _group, ElementType _element );

      bool compare( GroupType g, ElementType e ) const;
      
      GroupType group;
      ElementType element;
   };

   class DICOMTagIdentifierConsumer
   {
   public:
      virtual bool wantTag( DICOMTagIdentifier ge ) = 0;

      virtual bool implicitDataCallback( DICOMTagIdentifier ge, char* buffer, int length ) { return true; }
      virtual bool explicitDataCallback( DICOMTagIdentifier ge, char* vr, char* buffer, int length ) { return true; }
   };

   class DICOMHeaderReader
   {
   public:
      static bool readStream( std::istream& in, DICOMTagIdentifierConsumer* consumer );
   };
}

#endif __DICOMHEADERREADER_H__