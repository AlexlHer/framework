/*---------------------------------------------------------------------------*/
/* Span.h                                                      (C) 2000-2018 */
/*                                                                           */
/* Types d�finissant les vues de tableaux C dont la taille est un Int64.     */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_LARGEARRAYVIEW_H
#define ARCCORE_BASE_LARGEARRAYVIEW_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArrayView.h"

#include <type_traits>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{
namespace detail
{
// Pour indiquer que Span<T>::view() retourne un ArrayView
// et Span<const T>::view() retourn un ConstArrayView.
template<typename T>
class ViewTypeT
{
 public:
  using view_type = ArrayView<T>;
};
template<typename T>
class ViewTypeT<const T>
{
 public:
  using view_type = ConstArrayView<T>;
};
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Collection
 * \brief Vue d'un tableau d'�l�ments de type \a T.
 *
 * La vue est non modifiable si l'argument template est de type 'const T'.
 Cette classe permet d'acc�der et d'utiliser un tableau d'�l�ments du
 type \a T de la m�me mani�re qu'un tableau C standard. Elle est similaire �
 ArrayView � ceci pr�s que le nombre d'�l�ments est stock� sur un 'Int64' et
 peut donc d�passer 2Go. Elle est concue pour �tre similaire � la classe
 std::span du C++20.
*/
template<typename T>
class Span
{
  // Pour le cas o� on ne supporte pas le C++14.
  template< bool B, class XX = void >
  using Span_enable_if_t = typename std::enable_if<B,XX>::type;

 public:

  using ElementType = T;
  using element_type = ElementType;
  using value_type = typename std::remove_cv<ElementType>::type;
  using index_type = Int64;
  using difference_type = Int64;
  using pointer = ElementType*;
  using const_pointer = typename std::add_const<ElementType*>::type;
  using reference = ElementType&;
  using iterator = pointer;
  using const_iterator = const ElementType*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using view_type = typename detail::ViewTypeT<ElementType>::view_type;

 public:

  //! Construit une vue vide.
  Span() : m_ptr(nullptr), m_size(0) {}
  //! Constructeur de recopie depuis une autre vue
  Span(const ArrayView<value_type>& from)
  : m_ptr(from.m_ptr), m_size(from.size()) {}
  // Constructeur � partir d'un ConstArrayView. Cela n'est autoris� que
  // si T est const.
  template<typename X,typename = Span_enable_if_t<std::is_same<X,value_type>::value> >
  Span(const ConstArrayView<X>& from)
  : m_ptr(from.data()), m_size(from.size()) {}
  // Pour un Span<const T>, on a le droit de construire depuis un Span<T>
  template<typename X,typename = Span_enable_if_t<std::is_same<X,value_type>::value> >
  Span(const Span<X>& from)
  : m_ptr(from.data()), m_size(from.size()) {}
  //! Construit une vue sur une zone m�moire commencant par \a ptr et
  // contenant \a asize �l�ments.
  Span(T* ptr,Int64 asize)
  : m_ptr(ptr), m_size(asize) {}

 public:

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline T& operator[](Int64 i)
  {
    ARCCORE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline const T& operator[](Int64 i) const
  {
    ARCCORE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline const T& item(Int64 i) const
  {
    ARCCORE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  /*!
   * \brief Positionne le i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline void setItem(Int64 i,const T& v)
  {
    ARCCORE_CHECK_AT(i,m_size);
    m_ptr[i] = v;
  }

  //! Retourne la taille du tableau
  inline Int64 size() const { return m_size; }
  //! Nombre d'�l�ments du tableau
  inline Int64 length() const { return m_size; }

  /*!
   * \brief It�rateur sur le premier �l�ment du tableau.
   */
  iterator begin() { return iterator(m_ptr); }
  /*!
   * \brief It�rateur sur le premier �l�ment apr�s la fin du tableau.
   */
  iterator end() { return iterator(m_ptr+m_size); }
  /*!
   * \brief It�rateur constant sur le premier �l�ment du tableau.
   */
  const_iterator begin() const { return iterator(m_ptr); }
  /*!
   * \brief It�rateur constant sur le premier �l�ment apr�s la fin du tableau.
   */
  const_iterator end() const { return iterator(m_ptr+m_size); }

 public:

  //! Intervalle d'it�ration du premier au derni�r �l�ment.
  ArrayRange<pointer> range()
  {
    return ArrayRange<pointer>(m_ptr,m_ptr+m_size);
  }
  //! Intervalle d'it�ration du premier au derni�r �l�ment.
  ArrayRange<const_pointer> range() const
  {
    return ArrayRange<const_pointer>(m_ptr,m_ptr+m_size);
  }

 public:
  //! Addresse du index-�me �l�ment
  inline T* ptrAt(Int64 index)
  {
    ARCCORE_CHECK_AT(index,m_size);
    return m_ptr+index;
  }

  //! Addresse du index-�me �l�ment
  inline const T* ptrAt(Int64 index) const
  {
    ARCCORE_CHECK_AT(index,m_size);
    return m_ptr+index;
  }

  // El�ment d'indice \a i. V�rifie toujours les d�bordements
  const T& at(Int64 i) const
  {
    arccoreCheckAt(i,m_size);
    return m_ptr[i];
  }

  // Positionne l'�l�ment d'indice \a i. V�rifie toujours les d�bordements
  void setAt(Int64 i,const T& value)
  {
    arccoreCheckAt(i,m_size);
    m_ptr[i] = value;
  }

  //! Remplit le tableau avec la valeur \a o
  inline void fill(T o)
  {
    for( Int64 i=0, n=m_size; i<n; ++i )
      m_ptr[i] = o;
  }

  /*!
   * \brief Vue constante sur cette vue.
   */
  view_type smallView() const
  {
    Integer s = arccoreCheckArraySize(m_size);
    return view_type(s,m_ptr);
  }

  /*!
   * \brief Vue constante sur cette vue.
   */
  ConstArrayView<value_type> constSmallView() const
  {
    Integer s = arccoreCheckArraySize(m_size);
    return ConstArrayView<value_type>(s,m_ptr);
  }

  /*!
   * \brief Sous-vue � partir de l'�l�ment \a abegin
   * et contenant \a asize �l�ments.
   *
   * Si \a (\a abegin+ \a asize) est sup�rieur � la taille du tableau,
   * la vue est tronqu�e � cette taille, retournant �ventuellement une vue vide.
   */
  Span<T> subspan(Int64 abegin,Int64 asize) const
  {
    if (abegin>=m_size)
      return Span<T>();
    asize = _min(asize,m_size-abegin);
    return Span<T>(m_ptr+abegin,asize);
  }

  /*!
   * \brief Sous-vue � partir de l'�l�ment \a abegin
   * et contenant \a asize �l�ments.
   *
   * Si \a (\a abegin+ \a asize) est sup�rieur � la taille du tableau,
   * la vue est tronqu�e � cette taille, retournant �ventuellement une vue vide.
   */
  Span<T> subView(Int64 abegin,Int64 asize) const
  {
    return subspan(abegin,asize);
  }

  //! Sous-vue correspondant � l'interval \a index sur \a nb_interval
  Span<T> subViewInterval(Int64 index,Int64 nb_interval) const
  {
    Int64 n = m_size;
    Int64 isize = n / nb_interval;
    Int64 ibegin = index * isize;
    // Pour le dernier interval, prend les elements restants
    if ((index+1)==nb_interval)
      isize = n - ibegin;
    return Span<T>(m_ptr+ibegin,isize);
  }

  /*!\brief Recopie le tableau \a copy_array dans l'instance.
   * Comme aucune allocation m�moire n'est effectu�e, le
   * nombre d'�l�ments de \a copy_array doit �tre inf�rieur ou �gal au
   * nombre d'�l�ments courant. S'il est inf�rieur, les �l�ments du
   * tableau courant situ�s � la fin du tableau sont inchang�s
   */
  template<class U>
  inline void copy(const U& copy_array)
  {
    ARCCORE_ASSERT( (copy_array.size()<=m_size), ("Bad size %d %d",copy_array.size(),m_size) );
    const T* copy_begin = copy_array.data();
    T* to_ptr = m_ptr;
    Int64 n = copy_array.size();
    for( Int64 i=0; i<n; ++i )
      to_ptr[i] = copy_begin[i];
  }

  //! Retourne \a true si le tableau est vide (dimension nulle)
  bool empty() const { return m_size==0; }
  //! \a true si le tableau contient l'�l�ment de valeur \a v
  bool contains(const T& v) const
  {
    for( Int64 i=0; i<m_size; ++i ){
      if (m_ptr[i]==v)
        return true;
    }
    return false;
  }

 public:

  void setArray(const ArrayView<T>& v)
  {
    m_ptr = v.m_ptr;
    m_size = v.m_size;
  }
  void setArray(const Span<T>& v)
  {
    m_ptr = v.m_ptr;
    m_size = v.m_size;
  }

  /*!
   * \brief Pointeur sur le d�but de la vue.
   *
   * \warning Les acc�s via le pointeur retourn� ne pourront pas �tre
   * pas v�rifi�s par Arcane � la diff�rence des acc�s via
   * operator[](): aucune v�rification de d�passement n'est possible,
   * m�me en mode v�rification.
   */
  const_pointer data() const
  { return m_ptr; }

  /*!
   * \brief Pointeur constant sur le d�but de la vue.
   *
   * \warning Les acc�s via le pointeur retourn� ne pourront pas �tre
   * pas v�rifi�s par Arcane � la diff�rence des acc�s via
   * operator[](): aucune v�rification de d�passement n'est possible,
   * m�me en mode v�rification.
   */
  pointer data()
  { return m_ptr; }

 protected:
  
  /*!
   * \brief Modifie le pointeur et la taille du tableau.
   *
   * C'est � la classe d�riv�e de v�rifier la coh�rence entre le pointeur
   * allou� et la dimension donn�e.
   */
  inline void _setArray(T* v,Int64 s){ m_ptr = v; m_size = s; }

  /*!
   * \brief Modifie le pointeur du d�but du tableau.
   *
   * C'est � la classe d�riv�e de v�rifier la coh�rence entre le pointeur
   * allou� et la dimension donn�e.
   */
  inline void _setPtr(T* v) { m_ptr = v; }

  /*!
   * \brief Modifie la taille du tableau.
   *
   * C'est � la classe d�riv�e de v�rifier la coh�rence entre le pointeur
   * allou� et la dimension donn�e.
   */
  inline void _setSize(Int64 s) { m_size = s; }

 private:

  T* m_ptr;  //!< Pointeur sur le tableau
  Int64 m_size; //!< Nombre d'�l�ments du tableau

 private:

  static Int64 _min(Int64 a,Int64 b)
  {
    return ( (a<b) ? a : b );
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline bool
operator==(Span<const T> rhs, Span<const T> lhs)
{
  if (rhs.size()!=lhs.size())
    return false;
  Int64 s = rhs.size();
  for( Int64 i=0; i<s; ++i ){
    if (rhs[i]==lhs[i])
      continue;
    else
      return false;
  }
  return true;
}

template<typename T> inline bool
operator!=(Span<const T> rhs, Span<const T> lhs)
{
  return !(rhs==lhs);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline bool
operator==(Span<T> rhs, Span<T> lhs)
{
  return operator==(rhs.constView(),lhs.constView());
}

template<typename T> inline bool
operator!=(Span<T> rhs, Span<T> lhs)
{
  return !(rhs==lhs);
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Affiche sur le flot \a o les valeurs du tableau \a val.
 *
 * Si \a max_print est positif, alors au plus \a max_print valeurs
 * sont affich�es. Si la taille du tableau est sup�rieure �
 * \a max_print, alors les (max_print/2) premiers et derniers
 * �l�ments sont affich�s.
 */
template<typename T> inline void
dumpArray(std::ostream& o,Span<const T> val,int max_print)
{
  Int64 n = val.size();
  if (max_print>0 && n>max_print){
    // N'affiche que les (max_print/2) premiers et les (max_print/2) derniers
    // sinon si le tableau est tr�s grand cela peut g�n�rer des
    // sorties listings �normes.
    Int64 z = (max_print/2);
    Int64 z2 = n - z;
    o << "[0]=\"" << val[0] << '"';
    for( Int64 i=1; i<z; ++i )
      o << " [" << i << "]=\"" << val[i] << '"';
    o << " ... ... (skipping indexes " << z << " to " << z2 << " ) ... ... ";
    for( Int64 i=(z2+1); i<n; ++i )
      o << " [" << i << "]=\"" << val[i] << '"';
  }
  else{
    for( Int64 i=0; i<n; ++i ){
      if (i!=0)
        o << ' ';
      o << "[" << i << "]=\"" << val[i] << '"';
    }
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline std::ostream&
operator<<(std::ostream& o, Span<const T> val)
{
  dumpArray(o,val,500);
  return o;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline std::ostream&
operator<<(std::ostream& o, Span<T> val)
{
  o << val.constView();
  return o;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif
