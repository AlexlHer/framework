// TODO: ne pas inclure car cela induit des dépendances circulaires
#include "arcane/accelerator/Reduce.h"
#include "arcane/accelerator/cuda/CudaReduceImpl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane::Accelerator::impl
{
template class ReduceFunctorMin<double>;
template class ReduceFunctorMax<double>;
template class ReduceFunctorSum<double>;

template class ReduceFunctorMin<int>;
template class ReduceFunctorMax<int>;
template class ReduceFunctorSum<int>;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
