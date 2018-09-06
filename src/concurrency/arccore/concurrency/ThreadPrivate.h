/*---------------------------------------------------------------------------*/
/* ThreadPrivate.h                                             (C) 2000-2018 */
/*                                                                           */
/* Classe permettant de conserver une valeur sp�cifique par thread.          */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_CONCURRENCY_THREADPRIVATE_H
#define ARCCORE_CONCURRENCY_THREADPRIVATE_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArccoreGlobal.h"

#include "arccore/concurrency/GlibAdapter.h"

#include <vector>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Conteneur pour les valeurs priv�es par thread.
 *
 * Il faut appeler initialize() avant d'utiliser les m�thodes setValue()/getValue().
 * Cette m�thode initialize() peut �tre appel�e plusieurs fois.
 */
class ARCCORE_CONCURRENCY_EXPORT ThreadPrivateStorage
{
 public:
  ThreadPrivateStorage();
  ~ThreadPrivateStorage();
 public:
  /*!
   * \brief Initialise la cl� contenant les valeurs par thread.
   * Cette m�thode peut �tre appel�e plusieurs fois et ne fait rien si
   * la cl� a d�j� �t� initialis�e.
   *
   * \warning Cette m�thode n'est pas thread-safe. L'utilisateur doit donc
   * faire attention lors du premier appel.
   */
  void initialize();
  void* getValue();
  void setValue(void* v);
 private:
  GlibPrivate* m_storage;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Classe de base permettant de conserveur une instance d'un objet par thread.
 */
class ARCCORE_CONCURRENCY_EXPORT ThreadPrivateBase
{
  class ICreateFunctor
  {
   public:
    virtual ~ICreateFunctor(){}
    virtual void* createInstance() =0;
  };
 public:
  ThreadPrivateBase(ThreadPrivateStorage* key,ICreateFunctor* create_functor)
  : m_key(key), m_create_functor(create_functor) 
  {
  }

  ~ThreadPrivateBase()
  {
  }

 public:
  /*!
   * \brief R�cup�re l'instance sp�cifique au thread courant.
   *
   * Si cette derni�re n'existe pas encore, elle est cr�� via
   * le functor pass� en argument du constructeur.
   *
   * \warning Cette m�thode ne doit pas �tre appel�e tant que
   * la cl� associ�e (ThreadPrivateStorage) n'a pas �t� initialis�e
   * par l'apple � ThreadPrivateStorage::initialize().
   */
  void* item();
 private:
  ThreadPrivateStorage* m_key;
  GlibMutex m_mutex;
  ICreateFunctor* m_create_functor;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Classe permettant une instance d'un type par thread.
 *
 * Il faut passer en argument du constructeur le conteneur permettant
 * de conserver les valeurs. Ce conteneur doit avoir �t� initialis�
 * via ThreadPrivateStorage::initialize() avant d'utiliser cette classe.
 *
 * Cette classe ne poss�de qu'une seule m�thode item()
 * permettant de r�cup�rer une instance d'un type \a T par
 * thread. Au premier appel de item() pour un thread donn�,
 * une instance de \a T est construite.
 * Le type \a T doit avoir un constructeur par d�faut
 * et doit avoir une m�thode \a build().
 * \threadsafeclass
 */
template<typename T>
class ThreadPrivate
: private ThreadPrivateBase::ICreateFunctor
{
 public:
  ThreadPrivate(ThreadPrivateStorage* key)
  : m_storage(key,this)
  {
  }

  ~ThreadPrivate()
  {
    for( T* item : m_allocated_items )
      delete item;
  }

 public:
  //! Instance sp�cifique au thread courant.
  T* item()
  {
    return (T*)(m_storage.item());
  }

 private:

  void* createInstance() override
  {
    T* new_ptr = new T();
    new_ptr->build();
    m_allocated_items.push_back(new_ptr);
    return new_ptr;
  }

 private:
  std::vector<T*> m_allocated_items;
  ThreadPrivateBase m_storage;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  
