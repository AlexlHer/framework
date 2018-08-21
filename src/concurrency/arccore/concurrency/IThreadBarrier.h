/*---------------------------------------------------------------------------*/
/* IThreadBarrier.h                                            (C) 2000-2018 */
/*                                                                           */
/* Interface d'une barri�re avec les threads.                                */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_CONCURRENCY_ITHREADBARRIER_H
#define ARCCORE_CONCURRENCY_ITHREADBARRIER_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/concurrency/ConcurrencyGlobal.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Interface d'une barri�re entre threads.
 *
 * Une fois cr��e (via IThreadImplementation::createBarrier()),
 * la barri�re doit �tre initialis�e
 * via init() pour \a n threads. Ensuite, chaque thread doit
 * appeler la m�thode wait() pour attendre que tous les
 * autres thread arrivent � ce m�me point.
 * La barri�re peut �tre utilis�e plusieurs fois.
 * Pour d�truire la barri�re, il faut appeler destroy(). Cela lib�re aussi
 * l'instance qui ne doit ensuite plus �tre utilis�e.
 */
class ARCCORE_CONCURRENCY_EXPORT IThreadBarrier
{
 protected:

  virtual ~IThreadBarrier(){}

 public:

  //! Initialise la barri�re pour \a nb_thread.
  virtual void init(Integer nb_thread) =0;

  //! D�truit la barri�re.
  virtual void destroy() =0;

  /*!
   * \brief Bloque et attend que tous les threads appellent cette m�thode.
   *
   * \retval true si on est le dernier thread qui appelle cette m�thode.
   * \retval false sinon.
   */
  virtual bool wait() =0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  

