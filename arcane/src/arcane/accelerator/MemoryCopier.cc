﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2023 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* MemoryCopier.cc                                             (C) 2000-2023 */
/*                                                                           */
/* Fonctions diverses de copie mémoire.                                      */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/accelerator/AcceleratorGlobal.h"

#include "arcane/utils/Ref.h"
#include "arcane/utils/internal/SpecificMemoryCopyList.h"

#include "arcane/accelerator/core/RunQueue.h"
#include "arcane/accelerator/RunCommandLoop.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane::Accelerator::impl
{
using IndexedMemoryCopyArgs = Arcane::impl::IndexedMemoryCopyArgs;
using IndexedMultiMemoryCopyArgs = Arcane::impl::IndexedMultiMemoryCopyArgs;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename DataType, typename Extent>
class AcceleratorSpecificMemoryCopy
: public Arcane::impl::SpecificMemoryCopyBase<DataType, Extent>
{
  using BaseClass = Arcane::impl::SpecificMemoryCopyBase<DataType, Extent>;
  using BaseClass::_toTrueType;

 public:

  using BaseClass::m_extent;

 public:

  void copyFrom(const IndexedMemoryCopyArgs& args) override
  {
    _copyFrom(args.m_queue, args.m_indexes, _toTrueType(args.m_source), _toTrueType(args.m_destination));
  }

  void copyTo(const IndexedMemoryCopyArgs& args) override
  {
    _copyTo(args.m_queue, args.m_indexes, _toTrueType(args.m_source), _toTrueType(args.m_destination));
  }

  void copyFrom(const IndexedMultiMemoryCopyArgs& args) override
  {
    _copyFrom(args.m_queue, args.m_indexes, args.m_multi_memory, _toTrueType(args.m_source_buffer));
  }

  void copyTo(const IndexedMultiMemoryCopyArgs& args) override
  {
    _copyTo(args.m_queue, args.m_indexes, args.m_const_multi_memory, _toTrueType(args.m_destination_buffer));
  }

 public:

  void _copyFrom(RunQueue* queue, SmallSpan<const Int32> indexes,
                 Span<const DataType> source, Span<DataType> destination)
  {
    ARCANE_CHECK_POINTER(queue);

    Int32 nb_index = indexes.size();
    const Int64 sub_size = m_extent.v;

    auto command = makeCommand(queue);
    command << RUNCOMMAND_LOOP1(iter, nb_index)
    {
      auto [i] = iter();
      Int64 zindex = i * sub_size;
      Int64 zci = indexes[i] * sub_size;
      for (Int32 z = 0; z < sub_size; ++z)
        destination[zindex + z] = source[zci + z];
    };
  }

  void _copyFrom(RunQueue* queue, SmallSpan<const Int32> indexes, SmallSpan<Span<std::byte>> multi_views,
                 Span<const DataType> source)
  {
    ARCANE_CHECK_POINTER(queue);

    const Int32 nb_index = indexes.size() / 2;
    // On devrait pouvoir utiliser 'm_extent.v' mais avec CUDA 12.1 cela génère
    // une erreur lors de l'exécution: error 98 : invalid device function
    const Int32 sub_size = m_extent.v;

    auto command = makeCommand(queue);
    command << RUNCOMMAND_LOOP1(iter, nb_index)
    {
      auto [i] = iter();
      Int32 index0 = indexes[i * 2];
      Int32 index1 = indexes[(i * 2) + 1];
      Span<std::byte> orig_view_bytes = multi_views[index0];
      auto* orig_view_data = reinterpret_cast<DataType*>(orig_view_bytes.data());
      // Utilise un span pour tester les débordements de tableau mais on
      // pourrait directement utiliser 'orig_view_data' pour plus de performances
      Span<DataType> orig_view = { orig_view_data, orig_view_bytes.size() / (Int64)sizeof(DataType) };
      Int64 zci = ((Int64)(index1)) * sub_size;
      Int64 z_index = (Int64)i * sub_size;
      for (Int32 z = 0, n = sub_size; z < n; ++z)
        orig_view[zci + z] = source[z_index + z];
    };
  }

  void _copyTo(RunQueue* queue, SmallSpan<const Int32> indexes, Span<const DataType> source,
               Span<DataType> destination)
  {
    ARCANE_CHECK_POINTER(queue);

    Int32 nb_index = indexes.size();
    const Int64 sub_size = m_extent.v;

    auto command = makeCommand(queue);
    command << RUNCOMMAND_LOOP1(iter, nb_index)
    {
      auto [i] = iter();
      Int64 zindex = i * sub_size;
      Int64 zci = indexes[i] * sub_size;
      for (Int32 z = 0; z < sub_size; ++z)
        destination[zci + z] = source[zindex + z];
    };
  }

  void _copyTo(RunQueue* queue, SmallSpan<const Int32> indexes, SmallSpan<const Span<const std::byte>> multi_views,
               Span<DataType> destination)
  {
    ARCANE_CHECK_POINTER(queue);
    const Int32 nb_index = indexes.size() / 2;
    // On devrait pouvoir utiliser 'm_extent.v' mais avec CUDA 12.1 cela génère
    // une erreur lors de l'exécution: error 98 : invalid device function
    const Int32 sub_size = m_extent.v;

    auto command = makeCommand(queue);
    command << RUNCOMMAND_LOOP1(iter, nb_index)
    {
      auto [i] = iter();
      Int32 index0 = indexes[i * 2];
      Int32 index1 = indexes[(i * 2) + 1];
      Span<const std::byte> orig_view_bytes = multi_views[index0];
      auto* orig_view_data = reinterpret_cast<const DataType*>(orig_view_bytes.data());
      // Utilise un span pour tester les débordements de tableau mais on
      // pourrait directement utiliser 'orig_view_data' pour plus de performances
      Span<const DataType> orig_view = { orig_view_data, orig_view_bytes.size() / (Int64)sizeof(DataType) };
      Int64 zci = ((Int64)(index1)) * sub_size;
      Int64 z_index = (Int64)i * sub_size;
      for (Int32 z = 0, n = sub_size; z < n; ++z)
        destination[z_index + z] = orig_view[zci + z];
    };
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class AcceleratorIndexedCopyTraits
{
 public:

  using InterfaceType = Arcane::impl::ISpecificMemoryCopy;
  template <typename DataType, typename Extent> using SpecificType = AcceleratorSpecificMemoryCopy<DataType, Extent>;
  using RefType = Arcane::impl::SpecificMemoryCopyRef<AcceleratorIndexedCopyTraits>;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class AcceleratorSpecificMemoryCopyList
{
 public:
  AcceleratorSpecificMemoryCopyList()
  {
    Arcane::impl::ISpecificMemoryCopyList::setDefaultCopyListIfNotSet(&m_copy_list);
  }
  Arcane::impl::SpecificMemoryCopyList<AcceleratorIndexedCopyTraits> m_copy_list;
};

namespace
{
  AcceleratorSpecificMemoryCopyList global_copy_list;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // namespace Arcane::Accelerator::impl

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
