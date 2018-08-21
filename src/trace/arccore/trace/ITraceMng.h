/*---------------------------------------------------------------------------*/
/* ITraceMng.h                                                 (C) 2000-2017 */
/*                                                                           */
/* Gestionnaire des traces.                                                  */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_TRACE_ITRACEMNG_H
#define ARCCORE_TRACE_ITRACEMNG_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArrayView.h"
#include "arccore/base/BaseTypes.h"
#include "arccore/trace/TraceMessage.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Arguments de ITraceMessageListener::visitMessage().
 *
 * \a buffer() contient la cha�ne de caract�re � afficher.
 * \a buffer() se termine toujours par un z�ro terminal.
 *
 * Une instance de cette classe est un objet temporaire qui ne doit
 * pas �tre conserv� au del� de l'appel ITraceMessageListener::visitMessage().
 */
class TraceMessageListenerArgs
{
 public:
  TraceMessageListenerArgs(const TraceMessage* msg,ConstArrayView<char> buf)
  : m_message(msg), m_buffer(buf){}
 public:
  //! Infos sur le message de trace
  const TraceMessage* message() const
  {
    return m_message;
  }

  //! Cha�ne de caract�res du message.
  ConstArrayView<char> buffer() const
  {
    return m_buffer;
  }
 private:
  const TraceMessage* m_message;
  ConstArrayView<char> m_buffer;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Interface d'un visiteur pour les messages de trace.
 */
class ITraceMessageListener
{
 public:
  virtual ~ITraceMessageListener() {}
  /*!
   * \brief R�ception du message \a msg contenant la ch�ine \a str.
   *
   * Si le retour est \a true, le message n'est pas utilis� par le ITraceMng.
   *
   * L'instance a le droit d'appeler ITraceMng::writeDirect() pour �crire
   * directement des messages pendant l'appel � cette m�thode.
   *
   * \warning Attention, cette fonction doit �tre thread-safe car elle peut �tre
   * appel�e simultan�ment par plusieurs threads.
   */
  virtual bool visitMessage(const TraceMessageListenerArgs& args) =0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Core
 * \brief Interface du gestionnaire de traces.
 *
 Une instance de cette classe g�re les flots de traces.
 Pour envoyer un message, il suffit d'appeler la m�thode corrrespondante
 (info() pour un message d'information, error() pour une erreur, ...)
 pour r�cup�rer un flot et d'utiliser l'op�rateur << sur ce flot pour
 transmettre un message.
 
 Par exemple:
 \code
 ITraceMng* tr = ...;
 tr->info() << "Ceci est une information.";
 int proc_id = 0;
 tr->error() << "Erreur sur le processeur " << proc_id;
 \endcode

 Le message est envoy� lors de la destruction du flot. Dans les
 exemples pr�c�dents, les flots sont temporairement cr��s (par la m�thode info())
 et d�truits d�s que l'op�rateur << a �t� appliqu� dessus.

 \warning Il faut absolument appeler la m�thode finishInitialize() avant
 d'utiliser les appels aux m�thodes pushTraceClass() et popTraceClass().
 
 Si on souhaite envoyer un message en plusieurs fois, il faut stocker
 le flot retourn�:

 \code
 TraceMessage info = m_trace_mng->info();
 info() << "D�but de l'information.\n"
 info() << "Fin de l'information.";
 \endcode

 Il est possible d'utiliser des formatteurs simples sur les messages (via la classe #TraceMessage)
 ou des formatteurs standards des iostream en appliquant l'op�rateur operator()
 de TraceMessage.
 */
class ARCCORE_TRACE_EXPORT ITraceMng
{
 public:
  virtual ~ITraceMng(){}
 public:
  //! Flot pour un message d'erreur
  virtual TraceMessage error() =0;
  //! Flot pour un message d'erreur parall�le
  virtual TraceMessage perror() =0;
  //! Flot pour un message d'erreur fatale
  virtual TraceMessage fatal() =0;
  //! Flot pour un message d'erreur fatale parall�le
  virtual TraceMessage pfatal() =0;
  //! Flot pour un message d'avertissement
  virtual TraceMessage warning() =0;
  //! Flot pour un message d'avertissement parall�le
  virtual TraceMessage pwarning() =0;
  //! Flot pour un message d'information
  virtual TraceMessage info() =0;
  //! Flot pour un message d'information parall�le
  virtual TraceMessage pinfo() =0;
  //! Flot pour un message d'information d'une cat�gorie donn�e
  virtual TraceMessage info(char category) =0;
  //! Flot pour un message d'information d'un niveau donn�
  virtual TraceMessage info(Int32 level) =0;
  //! Flot pour un message d'information parall�le d'une cat�gorie donn�e
  virtual TraceMessage pinfo(char category) =0;
  //! Flot pour un message d'information conditionnel.
  virtual TraceMessage info(bool) =0;
  //! Flot pour un message de log.
  virtual TraceMessage log() =0;
  //! Flot pour un message de log parall�le.
  virtual TraceMessage plog() =0;
  //! Flot pour un message de log pr�c�d� de l'heure.
  virtual TraceMessage logdate() =0;
  //! Flot pour un message de debug.
  virtual TraceMessageDbg debug(Trace::eDebugLevel =Trace::Medium) =0;
  //! Flot pour un message non utilis�
  virtual TraceMessage devNull() =0;

  /*!
   * \brief Modifie l'�tat d'activation des messages d'info.
   *
   * \return l'ancien �tat d'activation.
   */
  virtual bool setActivated(bool v) =0;

  //! Termine l'initialisation
  virtual void finishInitialize() =0;

  /*!
   * \brief Ajoute la classe \a s � la pile des classes de messages actifs.
   * \threadsafe
   */
  virtual void pushTraceClass(const String& name) =0;
	
  /*!
   * \brief Supprime la derni�re classe de message de la pile.
   * \threadsafe
   */
  virtual void popTraceClass() =0;
	
  //! Flush tous les flots.
  virtual void flush() =0;

  //! Redirige tous les messages sur le flot \a o
  virtual void setRedirectStream(std::ostream* o) =0;

  //! Retourne le niveau dbg du fichier de configuration
  virtual Trace::eDebugLevel configDbgLevel() const =0;

 public:

  /*!
   * \brief Ajoute l'observateur \a v � ce gestionnaire de message.
   *
   * L'appelant reste propri�taire de \a v et doit l'enlever
   * via removeListener() avant de le d�truire.
   */
  virtual void addListener(ITraceMessageListener* v) =0;

  //! Supprime l'observateur \a v de ce gestionnaire de message.
  virtual void removeListener(ITraceMessageListener* v) =0;

  /*!
   * \brief Positionne l'identifiant du gestionnnaire.
   *
   * Si non nul, l'identifiant est affich� en cas d'erreur pour
   * identifier l'instance qui affiche le message. L'identifiant
   * peut �tre quelconque. Par d�faut, il s'agit du rang du processus et
   * du nom de la machine.
   */
  virtual void setTraceId(const String& id) =0;

  //! Identifiant du gestionnnaire.
  virtual const String& traceId() const =0;

  /*!
   * \brief Positionne le nom du fichier d'erreur � \a file_name.
   *
   * Si un fichier d'erreur est d�j� ouvert, il est referm� et un nouveau
   * avec ce nouveau nom de fichier sera cr�� lors de la prochaine erreur.
   *
   * Si \a file_name est la cha�ne nulle, aucun fichier d'erreur n'est utilis�.
   */
  virtual void setErrorFileName(const String& file_name) =0;

  /*!
   * \brief Positionne le nom du fichier de log � \a file_name.
   *
   * Si un fichier de log est d�j� ouvert, il est referm� et un nouveau
   * avec ce nouveau nom de fichier sera cr�� lors du prochaine log.
   *
   * Si \a file_name est la cha�ne nulle, aucun fichier de log n'est utilis�.
   */
  virtual void setLogFileName(const String& file_name) =0;

 public:

  //! Signale un d�but d'�criture du message \a message
  virtual void beginTrace(const TraceMessage* message) =0;

  //! Signale une fin d'�criture du message \a message
  virtual void endTrace(const TraceMessage* message) =0;

  /*!
   * \brief Envoie directement un message de type \a type.
   * \a type doit correspondre � Trace::eMessageType.
   * Cette m�thode ne doit �tre utilis�e que par le wrapping .NET.
   */
  virtual void putTrace(const String& message,int type) =0;

 public:
  
  //! Positionne la configuration pour la classe de message \a name
  virtual void setClassConfig(const String& name,const TraceClassConfig& config) =0;

  //! Configuration associ�es � la classe de message \a name
  virtual TraceClassConfig classConfig(const String& name) const =0;

  virtual void setMaster(bool is_master) =0;

  virtual bool isMaster() const =0;

  /*!
   * Positionne le niveau de verbosit� des sorties.
   * Les messages de niveau sup�rieur � ce niveau ne sont pas sortis.
   * Le niveau utilis� est celui donn� en argument de info(Int32).
   * Le niveau par d�faut est celui donn� par TraceMessage::DEFAULT_LEVEL.
   */
  virtual void setVerbosityLevel(Int32 level) =0;

  //! Niveau de verbosit� des messages
  virtual Int32 verbosityLevel() const =0;

  /*!
   * \internal
   * Indique que le gestionnaire de thread � chang� et qu'il faut
   * red�clarer les structures g�rant le multi-threading.
   * Interne � Arccore, ne pas utiliser.
   */
  virtual void resetThreadStatus() =0;

  /*!
   * \brief �crit directement un message.
   *
   * Ecrit directement le message \a msg contenant la cha�ne \a buf_array.
   * Le message n'est pas analys� par l'instance et est toujours �crit
   * sans aucun formattage sp�cifique. Cette op�ration ne doit en principe
   * �tre utilis�e que par un ITraceMessageListener. Pour les autres cas,
   * il faut utiliser les traces standards.
   */
  virtual void writeDirect(const TraceMessage* msg,const String& str) =0;

  //! Supprime toutes les classes de configuration positionn�es via setClassConfig().
  virtual void removeAllClassConfig() =0;

  /*!
   * \biref Applique le fonctor \a functor sur l'ensemble des TraceClassConfig enregistr�s.
   *
   * Le premier argument de la paire est le nom de classe de configuration et
   * le deuxi�me sa valeur telle que retourn�e par classConfig().
   *
   * Il est permis de modifier le TraceClassConfig en cours de visite via
   * un appel � setClassConfig().
   */
  virtual void visitClassConfigs(IFunctorWithArgumentT<std::pair<String,TraceClassConfig>>* functor) =0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

extern "C++" ARCCORE_TRACE_EXPORT
ITraceMng* arccoreCreateDefaultTraceMng();

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif
