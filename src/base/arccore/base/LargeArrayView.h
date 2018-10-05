/*---------------------------------------------------------------------------*/
/* LargeArrayView.h                                            (C) 2000-2018 */
/*                                                                           */
/* Types d�finissant les vues de tableaux C dont la taille est un Int64.     */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_LARGEARRAYVIEW_H
#define ARCCORE_BASE_LARGEARRAYVIEW_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArrayView.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Collection
 * \brief Vue modifiable d'un tableau d'un type \a T.
 *
 Cette classe template permet d'acc�der et d'utiliser un tableau d'�l�ments du
 type \a T de la m�me mani�re qu'un tableau C standard. Elle est similaire �
 ArrayView � ceci pr�s que le nombre d'�l�ments est stock� sur un 'Int64' et
 peut donc d�passer 2Go.
*/
template<class T>
class LargeArrayView
{
 public:

  //! Type des �l�ments du tableau
  typedef T value_type;
  //! Type pointeur d'un �l�ment du tableau
  typedef value_type* pointer;
  //! Type pointeur constant d'un �l�ment du tableau
  typedef const value_type* const_pointer;
  //! Type de l'it�rateur sur un �l�ment du tableau
  typedef ArrayIterator<pointer> iterator;
  //! Type de l'it�rateur constant sur un �l�ment du tableau
  typedef ArrayIterator<const_pointer> const_iterator;
  //! Type r�f�rence d'un �l�ment du tableau
  typedef value_type& reference;
  //! Type r�f�rence constante d'un �l�ment du tableau
  typedef const value_type& const_reference;
  //! Type indexant le tableau
  typedef Int64 size_type;
  //! Type d'une distance entre it�rateur �l�ments du tableau
  typedef std::ptrdiff_t difference_type;

  //! Type d'un it�rateur sur tout le tableau
  typedef IterT< ArrayView<T> > iter;
  //! Type d'un it�rateur constant sur tout le tableau
  typedef ConstIterT< ArrayView<T> > const_iter;

 public:


 public:

  //! Construit une vue vide.
  LargeArrayView() : m_ptr(nullptr), m_size(0) {}
  //! Constructeur de recopie depuis une autre vue
  LargeArrayView(const ArrayView<T>& from)
  : m_ptr(from.m_ptr), m_size(from.m_size) {}
  LargeArrayView(const LargeArrayView<T>& from)
  : m_ptr(from.m_ptr), m_size(from.m_size) {}
  //! Construit une vue sur une zone m�moire commencant par \a ptr et
  // contenant \a asize �l�ments.
  LargeArrayView(Int64 asize,T* ptr)
  : m_ptr(ptr), m_size(asize) {}
  //! Op�rateur de recopie
  const LargeArrayView<T>& operator=(const LargeArrayView<T>& from)
  {
    m_ptr = from.m_ptr;
    m_size = from.m_size;
    return *this;
  }

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
  ConstArrayView<T> constView() const
  {
    return ConstArrayView<T>(m_size,m_ptr);
  }

  /*!
   * \brief Sous-vue � partir de l'�l�ment \a abegin
   * et contenant \a asize �l�ments.
   *
   * Si \a (\a abegin+ \a asize) est sup�rieur � la taille du tableau,
   * la vue est tronqu�e � cette taille, retournant �ventuellement une vue vide.
   */
  LargeArrayView<T> subView(Int64 abegin,Int64 asize)
  {
    if (abegin>=m_size)
      return ArrayView<T>();
    asize = _min(asize,m_size-abegin);
    return ArrayView<T>(asize,m_ptr+abegin);
  }

  /*!
   * \brief Sous-vue constante � partir de
   * l'�l�ment \a abegin et contenant \a asize �l�ments.
   *
   * Si \a (\a abegin+ \a asize) est sup�rieur � la taille du tableau,
   * la vue est tronqu�e � cette taille, retournant �ventuellement une vue vide.
   */
  ConstArrayView<T> subConstView(Int64 abegin,Int64 asize) const
  {
    if (abegin>=m_size)
      return ConstArrayView<T>();
    asize = _min(asize,m_size-abegin);
    return ConstArrayView<T>(asize,m_ptr+abegin);
  }

  //! Sous-vue correspondant � l'interval \a index sur \a nb_interval
  LargeArrayView<T> subViewInterval(Int64 index,Int64 nb_interval)
  {
    Int64 n = m_size;
    Int64 isize = n / nb_interval;
    Int64 ibegin = index * isize;
    // Pour le dernier interval, prend les elements restants
    if ((index+1)==nb_interval)
      isize = n - ibegin;
    return LargeArrayView<T>(isize,m_ptr+ibegin);
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
    const T* copy_begin = copy_array.unguardedBasePointer();
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
  void setArray(const LargeArrayView<T>& v)
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
/*!
 * \ingroup Collection 
 * \brief Vue constante d'un tableau de type \a T.
 *
 * Cette classe fonctionne de la m�me mani�re que LargeArrayView � la seule
 * diff�rence qu'il n'est pas possible de modifier les �l�ments du tableau.
 */
template<class T>
class ConstLargeArrayView
{
 public:

  //! Type des �l�ments du tableau
  typedef T value_type;
  //! Type pointeur constant d'un �l�ment du tableau
  typedef const value_type* const_pointer;
  //! Type de l'it�rateur constant sur un �l�ment du tableau
  typedef ArrayIterator<const_pointer> const_iterator;
  //! Type r�f�rence constante d'un �l�ment du tableau
  typedef const value_type& const_reference;
  //! Type indexant le tableau
  typedef Int64 size_type;
  //! Type d'une distance entre it�rateur �l�ments du tableau
  typedef std::ptrdiff_t difference_type;

 public:

  //! Construit un tableau vide.
  ConstLargeArrayView() : m_ptr(nullptr), m_size(0) {}
  //! Construit un tableau avec \a s �l�ment
  ConstLargeArrayView(Int64 s,const T* ptr)
  : m_ptr(ptr), m_size(s) {}
  /*! \brief Constructeur par copie.
   * \warning Seul le pointeur est copi�. Aucune copie m�moire n'est effectu�e.
   */
  ConstLargeArrayView(const ConstLargeArrayView<T>& from)
  : m_ptr(from.m_ptr), m_size(from.m_size) {}
  /*! \brief Constructeur par copie.
   * \warning Seul le pointeur est copi�. Aucune copie m�moire n'est effectu�e.
   */
  ConstLargeArrayView(const LargeArrayView<T>& from)
  : m_ptr(from.data()), m_size(from.size()) { }
  /*! \brief Constructeur par copie.
   * \warning Seul le pointeur est copi�. Aucune copie m�moire n'est effectu�e.
   */
  ConstLargeArrayView(const ConstArrayView<T>& from)
  : m_ptr(from.data()), m_size(from.size()) { }
  /*! \brief Constructeur par copie.
   * \warning Seul le pointeur est copi�. Aucune copie m�moire n'est effectu�e.
   */
  ConstLargeArrayView(const ArrayView<T>& from)
  : m_ptr(from.data()), m_size(from.size()) { }

  /*!
   * \brief Op�rateur de recopie.
   * \warning Seul le pointeur est copi�. Aucune copie m�moire n'est effectu�e.
   */
  const ConstLargeArrayView<T>& operator=(const ConstLargeArrayView<T>& from)
  {
    m_ptr=from.m_ptr;
    m_size=from.m_size;
    return *this;
  }

  /*! \brief Op�rateur de recopie.
   * \warning Seul le pointeur est copi�. Aucune copie m�moire n'est effectu�e.
   */
  const ConstLargeArrayView<T>& operator=(const LargeArrayView<T>& from)
  {
    m_ptr  = from.data();
    m_size = from.size();
    return (*this);
  }

  /*! \brief Op�rateur de recopie.
   * \warning Seul le pointeur est copi�. Aucune copie m�moire n'est effectu�e.
   */
  const ConstLargeArrayView<T>& operator=(const ConstArrayView<T>& from)
  {
    m_ptr  = from.data();
    m_size = from.size();
    return (*this);
  }

  /*! \brief Op�rateur de recopie.
   * \warning Seul le pointeur est copi�. Aucune copie m�moire n'est effectu�e.
   */
  const ConstLargeArrayView<T>& operator=(const ArrayView<T>& from)
  {
    m_ptr  = from.data();
    m_size = from.size();
    return (*this);
  }

 public:

  /*!
   * \brief Sous-vue (constante) � partir de l'�l�ment \a abegin et
   contenant \a asize �l�ments.
   *
   * Si \a (abegin+asize) est sup�rieur � la taille du tableau,
   * la vue est tronqu� � cette taille, retournant �ventuellement une vue vide.
   */
  ConstLargeArrayView<T> subView(Int64 abegin,Int64 asize) const
  {
    if (abegin>=m_size)
      return ConstLargeArrayView<T>();
    asize = _min(asize,m_size-abegin);
    return ConstLargeArrayView<T>(asize,m_ptr+abegin);
  }

  /*!
   * \brief Sous-vue (constante) � partir de l'�l�ment \a abegin et
   * contenant \a asize �l�ments.
   *
   * Si \a (abegin+asize) est sup�rieur � la taille du tableau,
   * la vue est tronqu� � cette taille, retournant �ventuellement une vue vide.
   */
  ConstLargeArrayView<T> subConstView(Int64 abegin,Int64 asize) const
  {
    return subView(abegin,asize);
  }

  //! Sous-vue correspondant � l'interval \a index sur \a nb_interval
  ArrayView<T> subViewInterval(Int64 index,Int64 nb_interval)
  {
    Int64 n = m_size;
    Int64 isize = n / nb_interval;
    Int64 ibegin = index * isize;
    // Pour le dernier interval, prend les elements restants
    if ((index+1)==nb_interval)
      isize = n - ibegin;
    ARCCORE_CHECK_AT(ibegin+isize,n);
    return ConstLargeArrayView<T>(isize,m_ptr+ibegin);
  }

  //! Addresse du index-�me �l�ment
  inline const T* ptrAt(Int64 index) const
  {
    ARCCORE_CHECK_AT(index,m_size);
    return m_ptr+index;
  }

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  const T& operator[](Int64 i) const
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

  //! Nombre d'�l�ments du tableau
  inline Int64 size() const { return m_size; }
  //! Nombre d'�l�ments du tableau
  inline Int64 length() const { return m_size; }
  //! It�rateur sur le premier �l�ment du tableau.
  const_iterator begin() const { return const_iterator(m_ptr); }
  //! It�rateur sur le premier �l�ment apr�s la fin du tableau.
  const_iterator end() const { return const_iterator(m_ptr+m_size); }
  //! \a true si le tableau est vide (size()==0)
  inline bool empty() const { return m_size==0; }
  //! \a true si le tableau contient l'�l�ment de valeur \a v
  bool contains(const T& v) const
  {
    for( Int64 i=0; i<m_size; ++i ){
      if (m_ptr[i]==v)
        return true;
    }
    return false;
  }
  void setArray(const ConstArrayView<T>& v)
  {
    m_ptr = v.m_ptr;
    m_size = v.m_size;
  }
  void setArray(const ConstLargeArrayView<T>& v)
  {
    m_ptr = v.m_ptr;
    m_size = v.m_size;
  }

  /*!
   * \brief Pointeur sur la m�moire allou�e.
   *
   * \warning Les acc�s via le pointeur retourn� ne pourront pas �tre
   * pas v�rifi�s par Arcane � la diff�rence des acc�s via
   * operator[](): aucune v�rification de d�passement n'est possible,
   * m�me en mode v�rification.
   */
  const T* data() const  { return m_ptr; }
  //! Intervalle d'it�ration du premier au derni�r �l�ment.
  ArrayRange<const_pointer> range() const
  {
    return ArrayRange<const_pointer>(m_ptr,m_ptr+m_size);
  }

 private:

  const T* m_ptr; //!< Pointeur sur le d�but du tableau
  Int64 m_size; //!< Nombre d'�l�ments 

 private:

  static Int64 _min(Int64 a,Int64 b)
  {
    return ( (a<b) ? a : b );
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline bool
operator==(ConstLargeArrayView<T> rhs, ConstLargeArrayView<T> lhs)
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
operator!=(ConstLargeArrayView<T> rhs, ConstLargeArrayView<T> lhs)
{
  return !(rhs==lhs);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline bool
operator==(LargeArrayView<T> rhs, LargeArrayView<T> lhs)
{
  return operator==(rhs.constView(),lhs.constView());
}

template<typename T> inline bool
operator!=(LargeArrayView<T> rhs, LargeArrayView<T> lhs)
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
dumpArray(std::ostream& o,ConstLargeArrayView<T> val,int max_print)
{
  Int64 n = val.size();
  if (max_print>0 && n>max_print){
    // N'affiche que les (max_print/2) premiers et les (max_print/2) derni�rs
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
operator<<(std::ostream& o, ConstLargeArrayView<T> val)
{
  dumpArray(o,val,500);
  return o;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline std::ostream&
operator<<(std::ostream& o, LargeArrayView<T> val)
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
