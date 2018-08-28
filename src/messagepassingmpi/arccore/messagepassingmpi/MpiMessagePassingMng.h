/*---------------------------------------------------------------------------*/
/* MpiMessagePassingMng.h                                      (C) 2000-2018 */
/*                                                                           */
/* Implémentation MPI du gestionnaire des échanges de messages.              */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_MESSAGEPASSINGMPI_MPIMESSAGEPASSINGMNG_H
#define ARCCORE_MESSAGEPASSINGMPI_MPIMESSAGEPASSINGMNG_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/messagepassingmpi/MessagePassingMpiGlobal.h"
#include "arccore/messagepassing/MessagePassingMng.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{
namespace MessagePassing
{
namespace Mpi
{


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Implémentation MPI du gestionnaire des échanges de messages.
 */
class ARCCORE_MESSAGEPASSINGMPI_EXPORT MpiMessagePassingMng
: public MessagePassingMng
{
 public:
  class BuildInfo
  {
   public:
    BuildInfo() : m_comm_rank(A_NULL_RANK), m_comm_size(A_NULL_RANK), m_dispatchers(nullptr){}
    BuildInfo(Int32 comm_rank,Int32 comm_size,IDispatchers* dispatchers)
    : m_comm_rank(comm_rank), m_comm_size(comm_size), m_dispatchers(dispatchers){}
   public:
    Int32 commRank() const { return m_comm_rank; }
    Int32 m_comm_rank;
    Int32 commSize() const { return m_comm_size; }
    Int32 m_comm_size;
    IDispatchers* dispatchers() const { return m_dispatchers; }
    IDispatchers* m_dispatchers;
  }; 

 public:

  MpiMessagePassingMng(const BuildInfo& bi);
  ~MpiMessagePassingMng() override;

 public:

 private:

  MPI_Comm m_communicator;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Mpi
} // End namespace MessagePassing
} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  
