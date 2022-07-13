﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* MpiNeighborVariableSynchronizeDispatcher.cc                 (C) 2000-2022 */
/*                                                                           */
/* Synchronisations des variables via MPI_Neighbor_alltoallv.                */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/FatalErrorException.h"
#include "arcane/utils/CheckedConvert.h"

#include "arcane/parallel/mpi/MpiParallelMng.h"
#include "arcane/parallel/mpi/MpiAdapter.h"
#include "arcane/parallel/mpi/MpiDatatypeList.h"
#include "arcane/parallel/mpi/MpiDatatype.h"
#include "arcane/parallel/mpi/MpiTimeInterval.h"
#include "arcane/parallel/mpi/IVariableSynchronizerMpiCommunicator.h"
#include "arcane/parallel/IStat.h"

#include "arcane/impl/IDataSynchronizeBuffer.h"
#include "arcane/impl/VariableSynchronizerDispatcher.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*
 * Cette implémentation utilise la fonction MPI_Neighbor_alltoallv pour
 * les synchronisations. Cette fonction est disponible dans la version 3.1
 * de MPI.
 */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*
 * \brief Implémentation de la synchronisations des variables via
 * MPI_Neighbor_alltoallv().
 */
class MpiNeighborVariableSynchronizerDispatcher
: public AbstractGenericVariableSynchronizerDispatcher
{
 public:

  class Factory;
  explicit MpiNeighborVariableSynchronizerDispatcher(Factory* f);

 public:

  void compute() override;
  void beginSynchronize(IDataSynchronizeBuffer* buf) override;
  void endSynchronize(IDataSynchronizeBuffer* buf) override;

 private:

  MpiParallelMng* m_mpi_parallel_mng = nullptr;
  UniqueArray<int> m_mpi_send_counts;
  UniqueArray<int> m_mpi_receive_counts;
  UniqueArray<int> m_mpi_send_displacements;
  UniqueArray<int> m_mpi_receive_displacements;
  Ref<IVariableSynchronizerMpiCommunicator> m_synchronizer_communicator;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class MpiNeighborVariableSynchronizerDispatcher::Factory
: public IGenericVariableSynchronizerDispatcherFactory
{
 public:

  Factory(MpiParallelMng* mpi_pm, Ref<IVariableSynchronizerMpiCommunicator> synchronizer_communicator)
  : m_mpi_parallel_mng(mpi_pm)
  , m_synchronizer_communicator(synchronizer_communicator)
  {}

  Ref<IGenericVariableSynchronizerDispatcher> createInstance() override
  {
    auto* x = new MpiNeighborVariableSynchronizerDispatcher(this);
    return makeRef<IGenericVariableSynchronizerDispatcher>(x);
  }

 public:

  MpiParallelMng* m_mpi_parallel_mng = nullptr;
  Ref<IVariableSynchronizerMpiCommunicator> m_synchronizer_communicator;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

extern "C++" Ref<IGenericVariableSynchronizerDispatcherFactory>
arcaneCreateMpiNeighborVariableSynchronizerFactory(MpiParallelMng* mpi_pm,
                                                   Ref<IVariableSynchronizerMpiCommunicator> sync_communicator)
{
  auto* x = new MpiNeighborVariableSynchronizerDispatcher::Factory(mpi_pm, sync_communicator);
  return makeRef<IGenericVariableSynchronizerDispatcherFactory>(x);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

MpiNeighborVariableSynchronizerDispatcher::
MpiNeighborVariableSynchronizerDispatcher(Factory* f)
: m_mpi_parallel_mng(f->m_mpi_parallel_mng)
, m_synchronizer_communicator(f->m_synchronizer_communicator)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void MpiNeighborVariableSynchronizerDispatcher::
beginSynchronize(IDataSynchronizeBuffer* buf)
{
  // Ne fait rien au niveau MPI dans cette partie car cette implémentation
  // ne supporte pas encore l'asynchronisme.
  // On se contente de recopier les valeurs des variables dans le buffer d'envoi
  // pour permettre ensuite de modifier les valeurs de la variable entre
  // le beginSynchronize() et le endSynchronize().

  double send_copy_time = 0.0;
  {
    MpiTimeInterval tit(&send_copy_time);

    // Recopie les buffers d'envoi
    Integer nb_message = buf->nbRank();
    for (Integer i = 0; i < nb_message; ++i)
      buf->copySend(i);
  }
  Int64 total_share_size = buf->totalSendSize();
  m_mpi_parallel_mng->stat()->add("SyncSendCopy", send_copy_time, total_share_size);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void MpiNeighborVariableSynchronizerDispatcher::
endSynchronize(IDataSynchronizeBuffer* buf)
{
  const Int32 nb_message = buf->nbRank();

  auto* sync_communicator = m_synchronizer_communicator.get();
  ARCANE_CHECK_POINTER(sync_communicator);

  MPI_Comm communicator = sync_communicator->communicator();
  if (communicator == MPI_COMM_NULL)
    ARCANE_FATAL("Invalid null communicator");

  MpiParallelMng* pm = m_mpi_parallel_mng;
  MpiDatatypeList* dtlist = pm->datatypes();

  double copy_time = 0.0;
  double wait_time = 0.0;

  if (!buf->hasGlobalBuffer())
    ARCANE_THROW(NotSupportedException,"Can not use MPI_Neighbor_alltoallv when hasGlobalBufer() is false");

  const MPI_Datatype mpi_dt = dtlist->datatype(Byte())->datatype();

  for (Integer i = 0; i < nb_message; ++i) {
    Int32 nb_send = CheckedConvert::toInt32(buf->sendBuffer(i).size());
    Int32 nb_receive = CheckedConvert::toInt32(buf->receiveBuffer(i).size());
    Int32 send_displacement = CheckedConvert::toInt32(buf->sendDisplacement(i));
    Int32 receive_displacement = CheckedConvert::toInt32(buf->receiveDisplacement(i));

    m_mpi_send_counts[i] = nb_send;
    m_mpi_receive_counts[i] = nb_receive;
    m_mpi_send_displacements[i] = send_displacement;
    m_mpi_receive_displacements[i] = receive_displacement;
  }

  {
    MpiTimeInterval tit(&wait_time);
    auto send_buf = buf->globalSendBuffer();
    auto receive_buf = buf->globalReceiveBuffer();
    MPI_Neighbor_alltoallv(send_buf.data(), m_mpi_send_counts.data(), m_mpi_send_displacements.data(), mpi_dt,
                           receive_buf.data(), m_mpi_receive_counts.data(), m_mpi_receive_displacements.data(), mpi_dt,
                           communicator);
  }

  // Recopie les valeurs recues
  {
    MpiTimeInterval tit(&copy_time);
    for (Integer i = 0; i < nb_message; ++i)
      buf->copyReceive(i);
  }

  Int64 total_ghost_size = buf->totalReceiveSize();
  Int64 total_share_size = buf->totalSendSize();
  Int64 total_size = total_ghost_size + total_share_size;
  pm->stat()->add("SyncCopy", copy_time, total_ghost_size);
  pm->stat()->add("SyncWait", wait_time, total_size);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void MpiNeighborVariableSynchronizerDispatcher::
compute()
{
  ItemGroupSynchronizeInfo* sync_info = _syncInfo();
  ARCANE_CHECK_POINTER(sync_info);

  auto* sync_communicator = m_synchronizer_communicator.get();
  ARCANE_CHECK_POINTER(sync_communicator);

  auto sync_list = sync_info->infos();
  const Int32 nb_message = sync_list.size();

  m_mpi_send_counts.resize(nb_message);
  m_mpi_receive_counts.resize(nb_message);
  m_mpi_send_displacements.resize(nb_message);
  m_mpi_receive_displacements.resize(nb_message);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
