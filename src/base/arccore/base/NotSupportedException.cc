/*---------------------------------------------------------------------------*/
/* NotSupportedException.cc                                    (C) 2000-2018 */
/*                                                                           */
/* Exception lorsqu'une op�ration n'est pas support�e.                       */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/String.h"
#include "arccore/base/NotSupportedException.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

NotSupportedException::
NotSupportedException(const String& where)
: Exception("NotSupported",where)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

NotSupportedException::
NotSupportedException(const String& where,const String& message)
: Exception("NotSupported",where)
, m_message(message)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

NotSupportedException::
NotSupportedException(const TraceInfo& where)
: Exception("NotSupported",where)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

NotSupportedException::
NotSupportedException(const TraceInfo& where,const String& message)
: Exception("NotSupported",where)
, m_message(message)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

NotSupportedException::
NotSupportedException(const NotSupportedException& ex) ARCCORE_NOEXCEPT
: Exception(ex)
, m_message(ex.m_message)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void NotSupportedException::
explain(std::ostream& m) const
{
  m << "L'op�ration demand�e n'est pas support�e.\n";

  if (!m_message.null())
    m << "Message: " << m_message << '\n';
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

