/*---------------------------------------------------------------------------*/
/* ArrayView.h                                                 (C) 2000-2014 */
/*                                                                           */
/* Types d�finissant les vues de tableaux C.                                 */
/*---------------------------------------------------------------------------*/
#ifndef ARCANE_UTILS_ARRAYVIEW_H
#define ARCANE_UTILS_ARRAYVIEW_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/Math.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> class ConstArrayView;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Vue d'un tableau d'un type \a T.
 *
 * \ingroup Collection
 *
 Cette classe template permet d'acc�der et d'utiliser un tableau d'�l�ments du
 type \a T de la m�me mani�re qu'un tableau C standard. Elle maintient en
 plus la taille du tableau. La fonction
 size() permet de conna�tre le nombre d'�l�ments du tableau et l'op�rateur
 [] permet d'acc�der � un �l�ment donn�.

 Cette classe ne g�re aucune m�moire et c'est aux classes d�riv�es
 (ArrayAllocT,...) de g�rer la m�moire et de notifier l'instance par setPtr().

 Le contructeur et l'op�rateur de recopie ne font que recopier les pointeurs
 sans r�allouer la m�moire. Il faut donc les utiliser avec pr�caution.
 

 En mode d�bug, les acc�s par l'interm�diaire des op�rateurs [] sont
 v�rifi�s et le programme se met en pause automatiquement si un d�bordement de
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

 L'exemple suivant cr�� un it�rateur \e i sur le tableau \e a et it�re
 sur tout le tableau (m�thode i()) et affiche les �l�ments:

 \code
 for( ArrayView<Real>::const_iter i(a); i(); ++i )
 cout << *i;  
 \endcode

 L'exemple suivant fait la somme des 3 premiers �l�ments du tableau:

 \code
 Real sum = 0.;
 ArrayView<Real>::iterator b = a.begin();     // It�rateur sur le d�but du tableau
 ArrayView<Real>::iterator e = a.begin() + 3; // It�rateur sur le 4�me �l�ment du tableau
 for( ; b!=e; ++b )
 sum += *i;
 \endcode

*/
template<class T>
class ArrayView
{
 public:
	
  //! Type des �l�ments du tableau
  typedef T value_type;
  //! Type de l'it�rateur sur un �l�ment du tableau
  typedef value_type* iterator;
  //! Type de l'it�rateur constant sur un �l�ment du tableau
  typedef const value_type* const_iterator;
  //! Type pointeur d'un �l�ment du tableau
  typedef value_type* pointer;
  //! Type pointeur constant d'un �l�ment du tableau
  typedef const value_type* const_pointer;
  //! Type r�f�rence d'un �l�ment du tableau
  typedef value_type& reference;
  //! Type r�f�rence constante d'un �l�ment du tableau
  typedef const value_type& const_reference;
  //! Type indexant le tableau
  typedef Integer size_type;
  //! Type d'une distance entre it�rateur �l�ments du tableau
  typedef ptrdiff_t difference_type;

 public:


 public:

  //! Construit un tableau vide.
  ArrayView() : m_size(0), m_ptr(0) {}
  //! Construit un tableau de dimension \a size
  explicit ArrayView(Integer s,T* ptr) : m_size(s), m_ptr(ptr) {}
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
    ARCANE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline const T& operator[](Integer i) const
  {
    ARCANE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline const T& item(Integer i) const
  {
    ARCANE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  /*!
   * \brief Positionne le i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline void setItem(Integer i,const T& v)
  {
    ARCANE_CHECK_AT(i,m_size);
    m_ptr[i] = v;
  }

  //! Retourne la taille du tableau
  inline Integer size() const { return m_size; }
  //! Nombre d'�l�ments du tableau
  inline Integer length() const { return m_size; }
  
  //! Retourne un iterateur sur le premier �l�ment du tableau
  inline iterator begin() { return m_ptr; }
  //! Retourne un iterateur sur le premier �l�ment apr�s la fin du tableau
  inline iterator end() { return m_ptr+m_size; }
  //! Retourne un iterateur constant sur le premier �l�ment du tableau
  inline const_iterator begin() const { return m_ptr; }
  //! Retourne un iterateur constant sur le premier �l�ment apr�s la fin du tableau
  inline const_iterator end() const { return m_ptr+m_size; }

  //! Addresse du index-�me �l�ment
  inline T* ptrAt(Integer index)
  {
    ARCANE_CHECK_AT(index,m_size);
    return m_ptr+index;
  }

  //! Addresse du index-�me �l�ment
  inline const T* ptrAt(Integer index) const
  {
    ARCANE_CHECK_AT(index,m_size);
    return m_ptr+index;
  }

  // El�ment d'indice \a i. V�rifie toujours les d�bordements
  const T& at(Integer i) const
  {
    arcaneCheckAt(i,m_size);
    return m_ptr[i];
  }

  // Positionne l'�l�ment d'indice \a i. V�rifie toujours les d�bordements
  void setAt(Integer i,const T& value)
  {
    arcaneCheckAt(i,m_size);
    m_ptr[i] = value;
  }

  //! Remplit le tableau avec la valeur \a o
  inline void fill(T o)
  {
    for( Integer i=0, size=m_size; i<size; ++i )
      m_ptr[i] = o;
  }

  /*!
   * \brief Sous-vue � partir de l'�l�ment \a begin et contenant \a size �l�ments.
   *
   * Si \a (begin+size) est sup�rieur � la taille du tableau,
   * la vue est tronqu� � cette taille, retournant �ventuellement une vue vide.
   */
  ArrayView<T> subView(Integer begin,Integer size)
  {
    if (begin>=m_size)
      return ArrayView<T>();
    size = math::min(size,m_size-begin);
    return ArrayView<T>(size,m_ptr+begin);
  }

  /*!
   * \brief Sous-vue constante � partir de l'�l�ment \a begin et contenant \a size �l�ments.
   *
   * Si \a (begin+size) est sup�rieur � la taille du tableau,
   * la vue est tronqu� � cette taille, retournant �ventuellement une vue vide.
   */
  ConstArrayView<T> subConstView(Integer begin,Integer size) const
  {
    if (begin>=m_size)
      return ConstArrayView<T>();
    size = math::min(size,m_size-begin);
    return ConstArrayView<T>(size,m_ptr+begin);
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
      ARCANE_ASSERT( (copy_array.size()<=m_size), ("Bad size %d %d",copy_array.size(),m_size) );
      const T* copy_begin = copy_array.begin();
      T* to_ptr = m_ptr;
      Integer size = copy_array.size();
      for( Integer i=0; i<size; ++i )
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
    { m_ptr = v.m_ptr; m_size = v.m_size; }

  /*!
   * \brief Pointeur sur la m�moire allou�e.
   *
   * \warning Cette m�thode est dangereuse. Le pointeur retourn� peut �tre
   * invalid� d�s que le nombre d'�l�ments du tableau change.
   */
  inline T* unguardedBasePointer()
  { return m_ptr; }

  /*!
   * \brief Pointeur sur la m�moire allou�e.
   *
   * \warning Cette m�thode est dangereuse. Le pointeur retourn� peut �tre
   * invalid� d�s que le nombre d'�l�ments du tableau change.
   */
  inline const T* unguardedBasePointer() const
  { return m_ptr; }

 protected:
	
  /*!
   * \brief Retourne un pointeur sur le tableau
   *
   * \warning Il est pr�f�rable de ne pas utiliser cette m�thode pour
   * acc�der � un �l�ment du tableau car
   * ce pointeur peut �tre invalid� par un redimensionnement du tableau.
   * De plus, acc�der aux �l�ments du tableau par ce pointeur ne permet
   * aucune v�rification de d�passement, m�me en mode DEBUG.
   */
  inline T* _ptr() { return m_ptr; }
  /*!
   * \brief Retourne un pointeur sur le tableau
   *
   * \warning Il est pr�f�rable de ne pas utiliser cette m�thode pour
   * acc�der � un �l�ment du tableau car
   * ce pointeur peut �tre invalid� par un redimensionnement du tableau.
   * De plus, acc�der aux �l�ments du tableau par ce pointeur ne permet
   * aucune v�rification de d�passement, m�me en mode DEBUG.
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
  inline void _setPtr(T* v)
    { m_ptr = v; }

  /*!
   * \brief Modifie la taille du tableau.
   *
   * C'est � la classe d�riv�e de v�rifier la coh�rence entre le pointeur
   * allou� et la dimension donn�e.
   */
  inline void _setSize(Integer s)
    { m_size = s; }

 private:

  Integer m_size; //!< Nombre d'�l�ments du tableau
  T* m_ptr;  //!< Pointeur sur le tableau
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Vue constante d'un tableau de type \a T.
 *
 * \ingroup Collection

 Cette classe encapsule un tableau C constant standard (pointeur) et son nombre
 d'�l�ments. L'acc�s � ses �l�ments se fait par l'op�rateur operator[]().
 La m�thode base() permet d'obtenir le pointeur du tableau pour le passer
 aux fonctions C standard.

 L'instance conserve juste un pointeur sur le d�but du tableau C et ne fait
 aucune gestion m�moire. Le d�veloppeur doit s'assurer que le pointeur
 reste valide tant que l'instance existe. En particulier, la vue est invalid�e
 d�s que le tableau de r�f�rence est modifi� via un appel � une fonction non-const.

 Les �l�ments du tableau ne peuvent pas �tre modifi�s.

 En mode d�bug, une v�rification de d�bordement est effectu�e lors de l'acc�s
 � l'op�rateur operator[]().
 */
template<class T>
class ConstArrayView
{
 private:

 protected:

 public:
	
  //! Type des �l�ments du tableau
  typedef T value_type;
  //! Type de l'it�rateur constant sur un �l�ment du tableau
  typedef const value_type* const_iterator;
  //! Type pointeur constant d'un �l�ment du tableau
  typedef const value_type* const_pointer;
  //! Type r�f�rence constante d'un �l�ment du tableau
  typedef const value_type& const_reference;
  //! Type indexant le tableau
  typedef Integer size_type;
  //! Type d'une distance entre it�rateur �l�ments du tableau
  typedef ptrdiff_t difference_type;

 public:

  //! Construit un tableau vide.
  ConstArrayView() : m_size(0), m_ptr(0) {}
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
  : m_size(from.size()), m_ptr(from.begin())
    {
    }

  /*! \brief Op�rateur de recopie.
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
      m_ptr  = from.begin();
      return (*this);
    }
  // (HP) The destructor should be virtual because is super class-- Error if it's done
  // virtual ~ConstArrayView() { }

 public:

  /*!
   * \brief Sous-vue (constante) � partir de l'�l�ment \a begin et contenant \a size �l�ments.
   *
   * Si \a (begin+size) est sup�rieur � la taille du tableau,
   * la vue est tronqu� � cette taille, retournant �ventuellement une vue vide.
   */
  ConstArrayView<T> subView(Integer begin,Integer size) const
  {
    if (begin>=m_size)
      return ConstArrayView<T>();
    size = math::min(size,m_size-begin);
    return ConstArrayView<T>(size,m_ptr+begin);
  }

  /*!
   * \brief Sous-vue (constante) � partir de l'�l�ment \a begin et contenant \a size �l�ments.
   *
   * Si \a (begin+size) est sup�rieur � la taille du tableau,
   * la vue est tronqu� � cette taille, retournant �ventuellement une vue vide.
   */
  ConstArrayView<T> subConstView(Integer begin,Integer size) const
  {
    return subView(begin,size);
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
    ARCANE_CHECK_AT(ibegin+isize,n);
    return ConstArrayView<T>(isize,m_ptr+ibegin);
  }

  //! Addresse du index-�me �l�ment
  inline const T* ptrAt(Integer index) const
  {
    ARCANE_CHECK_AT(index,m_size);
    return m_ptr+index;
  }

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  const T& operator[](Integer i) const
  {
    ARCANE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  /*!
   * \brief i-�me �l�ment du tableau.
   *
   * En mode \a check, v�rifie les d�bordements.
   */
  inline const T& item(Integer i) const
  {
    ARCANE_CHECK_AT(i,m_size);
    return m_ptr[i];
  }

  //! Nombre d'�l�ments du tableau
  inline Integer size() const { return m_size; }
  //! Nombre d'�l�ments du tableau
  inline Integer length() const { return m_size; }
  //! Iterateur sur le premier �l�ment du tableau
  inline const_iterator begin() const { return m_ptr; }
  //! Iterateur sur le premier �l�ment apr�s la fin du tableau
  inline const_iterator end() const { return m_ptr+m_size; }
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
   * \warning Cette m�thode est dangereuse. Le pointeur retourn� peut �tre
   * invalid� d�s que le nombre d'�l�ments du tableau change.
   */
  inline const T* unguardedBasePointer() const
  { return m_ptr; }

 protected:

 private:

  Integer m_size; //!< Nombre d'�l�ments 
  const T* m_ptr; //!< Pointeur sur le d�but du tableau
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  
