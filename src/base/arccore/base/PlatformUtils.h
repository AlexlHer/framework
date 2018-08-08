/*---------------------------------------------------------------------------*/
/* PlatformUtils.h                                             (C) 2000-2018 */
/*                                                                           */
/* Fonctions utilitaires d�pendant de la plateforme.                         */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_PLATFORMUTILS_H
#define ARCCORE_BASE_PLATFORMUTILS_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArccoreGlobal.h"

#include <iosfwd>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class String;
class IStackTraceService;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
  \brief Espace de nom pour les fonctions d�pendant de la plateforme.
 
  Cet espace de nom contient toutes les fonctions d�pendant de la plateforme.
*/
namespace Platform
{

/*!
 * \brief Initialisations sp�cifiques � une platforme.
 *
 Cette routine est appel� lors de l'initialisation de l'architecture.
 Elle permet d'effectuer certains traitements qui d�pendent de la
 plateforme
 */
extern "C++" ARCCORE_BASE_EXPORT void platformInitialize();

/*!
 * \brief Routines de fin de programme sp�cifiques � une platforme.
 *
 Cette routine est appel� juste avant de quitter le programme.
 */
extern "C++" ARCCORE_BASE_EXPORT void platformTerminate();

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Date courante.
 *
 * La cha�ne est retourn�e sous la forme jour/mois/ann�e.
 */
extern "C++" ARCCORE_BASE_EXPORT String
getCurrentDate();

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Date courante.
 *
 * Retourne la date courante, exprim�e en secondes �coul�es
 * depuis le 1er janvier 1970.
 */
extern "C++" ARCCORE_BASE_EXPORT long
getCurrentTime();

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*!
 * \brief Date et l'heure courante sous la forme ISO 8601.
 *
 * La cha�ne est retourn�e sous la forme AAAA-MM-JJTHH:MM:SS.
 */
extern "C++" ARCCORE_BASE_EXPORT String
getCurrentDateTime();

/*!
 * \brief Nom de la machine sur lequel tourne le processus.
 */
extern "C++" ARCCORE_BASE_EXPORT String
getHostName();

/*!
 * \brief Chemin du r�pertoire courant.
 */
extern "C++" ARCCORE_BASE_EXPORT String
getCurrentDirectory();

/*!
 * \brief Num�ro du processus.
 */
extern "C++" ARCCORE_BASE_EXPORT int
getProcessId();

/*!
 * \brief Nom de l'utilisateur.
 */
extern "C++" ARCCORE_BASE_EXPORT String
getUserName();

/*!
 * \brief R�pertoire contenant les documents utilisateurs.
 *
 * Cela correspond � la variable d'environnement HOME sur Unix,
 * ou le r�pertoire 'Mes documents' sous Win32.
 */
extern "C++" ARCCORE_BASE_EXPORT String
getHomeDirectory();

/*!
 * \brief Longueur du fichier \a filename.
 * Si le fichier n'est pas lisible ou n'existe pas, retourne 0.
 */
extern "C++" ARCCORE_BASE_EXPORT long unsigned int
getFileLength(const String& filename);

/*!
 * \brief Variable d'environnement du nom \a name.
 *
 * Si aucune variable de nom \a name n'est d�finie,
 * la cha�ne nulle est retourn�e.
 */
extern "C++" ARCCORE_BASE_EXPORT String
getEnvironmentVariable(const String& name);

/*!
 * \brief Cr�� un r�pertoire.
 *
 * Cr�� le r�pertoire de nom \a dir_name. Si n�cessaire, cr�� les
 * r�pertoires parents n�cessaires.
 *
 * \retval true en cas d'�chec,
 * \retval false en cas de succ�s ou si le r�pertoire existe d�j�.
 */
extern "C++" ARCCORE_BASE_EXPORT bool
recursiveCreateDirectory(const String& dir_name);

/*!
 * \brief Cr�� le r�pertoire.
 *
 * Cr�� un r�pertoire de nom \a dir_name. Cette fonction suppose
 * que le r�pertoire parent existe d�j�.
 *
 * \retval true en cas d'�chec,
 * \retval false en cas de succ�s ou si le r�pertoire existe d�j�.
 */
extern "C++" ARCCORE_BASE_EXPORT bool
createDirectory(const String& dir_name);

/*!
 * \brief Supprime le fichier \a file_name.
 *
 * \retval true en cas d'�chec,
 * \retval false en cas de succ�s ou si le fichier n'existe pas.
 */
extern "C++" ARCCORE_BASE_EXPORT bool
removeFile(const String& file_name);

/*!
 * \brief V�rifie que le fichier \a file_name est accessible et lisible.
 *
 * \retval true si le fichier est lisible,
  * \retval false sinon.
 */
extern "C++" ARCCORE_BASE_EXPORT bool
isFileReadable(const String& file_name);

/*!
 * \brief Retourne le nom du r�pertoire d'un fichier.
 *
 * Retourne le nom du r�pertoire dans lequel se trouve le fichier
 * de nom \a file_name.
 * Par exemple, si \a file_name vaut \c "/tmp/toto.cc", retourne "/tmp".
 * Si le fichier ne contient pas de r�pertoires, retourne \c ".".
 */
extern "C++" ARCCORE_BASE_EXPORT String
getFileDirName(const String& file_name);

/*!
 * \brief Copie de zone m�moire
 *
 * Copie \a len octets de l'adresse \a from � l'adresse \a to.
 */
extern "C++" ARCCORE_BASE_EXPORT void
stdMemcpy(void* to,const void* from,::size_t len);

/*!
 * \brief M�moire utilis�e em octets
 *
 * \return la m�moire utilis�e ou un nombre n�gatif si inconnu
 */
extern "C++" ARCCORE_BASE_EXPORT double
getMemoryUsed();

/*!
 * \brief Temps CPU utilis� en microsecondes.
 *
 * L'origine du temps CPU est pris lors de l'appel � platformInitialize().
 *
 * \return le temps CPU utilis� en microsecondes.
 */
extern "C++" ARCCORE_BASE_EXPORT Int64
getCPUTime();

/*!
 * \brief Temps Real utilis� en secondes.
 *
 * \return le temps utilis� en seconde.
 */
extern "C++" ARCCORE_BASE_EXPORT Real
getRealTime();

/*!
 * \brief Retourne un temps sous forme des heures, minutes et secondes.
 *
 * Converti \a t, exprim� en seconde, sous la forme AhBmCs
 * avec A les heures, B les minutes et C les secondes.
 * Par exemple, 3732 devient 1h2m12s.
 */
extern "C++" ARCCORE_BASE_EXPORT String
timeToHourMinuteSecond(Real t);

/*!
 * \brief Retourne \a true si \a v est d�normalis� (flottant invalide).
 *  
 * Si la plate-forme ne supporte pas cette notion, retourne toujours \a false.
 */
extern "C++" ARCCORE_BASE_EXPORT bool
isDenormalized(Real v);

/*!
 * \brief Service utilis� pour obtenir la pile d'appel.
 *
 * Peut retourner nul si aucun service n'est disponible.
 */
extern "C++" ARCCORE_BASE_EXPORT IStackTraceService*
getStackTraceService();

/*! \brief Positionne le service utilis� pour obtenir la pile d'appel.
  
  Retourne l'ancien service utilis�.
*/
extern "C++" ARCCORE_BASE_EXPORT IStackTraceService*
setStackTraceService(IStackTraceService* service);

/*!
 * \brief Retourne une cha�ne de caractere contenant la pile d'appel.
 *
 * Si aucun service de gestion de pile d'appel n'est pr�sent
 * (getStackTraceService()==0), la cha�ne retourn�e est nulle.
 */
extern "C++" ARCCORE_BASE_EXPORT String
getStackTrace();

/*
 * \brief Copie une cha�ne de caract�re avec v�rification de d�bordement.
 *
 * \param input cha�ne � copier.
 * \param output pointeur o� sera recopi� la cha�ne.
 * \param output_len m�moire allou�e pour \a output.
 */
extern "C++" ARCCORE_BASE_EXPORT void
safeStringCopy(char* output,Integer output_len,const char* input);

/*!
 * \brief Met le process en sommeil pendant \a nb_second secondes.
 */
extern "C++" ARCCORE_BASE_EXPORT void
sleep(Integer nb_second);

/*!
 * \brief Active ou d�sactive les exceptions lors d'un calcul flottant.
 * Cette op�ration n'est pas support�e sur toutes les plateformes. Dans
 * le cas o� elle n'est pas support�e, rien ne se passe.
 */
extern "C++" ARCCORE_BASE_EXPORT void
enableFloatingException(bool active);

//! Indique si les exceptions flottantes du processeur sont activ�es.
extern "C++" ARCCORE_BASE_EXPORT bool
isFloatingExceptionEnabled();

/*!
 * \brief L�ve une exception flottante.
 *
 * Cette m�thode ne fait rien si hasFloatingExceptionSupport()==false.
 * En g�n�ral sous Linux, cela se traduit par l'envoie d'un signal
 * de type SIGFPE. Par d�faut %Arccore r�cup�re ce signal et
 * l�ve une exception de type 'ArithmeticException'.
 */
extern "C++" ARCCORE_BASE_EXPORT void
raiseFloatingException();

/*!
 * \brief Indique si l'impl�mentation permet de modifier
 * l'�tat d'activation des exceptions flottantes.
 *
 * Si cette m�thode retourne \a false, alors les m�thodes
 * enableFloatingException() et isFloatingExceptionEnabled()
 * sont sans effet.
 */
extern "C++" ARCCORE_BASE_EXPORT bool
hasFloatingExceptionSupport();

/*!
 * \brief Affiche la pile d'appel sur le flot \a ostr.
 */
extern "C++" ARCCORE_BASE_EXPORT void
dumpStackTrace(std::ostream& ostr);

/*!
 * \brief Indique si la console supporte les couleurs.
 */
extern "C++" ARCCORE_BASE_EXPORT bool
getConsoleHasColor();

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Cha�ne de caract�re permettant d'identifier le compilateur
 * utilis� pour compiler %Arccore.
 */
extern "C++" ARCCORE_BASE_EXPORT String
getCompilerId();

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Platform

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  

