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