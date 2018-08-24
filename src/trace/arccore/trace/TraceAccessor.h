/*---------------------------------------------------------------------------*/
/* TraceAccessor.h                                             (C) 2000-2018 */
/*                                                                           */
/* Acc�s aux traces.                                                         */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_TRACE_TRACEACCESSOR_H
#define ARCCORE_TRACE_TRACEACCESSOR_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/trace/TraceMessage.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ITraceMng;
class TraceMessage;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Classe d'acc�s aux traces.
 * \ingroup Core
 */
class ARCCORE_TRACE_EXPORT TraceAccessor
{
 public:

  // Construit un accesseur via le gestionnaire de trace \a m.
  explicit TraceAccessor(ITraceMng* m);
  TraceAccessor(const TraceAccessor& rhs) = default;
  virtual ~TraceAccessor(); //!< Lib�re les ressources

 public:

  //! Gestionnaire de trace
  ITraceMng* traceMng() const;

  //! Flot pour un message d'information
  TraceMessage info() const;

  /*! \brief Flot pour un message d'information en parall�le.
   *
   * A la difference de info(), tous les processeurs �crivent ce
   * message sur la sortie standard.
   */
  TraceMessage pinfo() const;

  //! Flot pour un message d'information d'une cat�gorie donn�e
  TraceMessage info(char category) const;

  //! Flot pour un message d'information parall�le d'une cat�gorie donn�e
  TraceMessage pinfo(char category) const;

  /*!
   * \brief Flot pour un message d'information.
   *
   * Si \a v est \a false, le message ne sera pas affich�.
   */
  TraceMessage info(bool v) const;

  //! Flot pour un message d'avertissement
  TraceMessage warning() const;

  /*! Flot pour un message d'avertissement parall�le
   *
   * A la difference de warning(), seul le processeur ma�tre �crit ce message.
   */
  TraceMessage pwarning() const;

  //! Flot pour un message d'erreur
  TraceMessage error() const;

  /*! Flot pour un message d'erreur parall�le
   *
   * A la difference de error(), seul le processeur ma�tre �crit ce message.
   */
  TraceMessage perror() const;

  //! Flot pour un message de log
  TraceMessage log() const;

  //! Flot pour un message de log
  TraceMessage plog() const;

  //! Flot pour un message de log pr�c�d� de la date
  TraceMessage logdate() const;

  //! Flot pour un message d'erreur fatale
  TraceMessage fatal() const;

  //! Flot pour un message d'erreur fatale en parall�le
  TraceMessage pfatal() const;

#ifdef ARCCORE_DEBUG
  //! Flot pour un message de debug
  TraceMessageDbg debug(Trace::eDebugLevel =Trace::Medium) const;
#else
  //! Flot pour un message de debug
  TraceMessageDbg debug(Trace::eDebugLevel =Trace::Medium) const
  { return TraceMessageDbg(); }
#endif

  //! Niveau debug du fichier de configuration
  Trace::eDebugLevel configDbgLevel() const;

  //! Flot pour un message d'information d'un niveau donn�
  TraceMessage info(Int32 verbose_level) const;

  //! Flot pour un message d'information avec le niveau d'information local � cette instance.
  TraceMessage linfo() const
  {
    return info(m_local_verbose_level);
  }

  //! Flot pour un message d'information avec le niveau d'information local � cette instance.
  TraceMessage linfo(Int32 relative_level) const
  {
    return info(m_local_verbose_level+relative_level);
  }

 protected:
  
  void _setLocalVerboseLevel(Int32 v) { m_local_verbose_level = v; }
  Int32 _localVerboseLevel() const { return m_local_verbose_level; }

 private:

  ITraceMng* m_trace;
  Int32 m_local_verbose_level;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  

