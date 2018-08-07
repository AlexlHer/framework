/*---------------------------------------------------------------------------*/
/* ArrayRange.h                                                (C) 2000-2018 */
/*                                                                           */
/* Intervalle sur les Array, ArrayView, ConstArrayView, ...                  */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_ARRAYRANGE_H
#define ARCCORE_BASE_ARRAYRANGE_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArrayIterator.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Intervalle sur les classes tableau de %Arccore.
 *
 * Cette classe est utilis�e pour adapter les classes tableaux aux
 * it�rateurs de la STL. Elle fournir les m�thodes telles que begin()/end().
 */
template<typename T>
class ArrayRange
{
 public:

  typedef std::iterator_traits<T> _TraitsType;

 public:

  typedef typename _TraitsType::value_type value_type;
  typedef typename _TraitsType::difference_type difference_type;
  typedef typename _TraitsType::reference reference;
  typedef typename _TraitsType::pointer pointer;
  typedef const value_type* const_pointer;
  //! Type de l'it�rateur sur un �l�ment du tableau
  typedef ArrayIterator<pointer> iterator;
  //! Type de l'it�rateur constant sur un �l�ment du tableau
  typedef ArrayIterator<const_pointer> const_iterator;

 public:

  //! Construit un interval vide.
  ArrayRange() ARCCORE_NOEXCEPT : m_begin(nullptr), m_end(nullptr) { }
  //! Construit un interval allant de \a abegin � \a aend.
  ArrayRange(pointer abegin,pointer aend) ARCCORE_NOEXCEPT : m_begin(abegin), m_end(aend) { }

 public:

  //! Retourne un iterateur sur le premier �l�ment du tableau
  iterator begin() { return iterator(m_begin); }
  //! Retourne un iterateur sur le premier �l�ment apr�s la fin du tableau
  iterator end() { return iterator(m_end); }
  //! Retourne un iterateur constant sur le premier �l�ment du tableau
  const_iterator begin() const { return const_iterator(m_begin); }
  //! Retourne un iterateur constant sur le premier �l�ment apr�s la fin du tableau
  const_iterator end() const { return const_iterator(m_end); }

  //! Pointeur sur le tableau sous-jacent.
  value_type* data() { return m_begin; }
  //! Pointeur constant sur le tableau sous-jacent.
  const value_type* data() const { return m_begin; }
  //! Nombre d'�l�ments dans le tableau
  Integer size() const { return (m_end-m_begin); }
  //! Indique si le tableau est vide.
  bool empty() const { return m_end==m_begin; }

 private:

  T m_begin;
  T m_end;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  
