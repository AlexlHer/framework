﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2023 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* VariableSynchronizerDispatcher.cc                           (C) 2000-2023 */
/*                                                                           */
/* Service de synchronisation des variables.                                 */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/impl/VariableSynchronizerDispatcher.h"

#include "arcane/utils/ArrayView.h"
#include "arcane/utils/Array2View.h"
#include "arcane/utils/FatalErrorException.h"
#include "arcane/utils/Real2.h"
#include "arcane/utils/Real3.h"
#include "arcane/utils/Real2x2.h"
#include "arcane/utils/Real3x3.h"

#include "arcane/core/VariableCollection.h"
#include "arcane/core/ParallelMngUtils.h"
#include "arcane/core/IParallelExchanger.h"
#include "arcane/core/ISerializeMessage.h"
#include "arcane/core/ISerializer.h"
#include "arcane/core/IData.h"
#include "arcane/core/datatype/DataStorageTypeInfo.h"
#include "arcane/core/datatype/DataTypeTraits.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{

namespace
{
  ArrayView<Byte>
  _toLegacySmallView(MutableMemoryView memory_view)
  {
    Span<std::byte> bytes = memory_view.bytes();
    void* data = bytes.data();
    Int32 size = bytes.smallView().size();
    return { size, reinterpret_cast<Byte*>(data) };
  }

} // namespace

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename SimpleType> VariableSynchronizeDispatcher<SimpleType>::
VariableSynchronizeDispatcher(const VariableSynchronizeDispatcherBuildInfo& bi)
: m_parallel_mng(bi.parallelMng())
, m_factory(bi.factory())
{
  ARCANE_CHECK_POINTER(m_factory.get());
  m_generic_instance = this->m_factory->createInstance();

  if (bi.table())
    m_buffer_copier = new TableBufferCopier(bi.table());
  else
    m_buffer_copier = new DirectBufferCopier();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename SimpleType> VariableSynchronizeDispatcher<SimpleType>::
~VariableSynchronizeDispatcher()
{
  delete m_buffer_copier;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename SimpleType> void VariableSynchronizeDispatcher<SimpleType>::
applyDispatch(IArrayDataT<SimpleType>* data)
{
  if (m_is_in_sync)
    ARCANE_FATAL("Only one pending serialisation is supported");
  m_is_in_sync = true;
  m_1d_buffer.setDataView(MutableMemoryView{ data->view() });
  _beginSynchronize(m_1d_buffer);
  _endSynchronize(m_1d_buffer);
  m_is_in_sync = false;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename SimpleType> void VariableSynchronizeDispatcher<SimpleType>::
applyDispatch(IArray2DataT<SimpleType>* data)
{
  if (m_is_in_sync)
    ARCANE_FATAL("Only one pending serialisation is supported");
  Array2View<SimpleType> value = data->view();

  // Cette valeur doit être la même sur tous les procs
  Integer dim2_size = value.dim2Size();
  if (dim2_size == 0)
    return;
  m_is_in_sync = true;
  Integer dim1_size = value.dim1Size();
  DataStorageTypeInfo storage_info = data->storageTypeInfo();
  Int32 nb_basic_element = storage_info.nbBasicElement();
  Int32 datatype_size = basicDataTypeSize(storage_info.basicDataType()) * nb_basic_element;
  m_2d_buffer.compute(m_buffer_copier, m_sync_info, dim2_size * datatype_size);

  m_2d_buffer.setDataView(makeMutableMemoryView(value.data(), datatype_size * dim2_size,dim1_size));
  _beginSynchronize(m_2d_buffer);
  _endSynchronize(m_2d_buffer);
  m_is_in_sync = false;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename SimpleType> void VariableSynchronizeDispatcher<SimpleType>::
applyDispatch(IScalarDataT<SimpleType>*)
{
  ARCANE_THROW(NotSupportedException, "Can not synchronize scalar data");
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename SimpleType> void VariableSynchronizeDispatcher<SimpleType>::
setItemGroupSynchronizeInfo(ItemGroupSynchronizeInfo* sync_info)
{
  m_sync_info = sync_info;
  m_generic_instance->setItemGroupSynchronizeInfo(sync_info);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Calcul et alloue les tampons nécessaire aux envois et réceptions
 * pour les synchronisations des variables 1D.
 */
template <typename SimpleType> void VariableSynchronizeDispatcher<SimpleType>::
compute()
{
  if (!m_sync_info)
    ARCANE_FATAL("The instance is not initialized. You need to call setItemGroupSynchronizeInfo() before");

  eBasicDataType bdt = DataTypeTraitsT<SimpleType>::basicDataType();
  Int32 nb_basic_type = DataTypeTraitsT<SimpleType>::nbBasicType();
  Int32 datatype_size = basicDataTypeSize(bdt) * nb_basic_type;
  m_1d_buffer.compute(m_buffer_copier, m_sync_info, datatype_size);
  m_generic_instance->compute();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Calcul et alloue les tampons nécessaire aux envois et réceptions
 * pour les synchronisations des variables 1D.
 * \todo: ne pas allouer les tampons car leur conservation est couteuse en
 * terme de memoire.
 */
void VariableSynchronizeBufferBase::
compute(IBufferCopier* copier, ItemGroupSynchronizeInfo* sync_info, Int32 datatype_size)
{
  m_buffer_copier = copier;
  m_sync_info = sync_info;
  auto sync_list = sync_info->infos();
  Integer nb_message = sync_list.size();
  m_nb_rank = nb_message;

  m_ghost_locals_buffer.resize(nb_message);
  m_share_locals_buffer.resize(nb_message);

  m_ghost_displacements.resize(nb_message);
  m_share_displacements.resize(nb_message);

  _allocateBuffers(datatype_size);

  {
    Integer array_index = 0;
    for (Integer i = 0, is = sync_list.size(); i < is; ++i) {
      const VariableSyncInfo& vsi = sync_list[i];
      Int32ConstArrayView ghost_grp = vsi.ghostIds();
      Integer local_size = ghost_grp.size();
      Int32 displacement = array_index;
      m_ghost_displacements[i] = displacement * datatype_size;
      m_ghost_locals_buffer[i] = m_ghost_memory_view.subView(displacement, local_size);
      array_index += local_size;
    }
  }
  {
    Integer array_index = 0;
    for (Integer i = 0, is = sync_list.size(); i < is; ++i) {
      const VariableSyncInfo& vsi = sync_list[i];
      Int32ConstArrayView share_grp = vsi.shareIds();
      Integer local_size = share_grp.size();
      Int32 displacement = array_index;
      m_share_displacements[i] = displacement * datatype_size;
      m_share_locals_buffer[i] = m_share_memory_view.subView(displacement, local_size);
      array_index += local_size;
    }
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VariableSynchronizeBufferBase::
copyReceive(Integer index)
{
  ARCANE_CHECK_POINTER(m_sync_info);
  ARCANE_CHECK_POINTER(m_buffer_copier);

  MutableMemoryView var_values = dataMemoryView();
  const VariableSyncInfo& vsi = (*m_sync_info)[index];
  ConstArrayView<Int32> indexes = vsi.ghostIds();
  ConstMemoryView local_buffer = receiveBuffer(index);

  m_buffer_copier->copyFromBuffer(indexes,local_buffer,var_values);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VariableSynchronizeBufferBase::
copySend(Integer index)
{
  ARCANE_CHECK_POINTER(m_sync_info);
  ARCANE_CHECK_POINTER(m_buffer_copier);

  ConstMemoryView var_values = dataMemoryView();
  const VariableSyncInfo& vsi = (*m_sync_info)[index];
  Int32ConstArrayView indexes = vsi.shareIds();
  MutableMemoryView local_buffer = sendBuffer(index);
  m_buffer_copier->copyToBuffer(indexes,local_buffer,var_values);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Calcul et alloue les tampons nécessaires aux envois et réceptions
 * pour les synchronisations des variables 1D.
 *
 * \todo: ne pas converver les tampons pour chaque type de donnée des variables
 * car leur conservation est couteuse en terme de memoire.
 */
template <typename SimpleType> void VariableSynchronizeDispatcher<SimpleType>::SyncBuffer::
_allocateBuffers(Int32 datatype_size)
{
  auto sync_list = m_sync_info->infos();

  Int64 total_ghost_buffer = 0;
  Int64 total_share_buffer = 0;
  for (auto& s : sync_list) {
    total_ghost_buffer += s.nbGhost();
    total_share_buffer += s.nbShare();
  }

  Int32 full_dim2_size = datatype_size;
  m_buffer.resize((total_ghost_buffer + total_share_buffer) * full_dim2_size);

  Int64 share_offset = total_ghost_buffer * full_dim2_size;

  auto s1 = m_buffer.span().subspan(0, share_offset);
  m_ghost_memory_view = makeMutableMemoryView(s1.data(), full_dim2_size, total_ghost_buffer);
  auto s2 = m_buffer.span().subspan(share_offset, total_share_buffer * full_dim2_size);
  m_share_memory_view = makeMutableMemoryView(s2.data(), full_dim2_size, total_share_buffer);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VariableSynchronizerMultiDispatcher::
synchronize(VariableCollection vars, ConstArrayView<VariableSyncInfo> sync_infos)
{
  Ref<IParallelExchanger> exchanger{ ParallelMngUtils::createExchangerRef(m_parallel_mng) };
  Integer nb_rank = sync_infos.size();
  Int32UniqueArray recv_ranks(nb_rank);
  for (Integer i = 0; i < nb_rank; ++i) {
    Int32 rank = sync_infos[i].targetRank();
    exchanger->addSender(rank);
    recv_ranks[i] = rank;
  }
  exchanger->initializeCommunicationsMessages(recv_ranks);
  for (Integer i = 0; i < nb_rank; ++i) {
    ISerializeMessage* msg = exchanger->messageToSend(i);
    ISerializer* sbuf = msg->serializer();
    Int32ConstArrayView share_ids = sync_infos[i].shareIds();
    sbuf->setMode(ISerializer::ModeReserve);
    for (VariableCollection::Enumerator ivar(vars); ++ivar;) {
      (*ivar)->serialize(sbuf, share_ids, nullptr);
    }
    sbuf->allocateBuffer();
    sbuf->setMode(ISerializer::ModePut);
    for (VariableCollection::Enumerator ivar(vars); ++ivar;) {
      (*ivar)->serialize(sbuf, share_ids, nullptr);
    }
  }
  exchanger->processExchange();
  for (Integer i = 0; i < nb_rank; ++i) {
    ISerializeMessage* msg = exchanger->messageToReceive(i);
    ISerializer* sbuf = msg->serializer();
    Int32ConstArrayView ghost_ids = sync_infos[i].ghostIds();
    sbuf->setMode(ISerializer::ModeGet);
    for (VariableCollection::Enumerator ivar(vars); ++ivar;) {
      (*ivar)->serialize(sbuf, ghost_ids, nullptr);
    }
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

VariableSynchronizerDispatcher::
~VariableSynchronizerDispatcher()
{
  delete m_dispatcher;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VariableSynchronizerDispatcher::
setItemGroupSynchronizeInfo(ItemGroupSynchronizeInfo* sync_info)
{
  for (IVariableSynchronizeDispatcher* d : m_dispatcher->dispatchers())
    d->setItemGroupSynchronizeInfo(sync_info);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VariableSynchronizerDispatcher::
compute()
{
  m_parallel_mng->traceMng()->info(4) << "DISPATCH RECOMPUTE";
  auto dispatchers = m_dispatcher->dispatchers();
  for (IVariableSynchronizeDispatcher* d : dispatchers)
    d->compute();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VariableSyncInfo::
_changeIds(Array<Int32>& ids, Int32ConstArrayView old_to_new_ids)
{
  UniqueArray<Int32> orig_ids(ids);
  ids.clear();

  for (Integer z = 0, zs = orig_ids.size(); z < zs; ++z) {
    Int32 old_id = orig_ids[z];
    Int32 new_id = old_to_new_ids[old_id];
    if (new_id != NULL_ITEM_LOCAL_ID)
      ids.add(new_id);
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void VariableSyncInfo::
changeLocalIds(Int32ConstArrayView old_to_new_ids)
{
  _changeIds(m_share_ids, old_to_new_ids);
  _changeIds(m_ghost_ids, old_to_new_ids);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Implémentation basique de la sérialisation.
 *
 * Cette implémentation est faite à partir de send/receive suivi de 'wait'.
 */
class SimpleVariableSynchronizerDispatcher
: public AbstractGenericVariableSynchronizerDispatcher
{
 public:

  class Factory;
  explicit SimpleVariableSynchronizerDispatcher(Factory* f);

 protected:

  void compute() override {}
  void beginSynchronize(IDataSynchronizeBuffer* buf) override;
  void endSynchronize(IDataSynchronizeBuffer* buf) override;

 private:

  IParallelMng* m_parallel_mng = nullptr;
  UniqueArray<Parallel::Request> m_all_requests;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class SimpleVariableSynchronizerDispatcher::Factory
: public IGenericVariableSynchronizerDispatcherFactory
{
 public:

  explicit Factory(IParallelMng* pm)
  : m_parallel_mng(pm)
  {}

  Ref<IGenericVariableSynchronizerDispatcher> createInstance() override
  {
    auto* x = new SimpleVariableSynchronizerDispatcher(this);
    return makeRef<IGenericVariableSynchronizerDispatcher>(x);
  }

 public:

  IParallelMng* m_parallel_mng = nullptr;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

SimpleVariableSynchronizerDispatcher::
SimpleVariableSynchronizerDispatcher(Factory* f)
: m_parallel_mng(f->m_parallel_mng)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

extern "C++" Ref<IGenericVariableSynchronizerDispatcherFactory>
arcaneCreateSimpleVariableSynchronizerFactory(IParallelMng* pm)
{
  auto* x = new SimpleVariableSynchronizerDispatcher::Factory(pm);
  return makeRef<IGenericVariableSynchronizerDispatcherFactory>(x);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void SimpleVariableSynchronizerDispatcher::
beginSynchronize(IDataSynchronizeBuffer* vs_buf)
{
  IParallelMng* pm = m_parallel_mng;

  const bool use_blocking_send = false;
  auto sync_list = _syncInfo()->infos();
  Int32 nb_message = sync_list.size();

  /*pm->traceMng()->info() << " ** ** COMMON BEGIN SYNC n=" << nb_message
                         << " this=" << (IVariableSynchronizeDispatcher*)this
                         << " m_sync_list=" << &this->m_sync_list;*/

  // Envoie les messages de réception non bloquant
  for (Integer i = 0; i < nb_message; ++i) {
    const VariableSyncInfo& vsi = sync_list[i];
    auto buf = _toLegacySmallView(vs_buf->receiveBuffer(i));
    if (!buf.empty()) {
      Parallel::Request rval = pm->recv(buf, vsi.targetRank(), false);
      m_all_requests.add(rval);
    }
  }

  vs_buf->copyAllSend();

  // Envoie les messages d'envoi en mode non bloquant.
  for (Integer i = 0; i < nb_message; ++i) {
    const VariableSyncInfo& vsi = sync_list[i];
    auto buf = _toLegacySmallView(vs_buf->sendBuffer(i));

    //ConstArrayView<SimpleType> const_share = share_local_buffer;
    if (!buf.empty()) {
      //for( Integer i=0, is=share_local_buffer.size(); i<is; ++i )
      //trace->info() << "TO rank=" << vsi.m_target_rank << " I=" << i << " V=" << share_local_buffer[i]
      //                << " lid=" << share_grp[i] << " v2=" << var_values[share_grp[i]];
      Parallel::Request rval = pm->send(buf, vsi.targetRank(), use_blocking_send);
      if (!use_blocking_send)
        m_all_requests.add(rval);
    }
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void SimpleVariableSynchronizerDispatcher::
endSynchronize(IDataSynchronizeBuffer* vs_buf)
{
  IParallelMng* pm = m_parallel_mng;

  /*pm->traceMng()->info() << " ** ** COMMON END SYNC n=" << nb_message
                         << " this=" << (IVariableSynchronizeDispatcher*)this
                         << " m_sync_list=" << &this->m_sync_list;*/

  // Attend que les réceptions se terminent
  pm->waitAllRequests(m_all_requests);
  m_all_requests.clear();

  // Recopie dans la variable le message de retour.
  vs_buf->copyAllReceive();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void IDataSynchronizeBuffer::
copyAllSend()
{
  Int32 nb_rank = nbRank();
  for (Int32 i = 0; i < nb_rank; ++i)
    copySend(i);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void IDataSynchronizeBuffer::
copyAllReceive()
{
  Int32 nb_rank = nbRank();
  for (Int32 i = 0; i < nb_rank; ++i)
    copyReceive(i);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define ARCANE_INSTANTIATE(type) \
  template class ARCANE_TEMPLATE_EXPORT VariableSynchronizeDispatcher<type>

ARCANE_INSTANTIATE(Byte);
ARCANE_INSTANTIATE(Int16);
ARCANE_INSTANTIATE(Int32);
ARCANE_INSTANTIATE(Int64);
ARCANE_INSTANTIATE(Real);
ARCANE_INSTANTIATE(Real2);
ARCANE_INSTANTIATE(Real3);
ARCANE_INSTANTIATE(Real2x2);
ARCANE_INSTANTIATE(Real3x3);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
