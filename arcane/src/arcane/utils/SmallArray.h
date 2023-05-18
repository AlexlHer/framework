﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2023 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* SmallArray.h                                                (C) 2000-2023 */
/*                                                                           */
/* Tableau 1D de données avec buffer pré-alloué.                             */
/*---------------------------------------------------------------------------*/
#ifndef ARCANE_UTILS_STACKARRAY_H
#define ARCANE_UTILS_STACKARRAY_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/Array.h"
#include "arcane/utils/MemoryAllocator.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane::impl
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 * \brief Allocateur avec buffer pré-alloué.
 *
 * Le buffer pré-alloué \a m_preallocated_buffer est utilisé lorsque la
 * taille demandée pour l'allocation est inférieure ou égale à
 * \a m_preallocated_size.
 *
 * Le buffer utilisé doit rester valide durant toute la durée de vie de l'allocateur.
 */
class ARCANE_UTILS_EXPORT StackMemoryAllocator final
: public IMemoryAllocator
{
 public:

  StackMemoryAllocator(void* buf, size_t size)
  : m_preallocated_buffer(buf)
  , m_preallocated_size(size)
  {}

 public:

  bool hasRealloc() const final;
  void* allocate(size_t new_size) final;
  void* reallocate(void* current_ptr, size_t new_size) final;
  void deallocate(void* ptr) final;
  size_t adjustCapacity(size_t wanted_capacity, size_t) final
  {
    return wanted_capacity;
  }
  size_t guarantedAlignment() final { return 0; }

 private:

  void* m_preallocated_buffer = nullptr;
  size_t m_preallocated_size = 0;
};

} // namespace Arcane::impl

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Collection
 *
 * \brief Tableau 1D de données avec buffer pré-alloué.
 *
 * Cette classe s'utilise comme UniqueArray mais contient un buffer de taille
 * fixe égale à \a BufSize qui est utilisé si la quantité de mémoire nécessaire
 * est inférieur à \a BufSize. Cela permet d'éviter des allocations dynamiques
 * lorsque le nombre d'éléments est faible. Si la mémoire nécessaire est
 * supérieure à BufSize, On utilise une allocation dynamique standard.
 */
template <typename T, size_t BufSize = 128>
class SmallArray final
: public Array<T>
{
  using BaseClassType = AbstractArray<T>;
  static constexpr Int64 nb_element_in_buf = (BufSize != 0) ? (BufSize / sizeof(T)) : 0;

 public:

  using typename BaseClassType::ConstReferenceType;
  static constexpr size_t MemorySize = BufSize;

 public:

  //! Créé un tableau vide
  SmallArray()
  : m_stack_allocator(m_stack_buffer, BufSize)
  {
    this->_initFromAllocator(&m_stack_allocator, nb_element_in_buf);
  }

  //! Créé un tableau de \a size éléments contenant la valeur \a value.
  SmallArray(Int64 req_size, ConstReferenceType value)
  : SmallArray()
  {
    this->_resize(req_size, value);
  }

  //! Créé un tableau de \a asize éléments contenant la valeur par défaut du type T()
  explicit SmallArray(Int64 asize)
  : SmallArray()
  {
    this->_resize(asize);
  }

  //! Créé un tableau de \a asize éléments contenant la valeur par défaut du type T()
  explicit SmallArray(Int32 asize)
  : SmallArray((Int64)asize)
  {
  }

  //! Créé un tableau de \a asize éléments contenant la valeur par défaut du type T()
  explicit SmallArray(size_t asize)
  : SmallArray((Int64)asize)
  {
  }

  //! Créé un tableau en recopiant les valeurs de la value \a aview.
  SmallArray(const ConstArrayView<T>& aview)
  : SmallArray(Span<const T>(aview))
  {
  }

  //! Créé un tableau en recopiant les valeurs de la value \a aview.
  SmallArray(const Span<const T>& aview)
  : SmallArray()
  {
    this->_initFromSpan(aview);
  }

  //! Créé un tableau en recopiant les valeurs de la value \a aview.
  SmallArray(const ArrayView<T>& aview)
  : SmallArray(Span<const T>(aview))
  {
  }

  //! Créé un tableau en recopiant les valeurs de la value \a aview.
  SmallArray(const Span<T>& aview)
  : SmallArray(Span<const T>(aview))
  {
  }

  SmallArray(std::initializer_list<T> alist)
  : SmallArray()
  {
    this->_initFromInitializerList(alist);
  }

  //! Créé un tableau en recopiant les valeurs \a rhs.
  SmallArray(const Array<T>& rhs)
  : SmallArray(rhs.constSpan())
  {
  }

  //! Copie les valeurs de \a rhs dans cette instance.
  void operator=(const Array<T>& rhs)
  {
    this->copy(rhs.constSpan());
  }

  //! Copie les valeurs de la vue \a rhs dans cette instance.
  void operator=(const ArrayView<T>& rhs)
  {
    this->copy(rhs);
  }

  //! Copie les valeurs de la vue \a rhs dans cette instance.
  void operator=(const Span<T>& rhs)
  {
    this->copy(rhs);
  }

  //! Copie les valeurs de la vue \a rhs dans cette instance.
  void operator=(const ConstArrayView<T>& rhs)
  {
    this->copy(rhs);
  }

  //! Copie les valeurs de la vue \a rhs dans cette instance.
  void operator=(const Span<const T>& rhs)
  {
    this->copy(rhs);
  }

  //! Détruit l'instance.
  ~SmallArray() override
  {
    // Il faut détruire explicitement car notre allocateur
    // sera détruit avant la classe de base et ne sera plus valide
    // lors de la désallocation.
    this->_destroy();
    this->_internalDeallocate();
    this->_reset();
  }

 public:

  template <size_t N> SmallArray(SmallArray<T, N>&& rhs) = delete;
  template <size_t N> SmallArray<T, BufSize> operator=(SmallArray<T, N>&& rhs) = delete;

 private:

  char m_stack_buffer[BufSize];
  impl::StackMemoryAllocator m_stack_allocator;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif
