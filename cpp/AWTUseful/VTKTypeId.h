#ifndef __VTKTypeId_H__
#define __VTKTypeId_H__

#include "vtkType.h"

inline int VTKTypeId( double         ) { return VTK_DOUBLE; }
inline int VTKTypeId( float          ) { return VTK_FLOAT; }
inline int VTKTypeId( int            ) { return VTK_INT; }
inline int VTKTypeId( short          ) { return VTK_SHORT; }
inline int VTKTypeId( unsigned short ) { return VTK_UNSIGNED_SHORT; }
inline int VTKTypeId( unsigned char  ) { return VTK_UNSIGNED_CHAR; }

#define TYPETOVTK( Ty ) VTKTypeId( static_cast<Ty>( 0 ) )

#endif // __VTKTypeId_H__
