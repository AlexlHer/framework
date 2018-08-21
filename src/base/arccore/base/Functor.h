/*---------------------------------------------------------------------------*/
/* Functor.h                                                   (C) 2000-2018 */
/*                                                                           */
/* Classes utilitaires pour g�rer des fonctors.                              */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_FUNCTOR_H
#define ARCCORE_BASE_FUNCTOR_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/IFunctor.h"

#include <functional>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 * \brief Functor associ� � une m�thode d'une classe \a T.
 */
template<typename T>
class FunctorT
: public IFunctor
{
 public:
	
  typedef void (T::*FuncPtr)(); //!< Type du pointeur sur la m�thode

 public:
	
  //! Constructeur
  FunctorT(T* object,FuncPtr funcptr)
  : m_function(funcptr), m_object(object){}

  ~FunctorT() override { }

 protected:

  //! Ex�cute la m�thode associ�
  void executeFunctor() override
  {
    (m_object->*m_function)();
  }

 private:
  FuncPtr m_function; //!< Pointeur vers la m�thode associ�e.
  T* m_object; //!< Objet associ�.
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 * \brief Functor associ� � une m�thode d'une classe \a T.
 */
template<typename ClassType,typename ArgType>
class FunctorWithArgumentT
: public IFunctorWithArgumentT<ArgType>
{
 public:
	
  typedef void (ClassType::*FuncPtr)(ArgType); //!< Type du pointeur sur la m�thode

 public:
	
  //! Constructeur
  FunctorWithArgumentT(ClassType* object,FuncPtr funcptr)
  : m_object(object), m_function(funcptr) {}

 protected:

  //! Ex�cute la m�thode associ�
  void executeFunctor(ArgType arg)
  {
    (m_object->*m_function)(arg);
  }
  
 private:

  ClassType* m_object; //!< Objet associ�.
  FuncPtr m_function; //!< Pointeur vers la m�thode associ�e.
};


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 * \brief Functor g�n�rique utilisant la classe std::function.
 */
template<typename ArgType>
class StdFunctorWithArgumentT
: public IFunctorWithArgumentT<ArgType>
{
 public:
	
  //! Constructeur
  StdFunctorWithArgumentT(const std::function<void(ArgType)>& function)
  : m_function(function) {}

 public:

 protected:

  //! Ex�cute la m�thode associ�
  void executeFunctor(ArgType arg)
  {
    m_function(arg);
  }
  
 private:

  std::function<void(ArgType)> m_function;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  

