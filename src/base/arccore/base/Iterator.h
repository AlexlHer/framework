/*---------------------------------------------------------------------------*/
/* Iterator.h                                                  (C) 2000-2018 */
/*                                                                           */
/* Iterateurs (obsol�te).                                                    */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_ITERATOR_H
#define ARCCORE_BASE_ITERATOR_H
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
 * \internal
 * \brief Interval d'it�ration.
 * \ingroup Collection
 Cette classe g�re un interval d'it�ration avec un d�but et une fin. Il
 permet de construire simplement un couple d'it�rateur pour it�rer
 sur tout le container.
*/
template<class IT,class R,class P,class V>
class IteratorBase
{
 public:

  IteratorBase(IT b,IT e) : m_begin(b), m_end(e) {}

  void operator++(){ ++m_begin; }
  void operator--(){ --m_begin; }
  R operator* () const { return *m_begin; }
  V operator->() const { return &(*m_begin); }
  bool notEnd() const { return m_begin!=m_end; }
  bool operator()() const { return notEnd(); }
  IT current() const { return m_begin; }
  IT end() const { return m_end; }
	
 private:

  IT m_begin; //!< It�rateur sur l'�l�ment courant
  IT m_end;   //!< It�rateur sur la fin du container.
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 * \brief interval d'it�rateurs
 * \ingroup Collection
 Cette classe g�re un couple d'it�rateurs autorisant la modification des
 �l�ments du container.
 */
template<class T>
class IterT
: public IteratorBase<typename T::iterator,
  typename T::reference,typename T::pointer,typename T::value_type*>
{
 public:

  typedef typename T::iterator iterator;
  typedef typename T::reference reference;
  typedef typename T::pointer pointer;
  typedef typename T::value_type value_type;
  typedef IteratorBase<iterator,reference,pointer,value_type*> Base;

  IterT(T& t) : Base(t.begin(),t.end()) {}
  IterT(iterator b,iterator e) : Base(b,e) {}
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 * \brief interval d'it�rateurs constant
 * \ingroup Collection
 *
 Cette classe g�re un couple d'it�rateurs qui n'autorisent pas la
 modification des �l�ments du container.
 */
template<class T>
class ConstIterT
: public IteratorBase<typename T::const_iterator,
  typename T::const_reference,typename T::const_pointer,const typename T::value_type*>
{
 public:

  typedef typename T::const_iterator const_iterator;
  typedef typename T::const_reference const_reference;
  typedef typename T::const_pointer const_pointer;
  typedef typename T::value_type value_type;
  typedef IteratorBase<const_iterator,const_reference,const_pointer,const value_type*> Base;

  ConstIterT(const T& t) : Base(t.begin(),t.end()) {}
  ConstIterT(const_iterator b,const_iterator e) : Base(b,e) {}
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif

