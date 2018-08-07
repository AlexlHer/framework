/*---------------------------------------------------------------------------*/
/* CStringUtils.h                                              (C) 2000-2018 */
/*                                                                           */
/* Fonctions utilitaires sur les cha�nes de caract�res.                      */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_CSTRINGUTILS_H
#define ARCCORE_BASE_CSTRINGUTILS_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArccoreGlobal.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Fonctions utilitaires sur les cha�nes de caract�res.
 */
namespace CStringUtils
{
  //! Retourne \e true si \a s1 et \a s2 sont identiques, \e false sinon
  ARCCORE_BASE_EXPORT bool isEqual(const char* s1,const char* s2);

  //! Retourne \e true si \a s1 est inf�rieur (ordre alphab�tique) � \a s2 , \e false sinon
  ARCCORE_BASE_EXPORT bool isLess(const char* s1,const char* s2);

  //! Retourne la longueur de la cha�ne \a s
  ARCCORE_BASE_EXPORT Integer len(const char* s);

  /*! \brief Copie les \a n premiers caract�res de \a from dans \a to.
   * \retval to */
  ARCCORE_BASE_EXPORT char* copyn(char* to,const char* from,Integer n);

  //! Copie \a from dans \a to
  ARCCORE_BASE_EXPORT char* copy(char* to,const char* from);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif

