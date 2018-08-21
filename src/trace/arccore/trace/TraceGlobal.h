/*---------------------------------------------------------------------------*/
/* TraceGlobal.h                                               (C) 2000-2018 */
/*                                                                           */
/* D�finitions globales de la composante 'Trace' de 'Arccore'.               */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_TRACE_TRACEGLOBAL_H
#define ARCCORE_TRACE_TRACEGLOBAL_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArccoreGlobal.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined(ARCCORE_COMPONENT_arccore_trace)
#define ARCCORE_TRACE_EXPORT ARCCORE_EXPORT
#define ARCCORE_TRACE_EXTERN_TPL
#else
#define ARCCORE_TRACE_EXPORT ARCCORE_IMPORT
#define ARCCORE_TRACE_EXTERN_TPL extern
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{
class ITraceMng;
class TraceMessageClass; 
class TraceClassConfig;
class TraceMessage;
class TraceMessageListenerArgs;
class ITraceMessageListener;
#ifdef ARCCORE_DEBUG
typedef TraceMessage TraceMessageDbg;
#else
class TraceMessageDbg;
#endif
namespace Trace
{
}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  

