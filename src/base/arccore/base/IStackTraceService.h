/*---------------------------------------------------------------------------*/
/* IStackTraceService.h                                        (C) 2000-2018 */
/*                                                                           */
/* Interface d'un service de trace des appels de fonctions.                  */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_ISTACKTRACESERVICE_H
#define ARCCORE_BASE_ISTACKTRACESERVICE_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/BaseTypes.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 * \brief Interface d'un service de trace des appels de fonctions.
 */
class ARCCORE_BASE_EXPORT IStackTraceService
{
 public:

  virtual ~IStackTraceService() {} //<! Lib�re les ressources

 public:

  virtual void build() =0;

 public:

  /*! \brief Cha�ne de caract�re indiquant la pile d'appel.
   *
   * \a first_function indique le num�ro dans la pile de la premi�re fonction
   * affich�e dans la trace.
   */
  virtual StackTrace stackTrace(int first_function=0) =0;

  /*!
   * \brief Nom d'une fonction dans la pile d'appel.
   *
   * \a function_index indique la position de la fonction � retourner dans la
   * pile d'appel. Par exemple, 0 indique la fonction courante, 1 celle
   * d'avant (donc celle qui appelle cette m�thode).
   */
  virtual StackTrace stackTraceFunction(int function_index) =0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  

