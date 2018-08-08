/*---------------------------------------------------------------------------*/
/* ArrayView.h                                                 (C) 2000-2018 */
/*                                                                           */
/* Types d�finissant les vues de tableaux C.                                 */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_ARRAYVIEW_H
#define ARCCORE_BASE_ARRAYVIEW_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArrayRange.h"

#include <iosfwd>
#include <cstddef>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> class ConstArrayView;
template<typename T> class ConstIterT;
template<typename T> class IterT;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Collection
 * \brief Vue modifiable d'un tableau d'un type \a T.
 *
 Cette classe template permet d'acc�der et d'utiliser un tableau d'�l�ments du
 type \a T de la m�me mani�re qu'un tableau C standard. Elle maintient en
 plus la taille du tableau. La fonction size() permet de conna�tre le nombre
 d'�l�ments du tableau et l'op�rateur operator[]() permet d'acc�der � un �l�ment donn�.

 Il est garanti que tous les �l�ments de la vue sont cons�cutifs en m�moire.

 Cette classe ne g�re aucune m�moire et c'est le conteneur associ� qui la g�re.
 Les conteneurs possibles fournis par %Arccore sont les classes Array,
 UniqueArray ou SharedArray. Une vue n'est valide que tant que le conteneur associ� n'est pas r�allou�.
 De m�me, le contructeur et l'op�rateur de recopie ne font que recopier les pointeurs
 sans r�allouer la m�moire. Il faut donc les utiliser avec pr�caution.
 
 Si %Arccore est compil� en mode v�rification (ARCCORE_CHECK est d�fini), les acc�s
 par l'interm�diaire de l'op�rateurs operator[]() sont v�rifi�s et une
 exception IndexOutOfRangeException est lanc� si un d�bordement de
 tableau a lieu. En attachant une session de debug au processus, il est
 possible de voir la pile d'appel au moment du d�bordement.

 Voici des exemples d'utilisation:

 \code
 Real t[5];
 ArrayView<Real> a(t,5); // G�re un tableau de 5 r�els.
 Integer i = 3;
 Real v = a[2]; // Affecte � la valeur du 2�me �l�ment
 a[i] = 5; // Affecte au 3�me �l�ment la valeur 5
 \endcode

 Il est aussi possible d'acc�der aux �l�ments du tableau par des
 it�rateurs de la m�me mani�re qu'avec les containers de la STL.
 Dans ce, il faut utiliser la m�thode range() qui poss�de les
 m�thodes classiques pour r�cup�rer des it�rateurs (begin()/end()).

 L'exemple suivant cr�� un it�rateur \e i sur le tableau \e a et it�re
 sur tout le tableau (m�thode i()) et affiche les �l�ments:

 \code
 * for( Real v : a.range() )
 *   cout << v;
 \endcode

 L'exemple suivant fait la somme des 3 premiers �l�ments du tableau:

 \code
 * Real sum = 0.;
 * for( Real v : a.subView(0,3).range() )
 *   sum += v;
 \endcode

*/
template<class T>
class ArrayView
{
 public:

  //! Type des �l�ments du tableau
  typedef T value_type;
  //! Type pointeur d'un �l�ment du tableau
  typedef value_type* pointer;
  //! Type pointeur constant d'un �l�ment du tableau
  typedef const value_type* const_pointer;
  //! Type de l'it�rateur sur un �l�ment du tableau
  typedef value_type* iterator;
  //! Type de l'it�rateur constant sur un �l�ment du tableau
  typedef const value_type* const_iterator;
  //! Type r�f�rence d'un �l�ment du tableau
  typedef value_type& reference;
  //! Type r�f�rence constante d'un �l�ment du tableau
  typedef const value_type& const_reference;
  //! Type indexant le tableau
  typedef Integer size_type;
  //! Type d'une distance entre it�rateur �l�ments du tableau
  typedef std::ptrdiff_t difference_type;

  //! Type d'un it�rateur sur tout le tableau
  typedef IterT< ArrayView<T> > iter;
  //! Type d'un it�rateur constant sur tout le tableau
  typedef ConstIterT< ArrayView<T> > const_iter;

 public:


 public:

  //! Construit une vue vide.
  ArrayView() : m_size(0), m_ptr(0) {}
  //! Constructeur de recopie depuis une autre vue
  ArrayView(const ArrayView<T>& from)
  : m_size(from.m_size), m_ptr(from.m_ptr) {}
  //! Construit une vue sur une zone m�moire commencant par \a ptr et
  // contenant \a asize �l�ments.
  explicit ArrayView(Integer asize,T* ptr) : m_size(asize), m_ptr(ptr) {}
  //! Op�rateur de recopie
  const ArrayView<T>& operator=(const ArrayView<T>& from)
    { m_size=from.m_size; m_ptr=from.m_ptr; return *this; }

 public:

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline T& operator[](Integer i)
  {
    ARCCORE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline const T& operator[](Integer i) const
  {
    ARCCORE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline const T& item(Integer i) const
  {
    ARCCORE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  /*!
   * \brief Positionne le i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline void setItem(Integer i,const T& v)
  {
    ARCCORE_CHECK_AT(i,m_size);
    m_ptr[i] = v;
  }

  //! Retourne la taille du tableau
  inline Integer size() const { return m_size; }
  //! Nombre d'�l�ments du tableau
  inline Integer length() const { return m_size; }

  /*!
   * \brief It�rateur sur le premier �l�ment du tableau.
   * \deprecated Utiliser range().begin(), std::begin() ou data() � la place.
   */
  ARCCORE_DEPRECATED_2018 iterator begin() { return iterator(m_ptr); }
  /*!
   * \brief It�rateur sur le premier �l�ment apr�s la fin du tableau.
   * \deprecated Utiliser range().end() � la place.
   */
  ARCCORE_DEPRECATED_2018 iterator end() { return iterator(m_ptr+m_size); }
  /*!
   * \brief It�rateur constant sur le premier �l�ment du tableau.
   * \deprecated Utiliser range().begin(), std::begin() ou data() � la place.
   */
  ARCCORE_DEPRECATED_2018 const_iterator begin() const { return iterator(m_ptr); }
  /*!
   * \brief It�rateur constant sur le premier �l�ment apr�s la fin du tableau.
   * \deprecated Utiliser range().end() � la place.
   */
  ARCCORE_DEPRECATED_2018 const_iterator end() const { return iterator(m_ptr+m_size); }

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
  inline T* ptrAt(Integer index)
  {
    ARCCORE_CHECK_AT(index,m_size);
    return m_ptr+index;
  }

  //! Addresse du index-�me �l�ment
  inline const T* ptrAt(Integer index) const
  {
    ARCCORE_CHECK_AT(index,m_size);
    return m_ptr+index;
  }

  // El�ment d'indice \a i. V�rifie toujours les d�bordements
  const T& at(Integer i) const
  {
    arccoreCheckAt(i,m_size);
    return m_ptr[i];
  }

  // Positionne l'�l�ment d'indice \a i. V�rifie toujours les d�bordements
  void setAt(Integer i,const T& value)
  {
    arccoreCheckAt(i,m_size);
    m_ptr[i] = value;
  }

  //! Remplit le tableau avec la valeur \a o
  inline void fill(T o)
  {
    for( Integer i=0, n=m_size; i<n; ++i )
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
  ArrayView<T> subView(Integer abegin,Integer asize)
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
  ConstArrayView<T> subConstView(Integer abegin,Integer asize) const
  {
    if (abegin>=m_size)
      return ConstArrayView<T>();
    asize = _min(asize,m_size-abegin);
    return ConstArrayView<T>(asize,m_ptr+abegin);
  }

  //! Sous-vue correspondant � l'interval \a index sur \a nb_interval
  ArrayView<T> subViewInterval(Integer index,Integer nb_interval)
  {
    Integer n = m_size;
    Integer isize = n / nb_interval;
    Integer ibegin = index * isize;
    // Pour le dernier interval, prend les elements restants
    if ((index+1)==nb_interval)
      isize = n - ibegin;
    return ArrayView<T>(isize,m_ptr+ibegin);
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
    Integer n = copy_array.size();
    for( Integer i=0; i<n; ++i )
      to_ptr[i] = copy_begin[i];
  }

  //! Retourne \a true si le tableau est vide (dimension nulle)
  bool empty() const { return m_size==0; }
  //! \a true si le tableau contient l'�l�ment de valeur \a v
  bool contains(const T& v) const
  {
    for( Integer i=0; i<m_size; ++i ){
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

  /*!
   * \brief Pointeur sur le d�but de la vue.
   *
   * \warning Les acc�s via le pointeur retourn� ne pourront pas �tre
   * pas v�rifi�s par Arcane � la diff�rence des acc�s via
   * operator[](): aucune v�rification de d�passement n'est possible,
   * m�me en mode v�rification.
   */
  inline T* unguardedBasePointer()
  { return m_ptr; }

  /*!
   * \brief Pointeur constant sur le d�but de la vue.
   *
   * \warning Les acc�s via le pointeur retourn� ne pourront pas �tre
   * pas v�rifi�s par Arcane � la diff�rence des acc�s via
   * operator[](): aucune v�rification de d�passement n'est possible,
   * m�me en mode v�rification.
   */
  inline const T* unguardedBasePointer() const
  { return m_ptr; }

  /*!
   * \brief Pointeur sur le d�but de la vue.
   *
   * \warning Les acc�s via le pointeur retourn� ne pourront pas �tre
   * pas v�rifi�s par Arcane � la diff�rence des acc�s via
   * operator[](): aucune v�rification de d�passement n'est possible,
   * m�me en mode v�rification.
   */
  const T* data() const
  { return m_ptr; }

  /*!
   * \brief Pointeur constant sur le d�but de la vue.
   *
   * \warning Les acc�s via le pointeur retourn� ne pourront pas �tre
   * pas v�rifi�s par Arcane � la diff�rence des acc�s via
   * operator[](): aucune v�rification de d�passement n'est possible,
   * m�me en mode v�rification.
   */
  T* data()
  { return m_ptr; }

 protected:

  /*!
   * \brief Retourne un pointeur sur le tableau.
   *
   * Cette m�thode est identique � unguardedBasePointer() (i.e: il faudra
   * penser � la supprimer)
   */
  inline T* _ptr() { return m_ptr; }
  /*!
   * \brief Retourne un pointeur sur le tableau
   *
   * Cette m�thode est identique � unguardedBasePointer() (i.e: il faudra
   * penser � la supprimer)
   */
  inline const T* _ptr() const { return m_ptr; }
  
  /*!
   * \brief Modifie le pointeur et la taille du tableau.
   *
   * C'est � la classe d�riv�e de v�rifier la coh�rence entre le pointeur
   * allou� et la dimension donn�e.
   */
  inline void _setArray(T* v,Integer s){ m_ptr = v; m_size = s; }

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
  inline void _setSize(Integer s) { m_size = s; }

 private:

  Integer m_size; //!< Nombre d'�l�ments du tableau
  T* m_ptr;  //!< Pointeur sur le tableau

 private:

  static Integer _min(Integer a,Integer b)
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
 * Cette classe fonctionne de la m�me mani�re que ArrayView � la seule
 * diff�rence qu'il n'est pas possible de modifier les �l�ments du tableau.
 */
template<class T>
class ConstArrayView
{
 public:

  //! Type des �l�ments du tableau
  typedef T value_type;
  //! Type pointeur constant d'un �l�ment du tableau
  typedef const value_type* const_pointer;
  //! Type de l'it�rateur constant sur un �l�ment du tableau
  typedef const value_type* const_iterator;
  //! Type r�f�rence constante d'un �l�ment du tableau
  typedef const value_type& const_reference;
  //! Type indexant le tableau
  typedef Integer size_type;
  //! Type d'une distance entre it�rateur �l�ments du tableau
  typedef std::ptrdiff_t difference_type;

  //! Type d'un it�rateur constant sur tout le tableau
  typedef ConstIterT< ConstArrayView<T> > const_iter;

 public:

  //! Construit un tableau vide.
  ConstArrayView() : m_size(0), m_ptr(nullptr) {}
  //! Construit un tableau avec \a s �l�ment
  explicit ConstArrayView(Integer s,const T* ptr)
  : m_size(s), m_ptr(ptr) {}
  /*! \brief Constructeur par copie.
   * \warning Seul le pointeur est copi�. Aucune copie m�moire n'est effectu�e.
   */
  ConstArrayView(const ConstArrayView<T>& from)
  : m_size(from.m_size), m_ptr(from.m_ptr) {}
  /*! \brief Constructeur par copie.
   * \warning Seul le pointeur est copi�. Aucune copie m�moire n'est effectu�e.
   */
  ConstArrayView(const ArrayView<T>& from)
  : m_size(from.size()), m_ptr(from.data())
    {
    }

  /*!
   * \brief Op�rateur de recopie.
   * \warning Seul le pointeur est copi�. Aucune copie m�moire n'est effectu�e.
   */
  const ConstArrayView<T>& operator=(const ConstArrayView<T>& from)
    { m_size=from.m_size; m_ptr=from.m_ptr; return *this; }

  /*! \brief Op�rateur de recopie.
   * \warning Seul le pointeur est copi�. Aucune copie m�moire n'est effectu�e.
   */
  const ConstArrayView<T>& operator=(const ArrayView<T>& from)
    {
      m_size = from.size();
      m_ptr  = from.data();
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
  ConstArrayView<T> subView(Integer abegin,Integer asize) const
  {
    if (abegin>=m_size)
      return ConstArrayView<T>();
    asize = _min(asize,m_size-abegin);
    return ConstArrayView<T>(asize,m_ptr+abegin);
  }

  /*!
   * \brief Sous-vue (constante) � partir de l'�l�ment \a abegin et
   * contenant \a asize �l�ments.
   *
   * Si \a (abegin+asize) est sup�rieur � la taille du tableau,
   * la vue est tronqu� � cette taille, retournant �ventuellement une vue vide.
   */
  ConstArrayView<T> subConstView(Integer abegin,Integer asize) const
  {
    return subView(abegin,asize);
  }

  //! Sous-vue correspondant � l'interval \a index sur \a nb_interval
  ArrayView<T> subViewInterval(Integer index,Integer nb_interval)
  {
    Integer n = m_size;
    Integer isize = n / nb_interval;
    Integer ibegin = index * isize;
    // Pour le dernier interval, prend les elements restants
    if ((index+1)==nb_interval)
      isize = n - ibegin;
    ARCCORE_CHECK_AT(ibegin+isize,n);
    return ConstArrayView<T>(isize,m_ptr+ibegin);
  }

  //! Addresse du index-�me �l�ment
  inline const T* ptrAt(Integer index) const
  {
    ARCCORE_CHECK_AT(index,m_size);
    return m_ptr+index;
  }

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  const T& operator[](Integer i) const
  {
    ARCCORE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline const T& item(Integer i) const
  {
    ARCCORE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  //! Nombre d'�l�ments du tableau
  inline Integer size() const { return m_size; }
  //! Nombre d'�l�ments du tableau
  inline Integer length() const { return m_size; }
  /*!
   * \brief It�rateur sur le premier �l�ment du tableau.
   * \deprecated Utiliser range().begin(), std::begin() ou data() � la place.
   */
  ARCCORE_DEPRECATED_2018 const_iterator begin() const { return const_iterator(m_ptr); }
  /*!
   * \brief It�rateur sur le premier �l�ment apr�s la fin du tableau.
   * \deprecated Utiliser range().end() � la place.
   */
  ARCCORE_DEPRECATED_2018 const_iterator end() const { return const_iterator(m_ptr+m_size); }
  //! \a true si le tableau est vide (size()==0)
  inline bool empty() const { return m_size==0; }
  //! \a true si le tableau contient l'�l�ment de valeur \a v
  bool contains(const T& v) const
  {
    for( Integer i=0; i<m_size; ++i ){
      if (m_ptr[i]==v)
        return true;
    }
    return false;
  }
  void setArray(const ConstArrayView<T>& v) { m_ptr = v.m_ptr; m_size = v.m_size; }

  /*!
   * \brief Pointeur sur la m�moire allou�e.
   *
   * \warning Les acc�s via le pointeur retourn� ne pourront pas �tre
   * pas v�rifi�s par Arcane � la diff�rence des acc�s via
   * operator[](): aucune v�rification de d�passement n'est possible,
   * m�me en mode v�rification.
   */
  inline const T* unguardedBasePointer() const
  { return m_ptr; }

  /*!
   * \brief Pointeur sur la m�moire allou�e.
   *
   * \warning Les acc�s via le pointeur retourn� ne pourront pas �tre
   * pas v�rifi�s par Arcane � la diff�rence des acc�s via
   * operator[](): aucune v�rification de d�passement n'est possible,
   * m�me en mode v�rification.
   */
  const T* data() const
  { return m_ptr; }
  //! Intervalle d'it�ration du premier au derni�r �l�ment.
  ArrayRange<const_pointer> range() const
  {
    return ArrayRange<const_pointer>(m_ptr,m_ptr+m_size);
  }

 private:

  Integer m_size; //!< Nombre d'�l�ments 
  const T* m_ptr; //!< Pointeur sur le d�but du tableau

 private:

  static Integer _min(Integer a,Integer b)
  {
    return ( (a<b) ? a : b );
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline bool
operator==(ConstArrayView<T> rhs, ConstArrayView<T> lhs)
{
  if (rhs.size()!=lhs.size())
    return false;
  Integer s = rhs.size();
  for( Integer i=0; i<s; ++i ){
    if (rhs[i]==lhs[i])
      continue;
    else
      return false;
  }
  return true;
}

template<typename T> inline bool
operator!=(ConstArrayView<T> rhs, ConstArrayView<T> lhs)
{
  return !(rhs==lhs);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline bool
operator==(ArrayView<T> rhs, ArrayView<T> lhs)
{
  return operator==(rhs.constView(),lhs.constView());
}

template<typename T> inline bool
operator!=(ArrayView<T> rhs,ArrayView<T> lhs)
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
dumpArray(std::ostream& o,ConstArrayView<T> val,int max_print)
{
  Integer n = val.size();
  if (max_print>0 && n>max_print){
    // N'affiche que les (max_print/2) premiers et les (max_print/2) derni�rs
    // sinon si le tableau est tr�s grand cela peut g�n�rer des
    // sorties listings �normes.
    Integer z = (max_print/2);
    Integer z2 = n - z;
    o << "[0]=\"" << val[0] << '"';
    for( Integer i=1; i<z; ++i )
      o << " [" << i << "]=\"" << val[i] << '"';
    o << " ... ... (skipping indexes " << z << " to " << z2 << " ) ... ... ";
    for( Integer i=(z2+1); i<n; ++i )
      o << " [" << i << "]=\"" << val[i] << '"';
  }
  else{
    for( Integer i=0; i<n; ++i ){
      if (i!=0)
        o << ' ';
      o << "[" << i << "]=\"" << val[i] << '"';
    }
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline std::ostream&
operator<<(std::ostream& o, ConstArrayView<T> val)
{
  dumpArray(o,val,500);
  return o;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline std::ostream&
operator<<(std::ostream& o, ArrayView<T> val)
{
  o << val.constView();
  return o;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief V�rifie que \a size peut �tre converti dans un 'Integer' pour servir
 * de taille � un tableau.
 * Si possible, retourne \a size convertie en un 'Integer'. Sinon, lance
 * une exception de type ArgumentException.
 */
extern "C++" ARCCORE_BASE_EXPORT Integer
arccoreCheckArraySize(unsigned long long size);

/*!
 * \brief V�rifie que \a size peut �tre converti dans un 'Integer' pour servir
 * de taille � un tableau.
 * Si possible, retourne \a size convertie en un 'Integer'. Sinon, lance
 * une exception de type ArgumentException.
 */
extern "C++" ARCCORE_BASE_EXPORT Integer
arccoreCheckArraySize(long long size);

/*!
 * \brief V�rifie que \a size peut �tre converti dans un 'Integer' pour servir
 * de taille � un tableau.
 * Si possible, retourne \a size convertie en un 'Integer'. Sinon, lance
 * une exception de type ArgumentException.
 */
extern "C++" ARCCORE_BASE_EXPORT Integer
arccoreCheckArraySize(unsigned long size);

/*!
 * \brief V�rifie que \a size peut �tre converti dans un 'Integer' pour servir
 * de taille � un tableau.
 * Si possible, retourne \a size convertie en un 'Integer'. Sinon, lance
 * une exception de type ArgumentException.
 */
extern "C++" ARCCORE_BASE_EXPORT Integer
arccoreCheckArraySize(long size);

/*!
 * \brief V�rifie que \a size peut �tre converti dans un 'Integer' pour servir
 * de taille � un tableau.
 * Si possible, retourne \a size convertie en un 'Integer'. Sinon, lance
 * une exception de type ArgumentException.
 */
extern "C++" ARCCORE_BASE_EXPORT Integer
arccoreCheckArraySize(unsigned int size);

/*!
 * \brief V�rifie que \a size peut �tre converti dans un 'Integer' pour servir
 * de taille � un tableau.
 * Si possible, retourne \a size convertie en un 'Integer'. Sinon, lance
 * une exception de type ArgumentException.
 */
extern "C++" ARCCORE_BASE_EXPORT Integer
arccoreCheckArraySize(int size);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace std
{
template< class T > inline typename Arccore::ArrayRange<typename Arccore::ArrayView<T>::pointer>::iterator
begin( Arccore::ArrayView<T>& c )
{
  return c.range().begin();
}
template< class T > inline typename Arccore::ArrayRange<typename Arccore::ArrayView<T>::pointer>::iterator
end( Arccore::ArrayView<T>& c )
{
  return c.range().end();
}

template< class T > inline typename Arccore::ArrayRange<typename Arccore::ArrayView<T>::const_pointer>::const_iterator
begin( const Arccore::ConstArrayView<T>& c )
{
  return c.range().begin();
}
template< class T > inline typename Arccore::ArrayRange<typename Arccore::ArrayView<T>::const_pointer>::const_iterator
end( const Arccore::ConstArrayView<T>& c )
{
  return c.range().end();
}
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif
