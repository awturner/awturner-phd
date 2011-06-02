
#ifndef __PRINTMACROS_H__
#define __PRINTMACROS_H__

#include <iostream>
#include "Useful/Cls.h"

// Select which stream to write to by default
#ifndef PM_DEF_STREAM
#define PM_DEF_STREAM std::cerr
#endif

// Prints a message and then exits
#define PRINTANDEXIT( n, x ) { PM_DEF_STREAM << x << std::endl; exit( n ); }

// Prints a variable to the stream
#define PRINTVBLSTREAM( stream, x )        { stream << (#x) << " = " << (x) << std::endl; stream.flush(); }
// Prints a variable to the stream with a newline after the equals
#define PRINTVBLSTREAMNL( stream, x )      { stream << (#x) << " = " << std::endl << (x) << std::endl; stream.flush(); }
// Prints a variable to the stream, but specifying a different name
#define PRINTVBL2STREAM( stream, x, y )    { stream << (#x) << " = " << (y) << std::endl; stream.flush(); }
// Prints a variable to the stream with a newline after the equals
#define PRINTVBL2STREAMNL( stream, x, y )  { stream << (#x) << " = " << std::endl << (y) << std::endl; stream.flush(); }
// Prints an array to the stream
#define PRINTVECSTREAM( stream, x, N )     { stream << (#x) << " = [ "; for ( unsigned long i = 0; i < N; ++i ) { stream << x[i] << " "; } stream << "]" << std::endl; stream.flush(); }
// Prints an array to the stream, but specifying a different name
#define PRINTVEC2STREAM( stream, y, x, N ) { stream << (#y) << " = [ "; for ( unsigned long i = 0; i < N; ++i ) { stream << x[i] << " "; } stream << "]" << std::endl; stream.flush(); }

// Prints a variable to the stream, adding a semicolon to surpress matlab output
#define PRINTVBLSTREAMMATLAB( stream, x )     { stream << (#x) << " = [" << (x) << "];" << std::endl;  stream.flush();}
// Prints a variable to the stream, but specifying a different name, adding a semicolon to surpress matlab output
#define PRINTVBL2STREAMMATLAB( stream, x, y ) { stream << (#x) << " = [" << (y) << "];" << std::endl;  stream.flush();}
// Prints an array to the stream, adding a semicolon to surpress matlab output
#define PRINTVECSTREAMMATLAB( stream, x, N )  { stream << (#x) << " = [ "; for ( unsigned long i = 0; i < N; ++i ) { stream << x[i] << " "; } stream << "]';" << std::endl;  stream.flush();}
// Prints an array to the stream, but specifying a different name, adding a semicolon to surpress matlab output
#define PRINTVEC2STREAMMATLAB( stream, y, x, N )  { stream << (#y) << " = [ "; for ( unsigned long i = 0; i < N; ++i ) { stream << x[i] << " "; } stream << "]';" << std::endl;  stream.flush();}

// Same as above, but no need to specify which stream
#define PRINTVBL( x )           PRINTVBLSTREAM( PM_DEF_STREAM, x )
#define PRINTVBLMATLAB( x )     PRINTVBLSTREAMMATLAB( PM_DEF_STREAM, x )
#define PRINTVBL2( x, y )       PRINTVBL2STREAM( PM_DEF_STREAM, x, y )
#define PRINTVBLNL( x )         PRINTVBLSTREAMNL( PM_DEF_STREAM, x )
#define PRINTVBL2NL( x, y )     PRINTVBL2STREAMNL( PM_DEF_STREAM, x, y )
#define PRINTVBL2MATLAB( x, y ) PRINTVBL2STREAMMATLAB( PM_DEF_STREAM, x, y )
#define PRINTVEC( x, N )        PRINTVECSTREAM( PM_DEF_STREAM, x, N )
#define PRINTVEC2( y, x, N )    PRINTVEC2STREAM( PM_DEF_STREAM, y, x, N )
#define PRINTVECMATLAB( x, N )  PRINTVECSTREAMMATLAB( PM_DEF_STREAM, x, N )

// Print a boolean value, formatting as true or false
#define PRINTBOOL( x )          PRINTVBL2STREAM( PM_DEF_STREAM, x, ( x ) ? "true" : "false" );

// Prints a blank (spacing) line
#define BLANKLINE PM_DEF_STREAM << std::endl;

#define PBLANKLINE PM_OUT_STREAM << std::endl;

// Prints the given message
#define DEBUGMACRO( x ) do { PM_DEF_STREAM << "DEBUG: " << x << std::endl; } while (0);

// Prints the given message
#define PRINTMACRO( x ) PM_OUT_STREAM << x << std::endl;

// Prints the function, file and line where it appears
#define DEBUGLINE { PM_DEF_STREAM << __FUNCTION__ << "(" << __FILE__ << ":" << __LINE__ << ")" << std::endl; }

// Exits, telling you where
#define DEBUGLINEANDEXIT( code ) { DEBUGMACRO( "Exiting with code " << code << " at " << __FUNCTION__ << ":" << __LINE__ ); exit( code ); }

// Pause execution in a portable way
#define PAUSE { DEBUGMACRO( "Paused at " << __FILE__ << ":" << __LINE__ << std::endl << "Press Enter to continue." ); std::cin.get( ); }

// Make the computer beep!
#ifndef NOBEEP
   #define DEBUGBEEP { PM_DEF_STREAM << static_cast<unsigned char>( 7 ); }
#else
   #define DEBUGBEEP {}
#endif

#endif // __PRINTMACROS_H__
