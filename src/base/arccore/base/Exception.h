/*---------------------------------------------------------------------------*/
/* Exception.h                                                 (C) 2000-2018 */
/*                                                                           */
/* Classe de base d'une exception.                                           */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_EXCEPTION_H
#define ARCCORE_BASE_EXCEPTION_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/String.h"
#include "arccore/base/StackTrace.h"
// On n'a pas explicitement besoin de ce .h mais il est plus simple
// de l'avoir pour pouvoir facilement lancer des exceptions avec les traces
#include "arccore/base/TraceInfo.h"

#include <exception>
#include <atomic>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Classe de base d'une exception.
 *
 * \ingroup Core
 Les exceptions sont g�r�es par le m�canisme <tt>try</tt> et <tt>catch</tt>
 du C++. Toutes les exceptions lanc�es dans le code <strong>doivent</strong>
 d�river de cette classe.

 Une exception peut-�tre collective. Cela signifie qu'elle sera lanc�e
 par tous les processeurs. Il est possible dans ce cas de n'afficher qu'une
 seule fois le message et �ventuellement d'arr�ter proprement l'ex�cution.
 */
class ARCCORE_BASE_EXPORT Exception
: public std::exception
{
 private:

  /*!
   * \internal
   *
   * Cette m�thode est priv�e pour interdire d'affecter une exception.
   */
  const Exception& operator=(const Exception&); //PURE

 public:

  /*!
   * Construit une exception de nom \a name et
   * envoy�e depuis la fonction \a where.
   */
  Exception(const String& name,const String& where);
  /*!
   * Construit une exception de nom \a name et
   * envoy�e depuis la fonction \a where.
   */
  Exception(const String& name,const TraceInfo& where);
  /*!
   * Construit une exception de nom \a name,
   * envoy�e depuis la fonction \a where et avec le message \a message.
   */
  Exception(const String& name,const String& where,const String& message);
  /*!
   * Construit une exception de nom \a name,
   * envoy�e depuis la fonction \a where et avec le message \a message.
   */
  Exception(const String& name,const TraceInfo& trace,const String& message);
  /*!
   * Construit une exception de nom \a name et
   * envoy�e depuis la fonction \a where.
   */
  Exception(const String& name,const String& where,const StackTrace& stack_trace);
  /*!
   * Construit une exception de nom \a name et
   * envoy�e depuis la fonction \a where.
   */
  Exception(const String& name,const TraceInfo& where,const StackTrace& stack_trace);
  /*!
   * Construit une exception de nom \a name,
   * envoy�e depuis la fonction \a where et avec le message \a message.
   */
  Exception(const String& name,const String& where,
            const String& message,const StackTrace& stack_trace);
  /*!
   * Construit une exception de nom \a name,
   * envoy�e depuis la fonction \a where et avec le message \a message.
   */
  Exception(const String& name,const TraceInfo& trace,
            const String& message,const StackTrace& stack_trace);
  //! Constructeur par copie.
  Exception(const Exception&);
  //! Lib�re les ressources
  ~Exception() ARCCORE_NOEXCEPT override;

 public:
 
  virtual void write(std::ostream& o) const;

  //! Vrai s'il s'agit d'une erreur collective (concerne tous les processeurs)
  bool isCollective() const { return m_is_collective; }

  //! Positionne l'�tat collective de l'expression
  void setCollective(bool v) { m_is_collective = v; }

  //! Positionne les infos suppl�mentaires
  void setAdditionalInfo(const String& v) { m_additional_info = v; }

  //! Retourne les infos suppl�mentaires
  const String& additionalInfo() const { return m_additional_info; }

  //! Pile d'appel au moment de l'exception (n�cessite un service de stacktrace)
  const StackTrace& stackTrace() const { return m_stack_trace; }

  //! Pile d'appel au moment de l'exception (n�cessite un service de stacktrace)
  const String& stackTraceString() const { return m_stack_trace.toString(); }

  //! Indique si des exceptions sont en cours
  static bool hasPendingException();

  static void staticInit();

  //! Message de l'exception
  const String& message() const { return m_message; }

  //! Localisation de l'exception
  const String& where() const { return m_where; }

  //! Nom de l'exception
  const String& name() const { return m_name; }

 protected:
  
  /*! \brief Explique la cause de l'exception dans le flot \a o.
   *
   * Cette m�thode permet d'ajouter des informations suppl�mentaires
   * au message d'exception.
   */
  virtual void explain(std::ostream& o) const;

  //! Positionne le message de l'exception
  void setMessage(const String& msg)
  {
    m_message = msg;
  }

 private:

  String m_name;
  String m_where;
  StackTrace m_stack_trace;
  String m_message;
  String m_additional_info;
  bool m_is_collective;

  void _setStackTrace();
  void _setWhere(const TraceInfo& where);

 private:

  static std::atomic<Int32> m_nb_pending_exception;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

extern "C++" ARCCORE_BASE_EXPORT std::ostream&
operator<<(std::ostream& o,const Exception& ex);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  

