/*---------------------------------------------------------------------------*/
/* Array.h                                                     (C) 2000-2018 */
/*                                                                           */
/* Tableau 1D.                                                               */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_COLLECTIONS_ARRAY_H
#define ARCCORE_COLLECTIONS_ARRAY_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArrayView.h"
#include "arccore/collections/IMemoryAllocator.h"

#include <memory>
#include <initializer_list>
#include <cstring>

// D�finir cette macro si on souhaite emp�cher la construction de Array.
// #define ARCCORE_AVOID_DEPRECATED_ARRAY_CONSTRUCTOR

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ArrayImplBase;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 *
 * \brief Interface d'un allocateur pour un vecteur.
 * \deprecated Cette classe n'est plus utilis�e.
 */
class ARCCORE_COLLECTIONS_EXPORT IArrayAllocator
{
 public:

  /*!
   * \brief D�truit l'allocateur.
   * 
   * Les objets allou�s par l'allocateur doivent tous avoir �t� d�sallou�s.
   */
  virtual ~IArrayAllocator() {}

 public:
  
  /*! \brief Alloue de la m�moire pour \a new_capacity objets.
   *
   * En cas de succ�s, retourne le pointeur sur le premier �l�ment allou�.
   * En cas d'�chec, une exception est lev�e (std::bad_alloc).
   * La valeur retourn�e n'est jamais nul.
   * \a new_capacity doit �tre strictement positif.
   */
  virtual ArrayImplBase* allocate(Integer sizeof_true_impl,Integer new_capacity,
                                  Integer sizeof_true_type,ArrayImplBase* init) = 0;

  /*! \brief Lib�re la m�moire.
   *
   * Lib�re la m�moire dont le premier �l�ment est donn�e par \a ptr.
   */
  virtual void deallocate(ArrayImplBase* ptr) =0;

  /*!
   * \brief R�alloue de la m�moire pour \a new_capacity �l�ments.
   *
   */
  virtual ArrayImplBase* reallocate(Integer sizeof_true_impl,Integer new_capacity,
                                    Integer sizeof_true_type,ArrayImplBase* ptr) =0;

  virtual Integer computeCapacity(Integer current,Integer wanted) =0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 *
 * \brief Interface d'un allocateur pour un vecteur.
 * \deprecated Cette classe n'est plus utilis�e.
 */
class ARCCORE_COLLECTIONS_EXPORT DefaultArrayAllocator
: public IArrayAllocator
{
 public:

  virtual ~DefaultArrayAllocator() {}

 public:
  
  virtual ArrayImplBase* allocate(Integer sizeof_true_impl,Integer new_capacity,
                                  Integer sizeof_true_type,ArrayImplBase* init);

  virtual void deallocate(ArrayImplBase* ptr);

  virtual ArrayImplBase* reallocate(Integer sizeof_true_impl,Integer new_capacity,
                                    Integer sizeof_true_type,ArrayImplBase* current);

  virtual Integer computeCapacity(Integer current,Integer wanted);
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 *
 * \brief Classe de base impl�mentant un vecteur.
 *
 * Cette classe sert d'impl�mentation pour tous les types tableaux
 * de Arccore, qu'ils soient 1D (Array),  2D simple (Array2)
 * ou 2D multiples (MultiArray2).
 *
 * \note Pour garantir l'alignement pour la vectorisation, cette structure
 * (et ArrayImplT) doit avoir une taille identique (sizeof) � celle sp�cifi�e dans
 * AlignedMemoryAllocator pour le simd et le cache.
 */  
class ARCCORE_COLLECTIONS_EXPORT ArrayImplBase
{
 public:
  ArrayImplBase() : nb_ref(0), capacity(0), size(0),
    dim1_size(0), dim2_size(0), mutli_dims_size(0),
    allocator(&DefaultMemoryAllocator::shared_null_instance)
 {}

  // GG: note: normalement ce destructeur est inutile et on pourrait utiliser
  // le destructeur g�n�r� par le compilateur. Cependant, si on le supprime
  // les tests SIMD en AVX en optimis� avec gcc 4.9.3 plantent.
  // Il faudrait v�rifier s'il s'agit d'un bug du compilateur ou d'un
  // probl�me dans Arccore.
  ~ArrayImplBase() {}
 public:
  static ArrayImplBase* shared_null;
 private:
  static ARCCORE_ALIGNAS(64) ArrayImplBase shared_null_instance;
 public:
  //! Nombre de r�f�rences sur cet objet.
  Int64 nb_ref;
  //! Nombre d'�l�ments allou�s
  Int64 capacity;
  //! Nombre d'�l�ments du tableau (pour les tableaux 1D)
  Int64 size;
  //! Taille de la premi�re dimension (pour les tableaux 2D)
  Int64 dim1_size;
  //! Taille de la deuxi�me dimension (pour les tableaux 2D)
  Int64 dim2_size;
  //! Tableau des dimensions pour les tableaux multiples
  Int64* mutli_dims_size;
  //! Allocateur m�moire
  IMemoryAllocator* allocator;

  // TODO: gerer le padding pour le 64 bit.
#ifdef ARCCORE_64BIT
#error "Padding for ArrayImplBase not implemented for 64 bit"
#else
  //Int32 padding[6];
  Int64 padding1;
#endif

  static ArrayImplBase* allocate(Integer sizeof_true_impl,Integer nb,
                                 Integer sizeof_true_type,ArrayImplBase* init);
  static ArrayImplBase* allocate(Integer sizeof_true_impl,Integer nb,
                                 Integer sizeof_true_type,ArrayImplBase* init,
                                 IMemoryAllocator* allocator);
  static ArrayImplBase* reallocate(Integer sizeof_true_impl,Integer nb,
                                   Integer sizeof_true_type,ArrayImplBase* current);
  static void deallocate(ArrayImplBase* current);
  static void overlapError(const void* begin1,Integer size1,
                           const void* begin2,Integer size2);

  static void checkSharedNull()
  {
    ArrayImplBase* s = shared_null;
    if (s->capacity!=0 || s->size!=0 || s->dim1_size!=0 || s->dim2_size!=0
        || s->mutli_dims_size || s->allocator)
      throwBadSharedNull();
  }
  static void throwBadSharedNull ARCCORE_NORETURN ();
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 *
 * \brief Impl�mentation d'un vecteur d'un type T.
 */  
template <typename T>
class ArrayImplT
: public ArrayImplBase
{
 public:
  ArrayImplT() {}
  T ptr[1];
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 *
 * \brief Caract�ristiques pour un tableau
 */
template<typename T>
class ArrayTraits
{
 public:
  typedef const T& ConstReferenceType;
  typedef FalseType IsPODType;
};

#define ARCCORE_DEFINE_ARRAY_PODTYPE(datatype)\
template<>\
class ArrayTraits<datatype>\
{\
 public:\
  typedef datatype ConstReferenceType;\
  typedef TrueType IsPODType;\
}

template<typename T>
class ArrayTraits<T*>
{
 public:
  typedef T* Ptr;
  typedef const Ptr& ConstReferenceType;
  typedef FalseType IsPODType;
};

template<typename T>
class ArrayTraits<const T*>
{
 public:
  typedef T* Ptr;
  typedef const T* ConstReferenceType;
  typedef FalseType IsPODType;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCCORE_DEFINE_ARRAY_PODTYPE(char);
ARCCORE_DEFINE_ARRAY_PODTYPE(signed char);
ARCCORE_DEFINE_ARRAY_PODTYPE(unsigned char);
ARCCORE_DEFINE_ARRAY_PODTYPE(short);
ARCCORE_DEFINE_ARRAY_PODTYPE(int);
ARCCORE_DEFINE_ARRAY_PODTYPE(long);
ARCCORE_DEFINE_ARRAY_PODTYPE(unsigned short);
ARCCORE_DEFINE_ARRAY_PODTYPE(unsigned int);
ARCCORE_DEFINE_ARRAY_PODTYPE(unsigned long);
ARCCORE_DEFINE_ARRAY_PODTYPE(float);
ARCCORE_DEFINE_ARRAY_PODTYPE(double);
ARCCORE_DEFINE_ARRAY_PODTYPE(long double);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Collection
 * \brief Classe abstraite de base d'un vecteur.
 *
 * Cette classe ne peut pas �tre utilis�e directement. Pour utiliser un
 * vecteur, choisissez la classe SharedArray ou UniqueArray.
 */
template<typename T>
class AbstractArray
{
 public:

  typedef typename ArrayTraits<T>::ConstReferenceType ConstReferenceType;
  typedef typename ArrayTraits<T>::IsPODType IsPODType;
  typedef AbstractArray<T> ThatClassType;
  typedef ArrayImplT<T> TrueImpl;

 public:
	
  //! Type des �l�ments du tableau
  typedef T value_type;
  //! Type pointeur d'un �l�ment du tableau
  typedef value_type* pointer;
  //! Type pointeur constant d'un �l�ment du tableau
  typedef const value_type* const_pointer;
  //! Type de l'it�rateur sur un �l�ment du tableau
  typedef pointer iterator;
  //! Type de l'it�rateur constant sur un �l�ment du tableau
  typedef const_pointer const_iterator;
  //! Type r�f�rence d'un �l�ment du tableau
  typedef value_type& reference;
  //! Type r�f�rence constante d'un �l�ment du tableau
  typedef ConstReferenceType const_reference;
  //! Type indexant le tableau
  typedef Integer size_type;
  //! Type d'une distance entre it�rateur �l�ments du tableau
  typedef ptrdiff_t difference_type;

 private:

  static TrueImpl* _sharedNull()
  {
    return static_cast<TrueImpl*>(ArrayImplBase::shared_null);
  }
 protected:

  //! Construit un vecteur vide avec l'allocateur par d�faut
  AbstractArray()
  : m_p(_sharedNull()), m_baseptr(m_p->ptr)
  {
  }
  /*!
   * \brief Construit un vecteur vide avec un allocateur sp�cifique \a a.
   * Si \a n'est pas nul, la m�moire est allou�e pour
   * contenir \a acapacity �l�ments (mais le tableau reste vide).
   */
  AbstractArray(IMemoryAllocator* a,Integer acapacity)
  : m_p(_sharedNull()), m_baseptr(m_p->ptr)
  {
    // Si un allocateur sp�cifique est utilis� et qu'il n'est pas
    // celui par d�faut, il faut toujours allouer un objet pour
    // pouvoir conserver l'instance de l'allocateur. Par d�faut
    // on utilise une taille de 1 �l�ment.
    if (a && a!=m_p->allocator){
      Integer c = (acapacity>1) ? acapacity : 1;
      _directFirstAllocateWithAllocator(c,a);
    }
  }
  //! Constructeur par d�placement. Ne doit �tre utilis� que par UniqueArray
  AbstractArray(ThatClassType&& rhs) ARCCORE_NOEXCEPT
  : m_p(rhs.m_p), m_baseptr(m_p->ptr)
  {
    rhs._reset();
  }
  AbstractArray(Integer asize,const T* values)
  : m_p(_sharedNull()), m_baseptr(m_p->ptr)
  {
    if (asize!=0){
      _internalAllocate(asize);
      _createRange(0,asize,values);
      m_p->size = asize;
    }
  }
  AbstractArray(const ConstArrayView<T>& view)
  : m_p(_sharedNull()), m_baseptr(m_p->ptr)
  {
    Integer asize = view.size();
    if (asize!=0){
      _internalAllocate(asize);
      _createRange(0,asize,view.unguardedBasePointer());
      m_p->size = asize;
    }
  }

  virtual ~AbstractArray()
  {
    //_removeReference();
    --m_p->nb_ref;
    _checkFreeMemory();
  }
 public:
  //! Lib�re la m�moire utilis�e par le tableau.
  void dispose()
  {
    _destroy();
    IMemoryAllocator* a = m_p->allocator;
    _internalDeallocate();
    _setMP(_sharedNull());
    // Si on a un allocateur sp�cifique, il faut allouer un
    // bloc pour conserver cette information.
    if (a != m_p->allocator)
      _directFirstAllocateWithAllocator(1,a);
    _updateReferences();
  }
  IMemoryAllocator* allocator() const
  {
    return m_p->allocator;
  }
 public:
  operator ConstArrayView<T>() const
  {
    return ConstArrayView<T>(size(),m_p->ptr);
  }
 public:
  //! Nombre d'�l�ments du vecteur
  Integer size() const { return m_p->size; }
  //! Nombre d'�l�ments du vecteur
  Integer length() const { return m_p->size; }
  //! Capacit� (nombre d'�l�ments allou�s) du vecteur
  Integer capacity() const { return m_p->capacity; }
  //! Capacit� (nombre d'�l�ments allou�s) du vecteur
  bool empty() const { return m_p->size==0; }
  //! Vrai si le tableau contient l'�l�ment de valeur \a v
  bool contains(ConstReferenceType v) const
  {
    const T* ptr = m_p->ptr;
    for( Integer i=0, n=m_p->size; i<n; ++i ){
      if (ptr[i]==v)
        return true;
    }
    return false;
  }
 public:
  //! El�ment d'indice \a i
  ConstReferenceType operator[](Integer i) const
  {
    ARCCORE_CHECK_AT(i,m_p->size);
    return m_p->ptr[i];
  }
 public:
#if 0
  const_iterator begin() const
  { return m_p->ptr; }
  const_iterator end() const
  { return m_p->ptr+m_p->m_size; }
#endif
 protected:
  TrueImpl* m_p;
 private:
  T* m_baseptr;
 protected:
  //! R�serve le m�moire pour \a new_capacity �l�ments
  void _reserve(Integer new_capacity)
  {
    if (new_capacity<=m_p->capacity)
      return;
    _internalRealloc(new_capacity,false);
  }
  /*!
   * \brief R�alloue le tableau pour une nouvelle capacit� �gale � \a new_capacity.
   *
   * Si la nouvelle capacit� est inf�rieure � l'ancienne, rien ne se passe.
   */
  virtual void _internalRealloc(Integer new_capacity,bool compute_capacity)
  {
    if (m_p==TrueImpl::shared_null){
      if (new_capacity!=0)
        _internalAllocate(new_capacity);
      return;
    }

    Integer acapacity = new_capacity;
    if (compute_capacity){
      acapacity = m_p->capacity;
      //std::cout << " REALLOC: want=" << wanted_size << " current_capacity=" << capacity << '\n';
      while (new_capacity>acapacity)
        acapacity = (acapacity==0) ? 4 : (acapacity + 1 + acapacity / 2);
      //std::cout << " REALLOC: want=" << wanted_size << " new_capacity=" << capacity << '\n';
    }
    // Si la nouvelle capacit� est inf�rieure � la courante,ne fait rien.
    if (acapacity<m_p->capacity)
      return;
    _internalReallocate(acapacity,IsPODType());
  }

  //! R�allocation pour un type POD
  virtual void _internalReallocate(Integer new_capacity,TrueType)
  {
    TrueImpl* old_p = m_p;
    Integer old_capacity = m_p->capacity;
    _directReAllocate(new_capacity);
    bool update = (new_capacity < old_capacity) || (m_p != old_p);
    if (update){
      _updateReferences();
    }
  }

  //! R�allocation pour un type complexe (non POD)
  virtual void _internalReallocate(Integer new_capacity,FalseType)
  {
    TrueImpl* old_p = m_p;
    Integer old_size = m_p->size;
    _directAllocate(new_capacity);
    if (m_p!=old_p){
      for( Integer i=0; i<old_size; ++i ){
        new (m_p->ptr+i) T(old_p->ptr[i]);
        old_p->ptr[i].~T();
      }
      m_p->nb_ref = old_p->nb_ref;
      ArrayImplBase::deallocate(old_p);
      _updateReferences();
    }
  }
  // Lib�re la m�moire
  virtual void _internalDeallocate()
  {
    if (m_p!=TrueImpl::shared_null)
      ArrayImplBase::deallocate(m_p);
  }
  virtual void _internalAllocate(Integer new_capacity)
  {
    _directAllocate(new_capacity);
    m_p->nb_ref = _getNbRef();
    _updateReferences();
  }
 private:
  virtual void _directFirstAllocateWithAllocator(Integer new_capacity,IMemoryAllocator* a)
  {
    //TODO: v�rifier m_p vaut shared_null
    _setMP(static_cast<TrueImpl*>(ArrayImplBase::allocate(sizeof(TrueImpl),new_capacity,sizeof(T),m_p,a)));
    m_p->allocator = a;
    m_p->nb_ref = _getNbRef();
    m_p->size = 0;
    _updateReferences();
  }
  virtual void _directAllocate(Integer new_capacity)
  {
    _setMP(static_cast<TrueImpl*>(ArrayImplBase::allocate(sizeof(TrueImpl),new_capacity,sizeof(T),m_p)));
  }
  virtual void _directReAllocate(Integer new_capacity)
  {
    _setMP(static_cast<TrueImpl*>(ArrayImplBase::reallocate(sizeof(TrueImpl),new_capacity,sizeof(T),m_p)));
  }
 public:
  void printInfos(std::ostream& o)
  {
    o << " Infos: size=" << m_p->size << " capacity=" << m_p->capacity << '\n';
  }
 protected:
  //! Mise � jour des r�f�rences
  virtual void _updateReferences()
  {
  }
  //! Mise � jour des r�f�rences
  virtual Integer _getNbRef()
  {
    return 1;
  }
  //! Ajoute \a n �l�ment de valeur \a val � la fin du tableau
  void _addRange(ConstReferenceType val,Integer n)
  {
    Integer s = m_p->size;
    if ((s+n) > m_p->capacity)
      _internalRealloc(s+n,true);
    for( Integer i=0; i<n; ++i )
      new (m_p->ptr + s + i) T(val);
    m_p->size += n;
  }

  //! Ajoute \a n �l�ment de valeur \a val � la fin du tableau
  void _addRange(ConstArrayView<T> val)
  {
    Integer n = val.size();
    const T* ptr = val.data();
    Integer s = m_p->size;
    if ((s+n) > m_p->capacity)
      _internalRealloc(s+n,true);
    _createRange(s,s+n,ptr);
    m_p->size += n;
  }

  //! D�truit l'instance si plus personne ne la r�f�rence
  void _checkFreeMemory()
  {
    if (m_p->nb_ref==0){
      _destroy();
      _internalDeallocate();
    }
  }
  void _destroy()
  {
    _destroyRange(0,m_p->size,IsPODType());
  }
  void _destroyRange(Integer,Integer,TrueType)
  {
    // Rien � faire pour un type POD.
  }
  void _destroyRange(Integer abegin,Integer aend,FalseType)
  {
    for( Integer i=abegin; i<aend; ++i )
      m_p->ptr[i].~T();
  }
  void _createRangeDefault(Integer,Integer,TrueType)
  {
  }
  void _createRangeDefault(Integer abegin,Integer aend,FalseType)
  {
    for( Integer i=abegin; i<aend; ++i )
      new (m_p->ptr+i) T();
  }
  void _createRange(Integer abegin,Integer aend,ConstReferenceType value,TrueType)
  {
    for( Integer i=abegin; i<aend; ++i )
      m_p->ptr[i] = value;
  }
  void _createRange(Integer abegin,Integer aend,ConstReferenceType value,FalseType)
  {
    for( Integer i=abegin; i<aend; ++i )
      new (m_p->ptr+i) T(value);
  }
  void _createRange(Integer abegin,Integer aend,const T* values)
  {
    for( Integer i=abegin; i<aend; ++i ){
      new (m_p->ptr+i) T(*values);
      ++values;
    }
  }
  void _fill(ConstReferenceType value)
  {
    for( Integer i=0, n=size(); i<n; ++i )
      m_p->ptr[i] = value;
  }
  void _clone(const ThatClassType& orig_array)
  {
    Integer that_size = orig_array.size();
    _internalAllocate(that_size);
    m_p->size = that_size;
    m_p->dim1_size = orig_array.m_p->dim1_size;
    m_p->dim2_size = orig_array.m_p->dim2_size;
    _createRange(0,that_size,orig_array.m_p->ptr);
  }
  void _resize(Integer s)
  {
    if (s>m_p->size) {
      this->_internalRealloc(s,false);
      this->_createRangeDefault(m_p->size,s,IsPODType());
    }
    else{
      this->_destroyRange(s,m_p->size,IsPODType());
    }
    m_p->size = s;
  }
  void _clear()
  {
    this->_destroyRange(0,m_p->size,IsPODType());
    m_p->size = 0;
  }
  void _resize(Integer s,ConstReferenceType value)
  {
    if (s>m_p->size){
      this->_internalRealloc(s,false);
      this->_createRange(m_p->size,s,value,IsPODType());
    }
    else{
      this->_destroyRange(s,m_p->size,IsPODType());
    }
    m_p->size = s;
  }
  void _copy(const T* rhs_begin,TrueType)
  {
    std::memcpy(m_p->ptr,rhs_begin,((size_t)m_p->size)*sizeof(T));
  }
  void _copy(const T* rhs_begin,FalseType)
  {
    for( Integer i=0, is=m_p->size; i<is; ++i )
      m_p->ptr[i] = rhs_begin[i];
  }
  void _copy(const T* rhs_begin)
  {
    _copy(rhs_begin,IsPODType());
  }
  void _copyView(ConstArrayView<T> rhs)
  {
    const T* rhs_begin = rhs.data();
    Integer rhs_size = rhs.size();
    T* abegin = m_p->ptr;
    // V�rifie que \a rhs n'est pas un �l�ment � l'int�rieur de ce tableau
    if (abegin>=rhs_begin && abegin<(rhs_begin+rhs_size))
      ArrayImplBase::overlapError(abegin,m_p->size,rhs_begin,rhs_size);
    _resize(rhs_size);
    _copy(rhs_begin);
  }

  /*!
   * \brief Impl�mente l'op�rateur d'assignement par d�placement.
   *
   * Cet appel n'est valide que pour les tableaux de type UniqueArray
   * qui n'ont qu'une seule r�f�rence. Les infos de \a rhs sont directement
   * copi�s cette l'instance. En retour, \a rhs contient le tableau vide.
   */
  void _move(ThatClassType& rhs) ARCCORE_NOEXCEPT
  {
    if (&rhs==this)
      return;

    // Comme il n'y a qu'une seule r�f�rence sur le tableau actuel, on peut
    // directement lib�rer la m�moire.
    _destroy();
    _internalDeallocate();

    // Recopie bit � bit.
    _setMP(rhs.m_p);

    // Indique que \a rhs est vide.
    rhs._reset();
  }

  /*!
   * \brief �change les valeurs de l'instance avec celles de \a rhs.
   *
   * Cet appel n'est valide que pour les tableaux de type UniqueArray
   * et l'�change se fait uniquement par l'�change des pointeurs. L'op�ration
   * est donc de complexit� constante.
   */
  void _swap(ThatClassType& rhs) ARCCORE_NOEXCEPT
  {
    std::swap(m_p,rhs.m_p);
    std::swap(m_baseptr,rhs.m_baseptr);
  }

 private:
  /*!
   * \brief R�initialise le tableau � un tableau vide.
   * \warning Cette m�thode n'est valide que pour les UniqueArray et pas
   * les SharedArray.
   */
  void _reset()
  {
    _setMP(_sharedNull());
  }

 protected:

  void _setMP(TrueImpl* new_mp)
  {
    m_p = new_mp;
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Collection
 *
 * \brief Classe de base des vecteurs 1D de donn�es.
 *
 * Cette classe manipule un vecteur (tableau) 1D de donn�es.
 *
 * Les instances de cette classe ne sont pas copiables ni affectable. Pour cr�er un
 * tableau copiable, il faut utiliser SharedArray (pour une s�mantique par
 * r�f�rence) ou UniqueArray (pour une s�mantique par valeur comme la STL).
 */
template<typename T>
class Array
: public AbstractArray<T>
{
 protected:

  using AbstractArray<T>::m_p;
  enum CloneBehaviour
    {
      CB_Clone,
      CB_Shared
    };
  enum BuildDeprecated
    {
      BD_NoWarning
    };
 public:

  typedef AbstractArray<T> BaseClassType;
  typedef typename BaseClassType::ConstReferenceType ConstReferenceType;

 public:
	
  //! Type des �l�ments du tableau
  typedef typename BaseClassType::value_type value_type;
  //! Type de l'it�rateur sur un �l�ment du tableau
  typedef typename BaseClassType::iterator iterator;
  //! Type de l'it�rateur constant sur un �l�ment du tableau
  typedef typename BaseClassType::const_iterator const_iterator;
  //! Type pointeur d'un �l�ment du tableau
  typedef typename BaseClassType::pointer pointer;
  //! Type pointeur constant d'un �l�ment du tableau
  typedef typename BaseClassType::const_pointer const_pointer;
  //! Type r�f�rence d'un �l�ment du tableau
  typedef typename BaseClassType::reference reference;
  //! Type r�f�rence constante d'un �l�ment du tableau
  typedef typename BaseClassType::const_reference const_reference;
  //! Type indexant le tableau
  typedef typename BaseClassType::size_type size_type;
  //! Type d'une distance entre it�rateur �l�ments du tableau
  typedef typename BaseClassType::difference_type difference_type;
#ifndef ARCCORE_AVOID_DEPRECATED_ARRAY_CONSTRUCTOR
 public:
#else
 protected:
#endif
  ARCCORE_DEPRECATED_2018 Array() : AbstractArray<T>() {}
  //! Cr�� un tableau vide avec un allocateur sp�cifique.
  ARCCORE_DEPRECATED_2018 Array(Integer asize,ConstReferenceType value) : AbstractArray<T>()
  {
    this->_resize(asize,value);
  }
  //! Constructeur avec liste d'initialisation.
  ARCCORE_DEPRECATED_2018 Array(std::initializer_list<T> alist) : AbstractArray<T>()
  {
    Integer nsize = arccoreCheckArraySize(alist.size());
    this->_reserve(nsize);
    for( auto x : alist )
      this->add(x);
  }
  ARCCORE_DEPRECATED_2018 explicit Array(Integer asize) : AbstractArray<T>()
  {
    this->_resize(asize);
  }
  ARCCORE_DEPRECATED_2018 Array(const ConstArrayView<T>& aview) : AbstractArray<T>(aview)
  {
  }
  /*!
   * \brief Cr�� un tableau de \a asize �l�ments avec un allocateur sp�cifique.
   *
   * Si ArrayTraits<T>::IsPODType vaut TrueType, les �l�ments ne sont pas
   * initialis�s. Sinon, c'est le constructeur par d�faut de T qui est utilis�.
   */
  ARCCORE_DEPRECATED_2018 Array(IMemoryAllocator* allocator,Integer asize)
  : AbstractArray<T>(allocator,asize)
  {
    this->_resize(asize);
  }
 protected:
  Array(BuildDeprecated) : AbstractArray<T>() {}
  Array(Integer asize,ConstReferenceType value,BuildDeprecated) : AbstractArray<T>()
  {
    this->_resize(asize,value);
  }
  //! Constructeur avec liste d'initialisation.
  Array(std::initializer_list<T> alist,BuildDeprecated) : AbstractArray<T>()
  {
    Integer nsize = arccoreCheckArraySize(alist.size());
    this->_reserve(nsize);
    for( auto x : alist )
      this->add(x);
  }
  Array(Integer asize,BuildDeprecated) : AbstractArray<T>()
  {
    this->_resize(asize);
  }
  Array(const ConstArrayView<T>& aview,BuildDeprecated) : AbstractArray<T>(aview)
  {
  }
  /*!
   * \brief Cr�� un tableau de \a asize �l�ments avec un allocateur sp�cifique.
   *
   * Si ArrayTraits<T>::IsPODType vaut TrueType, les �l�ments ne sont pas
   * initialis�s. Sinon, c'est le constructeur par d�faut de T qui est utilis�.
   */
  Array(IMemoryAllocator* allocator,Integer asize,BuildDeprecated)
  : AbstractArray<T>(allocator,asize)
  {
    this->_resize(asize);
  }
 protected:
  //! Constructeur par d�placement (uniquement pour UniqueArray)
  Array(Array<T>&& rhs) ARCCORE_NOEXCEPT : AbstractArray<T>(std::move(rhs)) {}
 private:
  Array(const Array<T>& rhs);
  void operator=(const Array<T>& rhs);

 public:
  ~Array()
  {
  }
 public:
  operator ConstArrayView<T>() const
  {
    return ConstArrayView<T>(this->size(),m_p->ptr);
  }
  operator ArrayView<T>()
  {
    return ArrayView<T>(this->size(),m_p->ptr);
  }
  //! Vue constante sur ce tableau
  ConstArrayView<T> constView() const
  {
    return ConstArrayView<T>(this->size(),m_p->ptr);
  }
  /*!
   * \brief Sous-vue � partir de l'�l�ment \a abegin et contenant \a asize �l�ments.
   *
   * Si \a (\a abegin + \a asize) est sup�rieur � la taille du tableau,
   * la vue est tronqu� � cette taille, retournant �ventuellement une vue vide.
   */
  ConstArrayView<T> subConstView(Integer abegin,Integer asize) const
  {
    return constView().subView(abegin,asize);
  }
  //! Vue mutable sur ce tableau
  ArrayView<T> view() const
  {
    return ArrayView<T>(this->size(),m_p->ptr);
  }
  /*!
   * \brief Sous-vue � partir de l'�l�ment \a abegin et contenant \a asize �l�ments.
   *
   * Si \a (\a abegin + \a asize) est sup�rieur � la taille du tableau,
   * la vue est tronqu� � cette taille, retournant �ventuellement une vue vide.
   */
  ArrayView<T> subView(Integer abegin,Integer asize)
  {
    return view().subView(abegin,asize);
  }

  /*!
   * \brief Extrait un sous-tableau � � partir d'une liste d'index.
   *
   * Le r�sultat est stock� dans \a result dont la taille doit �tre au moins
   * �gale � celle de \a indexes.
   */
  void sample(ConstArrayView<Integer> indexes,ArrayView<T> result) const
  {
    const Integer result_size = indexes.size();
#ifdef ARCCORE_CHECK
    const Integer my_size = m_p->size;
#endif
    for( Integer i=0; i<result_size; ++i) {
      Int32 index = indexes[i];
      ARCCORE_CHECK_AT(index,my_size);
      result[i] = m_p->ptr[index];
    }
  }

 public:
  //! Ajoute l'�l�ment \a val � la fin du tableau
  void add(ConstReferenceType val)
  {
    if (m_p->size >= m_p->capacity)
      this->_internalRealloc(m_p->size+1,true);
    new (m_p->ptr + m_p->size) T(val);
    ++m_p->size;
  }
  //! Ajoute \a n �l�ment de valeur \a val � la fin du tableau
  void addRange(ConstReferenceType val,Integer n)
  {
    this->_addRange(val,n);
  }
  //! Ajoute \a n �l�ment de valeur \a val � la fin du tableau
  void addRange(ConstArrayView<T> val)
  {
    this->_addRange(val);
  }
  /*!
   * \brief Change le nombre d'�l�ment du tableau � \a s.
   * Si le nouveau tableau est plus grand que l'ancien, les nouveaux
   * �l�ments ne sont pas initialis�s s'il s'agit d'un type POD.
   */
  void resize(Integer s) { this->_resize(s); }
  /*!
   * \brief Change le nombre d'�l�ment du tableau � \a s.
   * Si le nouveau tableau est plus grand que l'ancien, les nouveaux
   * �l�ments sont initialis� avec la valeur \a fill_value.
   */
  void resize(Integer s,ConstReferenceType fill_value)
  {
    this->_resize(s,fill_value);
  }
  //! R�serve le m�moire pour \a new_capacity �l�ments
  void reserve(Integer new_capacity)
  {
    this->_reserve(new_capacity);
  }
  /*!
   * \brief Supprime l'entit� ayant l'indice \a index.
   *
   * Tous les �l�ments de ce tableau apr�s celui supprim� sont
   * d�cal�s.
   */
  void remove(Integer index)
  {
    Integer s = m_p->size;
    ARCCORE_CHECK_AT(index,s);
    for( Integer i=index; i<(s-1); ++i )
      m_p->ptr[i] = m_p->ptr[i+1];
    --m_p->size;
    m_p->ptr[m_p->size].~T();
  }
  /*!
   * \brief Supprime la derni�re entit� du tableau.
   */
  void popBack()
  {
    ARCCORE_CHECK_AT(0,m_p->size);
    --m_p->size;
    m_p->ptr[m_p->size].~T();
  }
  //! El�ment d'indice \a i. V�rifie toujours les d�bordements
  ConstReferenceType at(Integer i) const
  {
    arccoreCheckAt(i,m_p->size);
    return m_p->ptr[i];
  }
  //! Positionne l'�l�ment d'indice \a i. V�rifie toujours les d�bordements
  void setAt(Integer i,ConstReferenceType value)
  {
    arccoreCheckAt(i,m_p->size);
    m_p->ptr[i] = value;
  }
  //! El�ment d'indice \a i
  ConstReferenceType item(Integer i) const { return m_p->ptr[i]; }
  //! El�ment d'indice \a i
  void setItem(Integer i,ConstReferenceType v) { m_p->ptr[i] = v; }
  //! El�ment d'indice \a i
  ConstReferenceType operator[](Integer i) const
  {
    ARCCORE_CHECK_AT(i,m_p->size);
    return m_p->ptr[i];
  }
  //! El�ment d'indice \a i
  T& operator[](Integer i)
  {
    ARCCORE_CHECK_AT(i,m_p->size);
    return m_p->ptr[i];
  }
  //! Dernier �l�ment du tableau
  /*! Le tableau ne doit pas �tre vide */
  T& back()
  {
    ARCCORE_CHECK_AT(m_p->size-1,m_p->size);
    return m_p->ptr[m_p->size-1];
  }
  //! Dernier �l�ment du tableau (const)
  /*! Le tableau ne doit pas �tre vide */
  ConstReferenceType back() const
  {
    ARCCORE_CHECK_AT(m_p->size-1,m_p->size);
    return m_p->ptr[m_p->size-1];
  }

  //! Premier �l�ment du tableau
  /*! Le tableau ne doit pas �tre vide */
  T& front()
  {
    ARCCORE_CHECK_AT(0,m_p->size);
    return m_p->ptr[0];
  }

  //! Premier �l�ment du tableau (const)
  /*! Le tableau ne doit pas �tre vide */
  ConstReferenceType front() const
  {
    ARCCORE_CHECK_AT(0,m_p->size);
    return m_p->ptr[0];
  }

  //! Supprime les �l�ments du tableau
  void clear()
  {
    this->_clear();
  }
  
  //! Remplit le tableau avec la valeur \a value
  void fill(ConstReferenceType value)
  {
    this->_fill(value);
  }
  
  /*!
   * \brief Copie les valeurs de \a rhs dans l'instance.
   *
   * L'instance est redimensionn�e pour que this->size()==rhs.size().
   */  
  void copy(ConstArrayView<T> rhs)
  {
    this->_copyView(rhs);
  }

  //! Clone le tableau
  Array<T> clone() const
  {
    return Array<T>(this->constView());
  }

  //! \internal Acc�s � la racine du tableau hors toute protection
  const T* unguardedBasePointer() const
    { return m_p->ptr; }
  //! \internal Acc�s � la racine du tableau hors toute protection
  T* unguardedBasePointer()
    { return m_p->ptr; }

  //! Acc�s � la racine du tableau hors toute protection
  const T* data() const
  { return m_p->ptr; }
  //! \internal Acc�s � la racine du tableau hors toute protection
  T* data()
  { return m_p->ptr; }

 public:

  /*!
   * \brief It�rateur sur le premier �l�ment du tableau.
   * \deprecated Utiliser range().begin(), std::begin() ou data() � la place.
   */
  ARCCORE_DEPRECATED_2018 iterator begin()
  { return iterator(m_p->ptr); }
  /*!
   * \brief It�rateur constant sur le premier �l�ment du tableau.
   * \deprecated Utiliser range().begin(), std::begin() ou data() � la place.
   */
  ARCCORE_DEPRECATED_2018 const_iterator begin() const
  { return const_iterator(m_p->ptr); }
  /*!
   * \brief It�rateur sur le premier �l�ment apr�s la fin du tableau.
   * \deprecated Utiliser range().end() � la place.
   */
  ARCCORE_DEPRECATED_2018 iterator end()
  { return iterator(m_p->ptr+m_p->size); }
  /*!
   * \brief It�rateur constant sur le premier �l�ment apr�s la fin du tableau.
   * \deprecated Utiliser range().end() � la place.
   */
  ARCCORE_DEPRECATED_2018 const_iterator end() const
  { return const_iterator(m_p->ptr+m_p->size); }

 public:

  //! Intervalle d'it�ration du premier au derni�r �l�ment.
  ArrayRange<pointer> range()
  {
    return ArrayRange<pointer>(m_p->ptr,m_p->ptr+m_p->size);
  }
  //! Intervalle d'it�ration du premier au derni�r �l�ment.
  ArrayRange<const_pointer> range() const
  {
    return ArrayRange<const_pointer>(m_p->ptr,m_p->ptr+m_p->size);
  }
 public:

  //@{ M�thodes pour compatibilit� avec la STL.
  //! Ajoute l'�l�ment \a val � la fin du tableau
  void push_back(ConstReferenceType val)
  {
    this->add(val);
  }
  //@}
private:

  //! Method called from totalview debugger
  static int TV_ttf_display_type(const Arccore::Array<T> * obj);
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Collection
 *
 * \brief Vecteur 1D de donn�es avec s�mantique par r�f�rence.
 *
 * Pour avoir un vecteur qui utilise une s�mantique par valeur (� la std::vector),
 * il faut utiliser la classe UniqueArray.
 *
 * La s�mantique par r�f�rence fonctionne comme suit:
 *
 * \code
 * SharedArray<int> a1(5);
 * SharedArray<int> a2;
 * a2 = a1; // a2 et a1 font r�f�rence � la m�me zone m�moire.
 * a1[3] = 1;
 * a2[3] = 2;
 * std::cout << a1[3]; // affiche '2'
 * \endcode
 *
 * Dans l'exemple pr�c�dent, \a a1 et \a a2 font r�f�rence � la m�me zone
 * m�moire et donc \a a2[3] aura la m�me valeur que \a a1[3] (soit la valeur \a 2),
 *
 * Un tableau partag�e est d�sallou� lorsqu'il n'existe plus
 * de r�f�rence sur ce tableau.
 *
 * \warning les op�rations de r�f�rencement/d�r�f�rencement (les op�rateurs
 * d'affection, de recopie et les destructeurs) ne sont pas thread-safe. Par
 * cons�quent ce type de tableau doit �tre utilis� avec pr�caution dans
 * le cas d'un environnement multi-thread.
 *
 */
template<typename T>
class SharedArray
: public Array<T>
{
 protected:

  using AbstractArray<T>::m_p;
  using Array<T>::BD_NoWarning;

 public:

  typedef SharedArray<T> ThatClassType;
  typedef AbstractArray<T> BaseClassType;
  typedef typename BaseClassType::ConstReferenceType ConstReferenceType;

 public:

 public:
  //! Cr�� un tableau vide
  SharedArray() : Array<T>(BD_NoWarning), m_next(nullptr), m_prev(nullptr) {}
  //! Cr�� un tableau de \a size �l�ments contenant la valeur \a value.
  SharedArray(Integer asize,ConstReferenceType value)
  : Array<T>(BD_NoWarning), m_next(nullptr), m_prev(nullptr)
  {
    this->_resize(asize,value);
  }
  //! Cr�� un tableau de \a size �l�ments contenant la valeur par d�faut du type T()
  explicit SharedArray(Integer asize)
  : Array<T>(BD_NoWarning), m_next(nullptr), m_prev(nullptr)
  {
    this->_resize(asize);
  }
  //! Cr�� un tableau en recopiant les valeurs de la value \a view.
  SharedArray(const ConstArrayView<T>& aview)
  : Array<T>(aview,BD_NoWarning), m_next(nullptr), m_prev(nullptr)
  {
  }
  SharedArray(std::initializer_list<T> alist)
  : Array<T>(alist,BD_NoWarning), m_next(nullptr), m_prev(nullptr)
  {
  }
  //! Cr�� un tableau faisant r�f�rence � \a rhs.
  SharedArray(const SharedArray<T>& rhs)
  : Array<T>(BD_NoWarning), m_next(nullptr), m_prev(nullptr)
  {
    _initReference(rhs);
  }
  //! Cr�� un tableau en recopiant les valeurs \a rhs.
  inline SharedArray(const UniqueArray<T>& rhs);
  //! Change la r�f�rence de cette instance pour qu'elle soit celle de \a rhs.
  void operator=(const SharedArray<T>& rhs)
  {
    this->_operatorEqual(rhs);
  }
  //! Copie les valeurs de \a rhs dans cette instance.
  inline void operator=(const UniqueArray<T>& rhs);
  //! Copie les valeurs de la vue \a rhs dans cette instance.
  void operator=(const ConstArrayView<T>& rhs)
  {
    this->copy(rhs);
  }
  //! D�truit le tableau
  ~SharedArray() override
  {
    _removeReference();
  }
 public:

  //! Clone le tableau
  SharedArray<T> clone() const
  {
    return SharedArray<T>(this->constView());
  }
 protected:
  void _initReference(const ThatClassType& rhs)
  {
    this->_setMP(rhs.m_p);
    _addReference(&rhs);
    ++m_p->nb_ref;
  }
  //! Mise � jour des r�f�rences
  void _updateReferences() override final
  {
    for( ThatClassType* i = m_prev; i; i = i->m_prev )
      i->_setMP(m_p);
    for( ThatClassType* i = m_next; i; i = i->m_next )
      i->_setMP(m_p);
  }
  //! Mise � jour des r�f�rences
  Integer _getNbRef() override final
  {
    Integer nb_ref = 1;
    for( ThatClassType* i = m_prev; i; i = i->m_prev )
      ++nb_ref;
    for( ThatClassType* i = m_next; i; i = i->m_next )
      ++nb_ref;
    return nb_ref;
  }
  /*!
   * \brief Ins�re cette instance dans la liste cha�n�e.
   * L'instance est ins�r�e � la position de \a new_ref.
   * \pre m_prev==0
   * \pre m_next==0;
   */
  void _addReference(const ThatClassType* new_ref)
  {
    ThatClassType* nf = const_cast<ThatClassType*>(new_ref);
    ThatClassType* prev = nf->m_prev;
    nf->m_prev = this;
    m_prev = prev;
    m_next = nf;
    if (prev)
      prev->m_next = this;
  }
  //! Supprime cette instance de la liste cha�n�e des r�f�rences
  void _removeReference()
  {
    if (m_prev)
      m_prev->m_next = m_next;
    if (m_next)
      m_next->m_prev = m_prev;
  }
  //! D�truit l'instance si plus personne ne la r�f�rence
  void _checkFreeMemory()
  {
    if (m_p->nb_ref==0){
      this->_destroy();
      this->_internalDeallocate();
    }
  }
  void _operatorEqual(const ThatClassType& rhs)
  {
    if (&rhs!=this){
      _removeReference();
      _addReference(&rhs);
      ++rhs.m_p->nb_ref;
      --m_p->nb_ref;
      _checkFreeMemory();
      this->_setMP(rhs.m_p);
    }
  }
 private:
  ThatClassType* m_next; //!< R�f�rence suivante dans la liste cha�n�e
  ThatClassType* m_prev; //!< R�f�rence pr�c�dente dans la liste cha�n�e
 private:
  //! Interdit
  void operator=(const Array<T>& rhs);
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Collection
 *
 * \brief Vecteur 1D de donn�es avec s�mantique par valeur (style STL).
 *
 * Cette classe g�re un tableau de valeur de la m�me mani�re que la
 * classe stl::vector de la STL.

 * La s�mantique par valeur fonctionne comme suit:
 *
 * \code
 * UniqueArray<int> a1(5);
 * UniqueArray<int> a2;
 * a2 = a1; // a2 devient une copie de a1.
 * a1[3] = 1;
 * a2[3] = 2;
 * std::cout << a1[3]; // affiche '1'
 * \endcode
 *
 * Il est possible de sp�cifier un allocateur m�moire sp�cifique via
 * le constructeur UniqueArray(IMemoryAllocator*). Dans ce cas, l'allocateur
 * sp�cifi� en argument doit rester valide tant que cette instance
 * est utilis�e.
 *
 * \warning L'allocateur est transf�r� � l'instance de destination lors d'un
 * appel au constructeur (UniqueArray(UniqueArray&&) ou assignement
 * (UniqueArray::operator=(UniqueArray&&) par d�placement ainsi que lors
 * de l'appel � UniqueArray::swap(). Si ces appels sont envisag�s, il
 * faut garantir que l'allocateur restera valide m�me apr�s transfert. Si
 * on ne peut pas garantir cela, il est pr�f�rable d'utiliser la
 * classe Array qui ne permet pas un tel transfert.
 */
template<typename T>
class UniqueArray
: public Array<T>
{
 protected:

  using AbstractArray<T>::m_p;
  using Array<T>::BD_NoWarning;

 public:

  typedef AbstractArray<T> BaseClassType;
  typedef typename BaseClassType::ConstReferenceType ConstReferenceType;

 public:

 public:
  //! Cr�� un tableau vide
  UniqueArray() : Array<T>(BD_NoWarning) {}
  //! Cr�� un tableau de \a size �l�ments contenant la valeur \a value.
  UniqueArray(Integer req_size,ConstReferenceType value) : Array<T>(BD_NoWarning)
  {
    this->_resize(req_size,value);
  }
  //! Cr�� un tableau de \a asize �l�ments contenant la valeur par d�faut du type T()
  explicit UniqueArray(Integer asize) : Array<T>(BD_NoWarning)
  {
    this->_resize(asize);
  }
  //! Cr�� un tableau en recopiant les valeurs de la value \a aview.
  UniqueArray(const ConstArrayView<T>& aview) : Array<T>(aview,BD_NoWarning)
  {
  }
  UniqueArray(std::initializer_list<T> alist) : Array<T>(alist,BD_NoWarning)
  {
  }
  //! Cr�� un tableau en recopiant les valeurs \a rhs.
  UniqueArray(const Array<T>& rhs) : Array<T>(rhs.constView(),BD_NoWarning)
  {
  }
  //! Cr�� un tableau en recopiant les valeurs \a rhs.
  UniqueArray(const UniqueArray<T>& rhs) : Array<T>(rhs.constView(),BD_NoWarning)
  {
  }
  //! Cr�� un tableau en recopiant les valeurs \a rhs.
  UniqueArray(const SharedArray<T>& rhs) : Array<T>(rhs.constView(),BD_NoWarning)
  {
  }
  //! Constructeur par d�placement. \a rhs est invalid� apr�s cet appel
  UniqueArray(UniqueArray<T>&& rhs) ARCCORE_NOEXCEPT : Array<T>(std::move(rhs)) {}
  //! Cr�� un tableau vide avec un allocateur sp�cifique \a allocator
  explicit UniqueArray(IMemoryAllocator* allocator) : Array<T>(allocator,0,BD_NoWarning) {}
  /*!
   * \brief Cr�� un tableau de \a asize �l�ments avec un
   * allocateur sp�cifique \a allocator.
   *
   * Si ArrayTraits<T>::IsPODType vaut TrueType, les �l�ments ne sont pas
   * initialis�s. Sinon, c'est le constructeur par d�faut de T qui est utilis�.
   */
  UniqueArray(IMemoryAllocator* allocator,Integer asize)
  : Array<T>(allocator,asize,BD_NoWarning) { }
  //! Copie les valeurs de \a rhs dans cette instance.
  void operator=(const Array<T>& rhs)
  {
    this->copy(rhs.constView());
  }
  //! Copie les valeurs de \a rhs dans cette instance.
  void operator=(const SharedArray<T>& rhs)
  {
    this->copy(rhs.constView());
  }
  //! Copie les valeurs de \a rhs dans cette instance.
  void operator=(const UniqueArray<T>& rhs)
  {
    this->copy(rhs.constView());
  }
  //! Op�rateur de recopie par d�placement. \a rhs est invalid� apr�s cet appel.
  void operator=(UniqueArray<T>&& rhs) ARCCORE_NOEXCEPT
  {
    this->_move(rhs);
  }
  //! Copie les valeurs de la vue \a rhs dans cette instance.
  void operator=(ConstArrayView<T> rhs)
  {
    this->copy(rhs);
  }
  //! D�truit l'instance.
  ~UniqueArray() override
  {
  }
 public:
  /*!
   * \brief �change les valeurs de l'instance avec celles de \a rhs.
   *
   * L'�change se fait en temps constant et sans r�allocation.
   */
  void swap(UniqueArray<T>& rhs)
  {
    this->_swap(rhs);
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief �change les valeurs de \a v1 et \a v2.
 *
 * L'�change se fait en temps constant et sans r�allocation.
 */
template<typename T> inline void
swap(UniqueArray<T>& v1,UniqueArray<T>& v2)
{
  v1.swap(v2);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline SharedArray<T>::
SharedArray(const UniqueArray<T>& rhs)
: Array<T>(rhs.constView(),BD_NoWarning)
, m_next(nullptr)
, m_prev(nullptr)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline void SharedArray<T>::
operator=(const UniqueArray<T>& rhs)
{
  this->copy(rhs);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline std::ostream&
operator<<(std::ostream& o, const AbstractArray<T>& val)
{
  o << ConstArrayView<T>(val);
  return o;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T> inline bool
operator==(const AbstractArray<T>& rhs, const AbstractArray<T>& lhs)
{
  return operator==(ConstArrayView<T>(rhs),ConstArrayView<T>(lhs));
}

template<typename T> inline bool
operator!=(const AbstractArray<T>& rhs, const AbstractArray<T>& lhs)
{
  return !(rhs==lhs);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

extern ARCCORE_COLLECTIONS_EXPORT void
_testArrayNewInternal();

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace std
{
template< class T > inline typename Arccore::ArrayRange<typename Arccore::Array<T>::pointer>::iterator
begin( Arccore::Array<T>& c )
{
  return c.range().begin();
}
template< class T > inline typename Arccore::ArrayRange<typename Arccore::Array<T>::pointer>::iterator
end( Arccore::Array<T>& c )
{
  return c.range().end();
}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  
