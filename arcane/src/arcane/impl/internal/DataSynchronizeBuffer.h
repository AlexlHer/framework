﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2023 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* DataSynchronizeBuffer.h                                     (C) 2000-2023 */
/*                                                                           */
/* Implémentation d'un buffer générique pour la synchronisation de donnéess. */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#ifndef ARCANE_IMPL_DATASYNCHRONIZEBUFFER_H
#define ARCANE_IMPL_DATASYNCHRONIZEBUFFER_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/MemoryView.h"
#include "arcane/utils/Array.h"
#include "arcane/utils/SmallArray.h"
#include "arcane/utils/TraceAccessor.h"

#include "arcane/impl/IDataSynchronizeBuffer.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
namespace Arcane
{
class IBufferCopier;
class DataSynchronizeInfo;
class DataSynchronizeBufferInfoList;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 * \brief Classe de base de l'implémentation de IDataSynchronizeBuffer.
 */
class ARCANE_IMPL_EXPORT DataSynchronizeBufferBase
: public IDataSynchronizeBuffer
{
  class BufferInfo
  {
    friend DataSynchronizeBufferBase;

   public:

    //! Buffer global
    MutableMemoryView globalBuffer() { return m_memory_view; }

    //! Buffer pour le \a index-ème rang
    MutableMemoryView localBuffer(Int32 index);

    //! Déplacement dans \a globalBuffer() pour le \a index-ème rang
    Int64 displacement(Int32 index) const;

    //! Taille totale en octet du buffer global
    Int64 totalSize() const { return m_memory_view.bytes().size(); }

    //! Numéros locaux des entités pour le rang \a index
    ConstArrayView<Int32> localIds(Int32 index) const;

    void checkValid() const
    {
      ARCANE_CHECK_POINTER(m_buffer_info);
    }

   private:

    MutableMemoryView m_memory_view;
    Int32 m_datatype_size = 0;
    const DataSynchronizeBufferInfoList* m_buffer_info = nullptr;
  };

 public:

  Int32 nbRank() const final { return m_nb_rank; }
  bool hasGlobalBuffer() const final { return true; }

  MutableMemoryView receiveBuffer(Int32 index) final { return m_ghost_buffer_info.localBuffer(index); }
  MutableMemoryView sendBuffer(Int32 index) final { return m_share_buffer_info.localBuffer(index); }

  Int64 receiveDisplacement(Int32 index) const final { return m_ghost_buffer_info.displacement(index); }
  Int64 sendDisplacement(Int32 index) const final { return m_share_buffer_info.displacement(index); }

  MutableMemoryView globalReceiveBuffer() final { return m_ghost_buffer_info.globalBuffer(); }
  MutableMemoryView globalSendBuffer() final { return m_share_buffer_info.globalBuffer(); }

  Int64 totalReceiveSize() const final { return m_ghost_buffer_info.totalSize(); }
  Int64 totalSendSize() const final { return m_share_buffer_info.totalSize(); }

  void barrier() final;

 public:

  void compute(IBufferCopier* copier, DataSynchronizeInfo* sync_list, Int32 datatype_size);

 protected:

  void _allocateBuffers(Int32 datatype_size);

 protected:

  DataSynchronizeInfo* m_sync_info = nullptr;
  //! Buffer pour toutes les données des entités fantômes qui serviront en réception
  BufferInfo m_ghost_buffer_info;
  //! Buffer pour toutes les données des entités partagées qui serviront en envoi
  BufferInfo m_share_buffer_info;

 protected:

  Int32 m_nb_rank = 0;
  IBufferCopier* m_buffer_copier = nullptr;

  //! Buffer contenant les données concaténées en envoi et réception
  UniqueArray<std::byte> m_buffer;

  Int32 m_datatype_size = 0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Implémentation de IDataSynchronizeBuffer pour une donnée
 */
class ARCANE_IMPL_EXPORT SingleDataSynchronizeBuffer
: public DataSynchronizeBufferBase
{
 public:

  void copyReceiveAsync(Int32 index) final;
  void copySendAsync(Int32 index) final;

 public:

  void setDataView(MutableMemoryView v) { m_data_view = v; }
  MutableMemoryView dataView() { return m_data_view; }

 private:

  //! Vue sur les données de la variable
  MutableMemoryView m_data_view;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Implémentation de IDataSynchronizeBuffer pour plusieurs données.
 */
class ARCANE_IMPL_EXPORT MultiDataSynchronizeBuffer
: public TraceAccessor
, public DataSynchronizeBufferBase
{

 public:

  MultiDataSynchronizeBuffer(ITraceMng* tm)
  : TraceAccessor(tm)
  {}

 public:

  void copyReceiveAsync(Int32 index) final;
  void copySendAsync(Int32 index) final;

 public:

  void setNbData(Int32 nb_data)
  {
    m_data_views.resize(nb_data);
  }
  void setDataView(Int32 index, MutableMemoryView v) { m_data_views[index] = v; }

 private:

  //! Vue sur les données de la variable
  SmallArray<MutableMemoryView> m_data_views;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif
