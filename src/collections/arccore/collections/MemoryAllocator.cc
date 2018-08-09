/*---------------------------------------------------------------------------*/
/* MemoryAllocator.cc                                          (C) 2000-2016 */
/*                                                                           */
/* Allocateurs m�moires.                                                     */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArgumentException.h"
#include "arccore/base/NotSupportedException.h"
#include "arccore/base/NotImplementedException.h"
#include "arccore/base/TraceInfo.h"

#include "arccore/collections/IMemoryAllocator.h"

#include <cstdlib>
#include <errno.h>

#if defined(ARCCORE_OS_WIN32)
#include <malloc.h>
#endif

#include <iostream>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

DefaultMemoryAllocator DefaultMemoryAllocator::shared_null_instance;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool DefaultMemoryAllocator::
hasRealloc() const
{
  return true;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void* DefaultMemoryAllocator::
allocate(size_t new_size)
{
  return ::malloc(new_size);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void* DefaultMemoryAllocator::
reallocate(void* current_ptr,size_t new_size)
{
  return ::realloc(current_ptr,new_size);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void DefaultMemoryAllocator::
deallocate(void* ptr)
{
  ::free(ptr);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

size_t DefaultMemoryAllocator::
adjustCapacity(size_t wanted_capacity,size_t element_size)
{
  ARCCORE_UNUSED(element_size);
  return wanted_capacity;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

AlignedMemoryAllocator AlignedMemoryAllocator::
SimdAllocator(AlignedMemoryAllocator::simdAlignment());

AlignedMemoryAllocator AlignedMemoryAllocator::
CacheLineAllocator(AlignedMemoryAllocator::cacheLineAlignment());

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * NOTE: Normalement les fonctions _mm_alloc() et _mm_free() permettent
 * d'allouer de la m�moire align�e. Il faudrait v�rifier si elles sont
 * disponibles partout.
 */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool AlignedMemoryAllocator::
hasRealloc() const
{
  return false;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void* AlignedMemoryAllocator::
allocate(size_t new_size)
{
#ifdef ARCCORE_OS_LINUX
  void* ptr = nullptr;
  int e = ::posix_memalign(&ptr,m_alignment,new_size);
  if (e==EINVAL)
    throw ArgumentException(A_FUNCINFO,"Invalid argument to posix_memalign");
  if (e==ENOMEM)
    return nullptr;
  return ptr;
#elif defined(ARCCORE_OS_WIN32)
return _aligned_malloc(new_size,m_alignment);
#else
  throw NotImplementedException(A_FUNCINFO);
#endif
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void* AlignedMemoryAllocator::
reallocate(void* current_ptr,size_t new_size)
{
#ifdef ARCCORE_OS_LINUX
  ARCCORE_UNUSED(current_ptr);
  ARCCORE_UNUSED(new_size);
  throw NotSupportedException(A_FUNCINFO);
#elif defined(ARCCORE_OS_WIN32)
  return _aligned_realloc(current_ptr,new_size,m_alignment);
#else
  throw NotImplementedException(A_FUNCINFO);
#endif
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void AlignedMemoryAllocator::
deallocate(void* ptr)
{
#ifdef ARCCORE_OS_LINUX
  ::free(ptr);
#elif defined(ARCCORE_OS_WIN32)
  return _aligned_free(ptr);
#else
  throw NotImplementedException(A_FUNCINFO);
#endif
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
namespace
{
bool is_verbose = false;
}
size_t AlignedMemoryAllocator::
adjustCapacity(size_t wanted_capacity,size_t element_size)
{
  if (element_size==0)
    return wanted_capacity;
  // Si \a element_size est plus petit que \a m_alignment, consid�re que
  // la m�moire allou�e doit �tre un multiple de l'alignement.
  // (On pourrait �tre plus restrictif suivant les types mais ce n'est en
  // g�n�ral pas utile).
  size_t block_size = m_alignment / element_size;
  if (block_size<=1)
    return wanted_capacity;

  // Si l'alignement n'est pas un multiple de la taille d'un �l�ment,
  // cela signifie que l'�l�ment ne sera pas utilis� pour la vectorisation.
  // Il n'est donc pas n�cessaire dans ce cas de modifier la capacit�.
  size_t nb_element = m_alignment % element_size;
  if (nb_element!=0)
    return wanted_capacity;

  if (is_verbose)
    std::cout << " wanted_capacity=" << wanted_capacity
              << " element_size=" << element_size
              << " block_size=" << block_size << '\n';

  // Ajoute � la capacit� ce qu'il faut pour que le module soit 0.
  size_t modulo = wanted_capacity % block_size;
  if (modulo!=0)
    wanted_capacity += (block_size-modulo);
  if (is_verbose)
    std::cout << " final_wanted_capacity=" << wanted_capacity
              << " modulo=" << modulo << '\n';
  ARCCORE_ASSERT(((wanted_capacity%block_size)==0),("Bad capacity"));
  return wanted_capacity;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool PrintableMemoryAllocator::
hasRealloc() const
{
  return true;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void* PrintableMemoryAllocator::
allocate(size_t new_size)
{
  void* ptr = ::malloc(new_size);
  std::cout << "DEF_ARRAY_ALLOCATE new_size=" << new_size << " ptr=" << ptr << '\n';
  return ptr;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void* PrintableMemoryAllocator::
reallocate(void* current_ptr,size_t new_size)
{
  void* ptr = ::realloc(current_ptr,new_size);
  std::cout << "DEF_ARRAY_REALLOCATE new_size=" << new_size
            << " current_ptr=" << current_ptr
            << " new_ptr=" << ptr << '\n';
  return ptr;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void PrintableMemoryAllocator::
deallocate(void* ptr)
{
  std::cout << "DEF_ARRAY_DEALLOCATE ptr=" << ptr << '\n';
  ::free(ptr);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

size_t PrintableMemoryAllocator::
adjustCapacity(size_t wanted_capacity,size_t element_size)
{
  ARCCORE_UNUSED(element_size);
  return wanted_capacity;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
