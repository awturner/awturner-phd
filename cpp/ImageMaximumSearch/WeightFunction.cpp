#include "WeightFunction.h"

template class AWT::WeightFunction<double>;
template class AWT::WeightFunction<float>;

template class AWT::UniformWeightFunction<double>;
template class AWT::UniformWeightFunction<float>;
template class AWT::ForwardOnlyWeightFunction<double>;
template class AWT::ForwardOnlyWeightFunction<float>;
template class AWT::NormalWeightFunction<double>;
template class AWT::NormalWeightFunction<float>;