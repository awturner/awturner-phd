#ifndef __TRIANGLEINTERSECTION_MOLLER_H__
#define __TRIANGLEINTERSECTION_MOLLER_H__

namespace AWT
{
   template <class T>
   class MollerTriangleIntersection
   {
   public:  
      static int tri_tri_intersect(T V0[3], T V1[3], T V2[3],
         T U0[3], T U1[3],T U2[3]);

         // Here is a version withouts divisions (a little faster)
      static int NoDivTriTriIsect(T V0[3],T V1[3],T V2[3],
         T U0[3],T U1[3],T U2[3]);

      //This version computes the line of intersection as well (if they are not coplanar):
      static int tri_tri_intersect_with_isectline(T V0[3],T V1[3],T V2[3], 
         T U0[3],T U1[3],T U2[3],int *coplanar,
         T isectpt1[3],T isectpt2[3]);

   private:
      static int coplanar_tri_tri(T N[3],T V0[3],T V1[3],T V2[3],
                     T U0[3],T U1[3],T U2[3]);

      static void AWT::MollerTriangleIntersection<T>::isect2(T VTX0[3],T VTX1[3],T VTX2[3],T VV0,T VV1,T VV2,
	    T D0,T D1,T D2,T *isect0,T *isect1,T isectpoint0[3],T isectpoint1[3]) ;

      static int compute_intervals_isectline(T VERT0[3],T VERT1[3],T VERT2[3],
				       T VV0,T VV1,T VV2,T D0,T D1,T D2,
				       T D0D1,T D0D2,T *isect0,T *isect1,
				       T isectpoint0[3],T isectpoint1[3]);
   };
}

#endif __TRIANGLEINTERSECTION_MOLLER_H__