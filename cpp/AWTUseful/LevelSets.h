#ifndef __LevelSets_H__
#define __LevelSets_H__

namespace AWT
{
   template <class T>
   class LevelSets
   {
   public:
      static T meanCurvatureTimesGradientMagnitude( T* in_values, T* in_spacing );
   };
}

#endif // __LevelSets_H__
