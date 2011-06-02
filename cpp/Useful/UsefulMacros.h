#ifndef __USEFULMACROS_H__
#define __USEFULMACROS_H__

// Explicitly case a and b to a given type before division
#define TYPED_DIV( Ty, a, b ) ( static_cast<Ty>( a ) / static_cast<Ty>( b ) )

#endif // __USEFULMACROS_H__