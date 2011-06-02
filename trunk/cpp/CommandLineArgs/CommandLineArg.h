#ifndef __COMMANDLINEARG_H__
#define __COMMANDLINEARG_H__

#include <string>

namespace AWT
{
   class CommandLineArg
   {
   public:
      CommandLineArg( char* name, bool req );

      virtual bool isRequired( ) const;
      virtual bool isSet( ) const;
      virtual bool isValid( ) const;

      virtual const char* getName( ) const;

   protected:
      const char* m_Name;
      const bool  m_Required;
      bool m_Set;
   };

   template <class T>
   class MultiCommandLineArg : public CommandLineArg
   {
   public:
      MultiCommandLineArg( char* name, int n, bool req );
      ~MultiCommandLineArg( );

      T getValue( const int idx ) const;

      int getNumberOfValues( ) const
      {
         return m_N;
      }

      virtual void parse( char** argv );

      virtual bool isSet( ) const;
      virtual bool isValid( ) const;

   protected:
      virtual bool parse( char* arg, T& out_Value );

      const int   m_N;
      T*          m_Values;
      bool*       m_SetArray;
   };

   typedef MultiCommandLineArg<bool>        BoolCommandLineArg;
   typedef MultiCommandLineArg<int>         IntCommandLineArg;
   typedef MultiCommandLineArg<double>      DoubleCommandLineArg;
   typedef MultiCommandLineArg<std::string> StringCommandLineArg;
}

#endif // __COMMANDLINEARG_H__