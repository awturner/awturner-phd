#ifndef __VTKIMAGETYPE_H__
#define __VTKIMAGETYPE_H__

namespace AWT
{
   template <class T>
   int getVTKImageType( T arg );
}

#define VTK_TYPE_SPECIALIZED_TEMPLATE( Ty, VTK_Ty ) template <> int AWT::getVTKImageType( Ty ) { return VTK_Ty; }

#include "vtkType.h"

VTK_TYPE_SPECIALIZED_TEMPLATE( float,          VTK_FLOAT );
VTK_TYPE_SPECIALIZED_TEMPLATE( double,         VTK_DOUBLE );
VTK_TYPE_SPECIALIZED_TEMPLATE( int,            VTK_INT );
VTK_TYPE_SPECIALIZED_TEMPLATE( unsigned int,   VTK_UNSIGNED_INT );
VTK_TYPE_SPECIALIZED_TEMPLATE( long,           VTK_LONG );
VTK_TYPE_SPECIALIZED_TEMPLATE( unsigned long,  VTK_UNSIGNED_LONG );
VTK_TYPE_SPECIALIZED_TEMPLATE( short,          VTK_SHORT );
VTK_TYPE_SPECIALIZED_TEMPLATE( unsigned short, VTK_UNSIGNED_SHORT );
VTK_TYPE_SPECIALIZED_TEMPLATE( unsigned char,  VTK_UNSIGNED_CHAR );
VTK_TYPE_SPECIALIZED_TEMPLATE( char,           VTK_CHAR );

#undef VTK_TYPE_SPECIALIZED_TEMPLATE

#endif // __VTKIMAGETYPE_H__