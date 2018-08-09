/*---------------------------------------------------------------------------*/
/* IMemoryAllocator.h                                          (C) 2000-2018 */
/*                                                                           */
/* Interface d'un allocateur m�moire.                                        */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_COLLECTIONS_IMEMORYALLOCATOR_H
#define ARCCORE_COLLECTIONS_IMEMORYALLOCATOR_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/collections/CollectionsGlobal.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Interface d'un allocateur pour la m�moire.
 *
 * Cette classe d�finit une interface pour l'allocation m�moire utilis�e
 * par les classes tableaux de Arccore (Array, UniqueArray).
 *
 * Une instance de cette classe doit rester valide tant qu'il existe
 * des tableaux l'utilisant.
 */
class ARCCORE_COLLECTIONS_EXPORT IMemoryAllocator
{
 public:

  /*!
   * \brief D�truit l'allocateur.
   *
   * Les objets allou�s par l'allocateur doivent tous avoir �t� d�sallou�s.
   */
  virtual ~IMemoryAllocator() {}

 public:

  /*!
   * \brief Indique si l'allocateur supporte la s�mantique de realloc.
   *
   * Les allocateurs par d�faut du C (malloc/realloc/free) supportent
   * �videmment le realloc mais ce n'est pas forc�ment le cas
   * des allocateurs sp�cifiques avec alignement m�moire (comme
   * par exemple posix_memalign).
   */
  virtual bool hasRealloc() const =0;

  /*!
   * \brief Alloue de la m�moire pour \a new_size octets et retourne le pointeur.
   *
   * La s�mantique est �quivalent � malloc():
   * - \a new_size peut valoir z�ro et dans ce cas le pointeur retourn�
   * est soit nul, soit une valeur sp�cifique
   * - le pointeur retourn� peut �tre nul si la m�moire n'a pas pu �tre allou�e.
   */
  virtual void* allocate(size_t new_size) =0;

  /*!
   * \brief R�alloue de la m�moire pour \a new_size octets et retourne le pointeur.
   *
   * Le pointeur \a current_ptr doit avoir �t� allou� via l'appel �
   * allocate() ou reallocate() de cette instance.
   *
   * La s�mantique de cette m�thode est �quivalente � realloc():
   * - \a current_ptr peut-�tre nul auquel cas cet appel est �quivalent
   * � allocate().
   * - le pointeur retourn� peut �tre nul si la m�moire n'a pas pu �tre allou�e.
   */
  virtual void* reallocate(void* current_ptr,size_t new_size) =0;

  /*!
   * \brief Lib�re la m�moire dont l'adresse de base est \a ptr.
   *
   * Le pointeur \a ptr doit avoir �t� allou� via l'appel �
   * allocate() ou reallocate() de cette instance.
   *
   * La s�mantique de cette m�thode �quivalente � free() et donc \a ptr
   * peut �tre nul auquel cas aucune op�ration n'est effectu�e.
   */
  virtual void deallocate(void* ptr) =0;

  /*!
   * \brief Ajuste la capacit� suivant la taille d'�l�ment.
   *
   * Cette m�thode est utilis�e pour �ventuellement modifi� le nombre
   * d'�l�ments allou�s suivant leur taille. Cela permet par exemple
   * pour les allocateurs align�s de garantir que le nombre d'�l�ments
   * allou�s est un multiple de cet alignement.
   * 
   */
  virtual size_t adjustCapacity(size_t wanted_capacity,size_t element_size) =0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Allocateur m�moire via malloc/realloc/free.
 *
 * TODO: marquer les m�thodes comme 'final'.
 */
class ARCCORE_COLLECTIONS_EXPORT DefaultMemoryAllocator
: public IMemoryAllocator
{
  friend class ArrayImplBase;

 private:

  static DefaultMemoryAllocator shared_null_instance;

 public:

  bool hasRealloc() const override;
  void* allocate(size_t new_size) override;
  void* reallocate(void* current_ptr,size_t new_size) override;
  void deallocate(void* ptr) override;
  size_t adjustCapacity(size_t wanted_capacity,size_t element_size) override;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Allocateur m�moire avec alignement m�moire sp�cifique.
 *
 * Cette classe s'utilise via les deux m�thodes publiques Simd()
 * et CacheLine() qui retournent repectivement un allocateur avec
 * un alignement ad�quat pour autoriser la vectorisation et un allocateur
 * align� sur une ligne de cache.
 */
class ARCCORE_COLLECTIONS_EXPORT AlignedMemoryAllocator
: public IMemoryAllocator
{
 private:

  static AlignedMemoryAllocator SimdAllocator;
  static AlignedMemoryAllocator CacheLineAllocator;

 public:

  // TODO: essayer de trouver les bonnes valeurs en fonction de la cible.
  // 64 est OK pour toutes les architectures x64 � la fois pour le SIMD
  // et la ligne de cache.

  // IMPORTANT: Si on change la valeur ici, il faut changer la taille de
  // l'alignement de ArrayImplBase.

  // TODO Pour l'instant seul un alignement sur 64 est autoris�. Pour
  // autoriser d'autres valeurs, il faut modifier l'impl�mentation dans
  // ArrayImplBase.

  // TODO marquer les m�thodes comme 'final'.

  //! Alignement pour les structures utilisant la vectorisation
  static ARCCORE_CONSTEXPR Integer simdAlignment() { return 64; }
  //! Alignement pour une ligne de cache.
  static ARCCORE_CONSTEXPR Integer cacheLineAlignment() { return 64; }

  /*!
   * \brief Allocateur garantissant l'alignement pour utiliser
   * la vectorisation sur la plateforme cible.
   *
   * Il s'agit de l'alignement pour le type plus restrictif et donc il
   * est possible d'utiliser cet allocateur pour toutes les structures vectorielles.
   */
  static AlignedMemoryAllocator* Simd()
  {
    return &SimdAllocator;
  }

  /*!
   * \brief Allocateur garantissant l'alignement sur une ligne de cache.
   */
  static AlignedMemoryAllocator* CacheLine()
  {
    return &CacheLineAllocator;
  }

 protected:

  AlignedMemoryAllocator(Integer alignment)
  : m_alignment((size_t)alignment){}

 public:

  bool hasRealloc() const override;
  void* allocate(size_t new_size) override;
  void* reallocate(void* current_ptr,size_t new_size) override;
  void deallocate(void* ptr) override;
  size_t adjustCapacity(size_t wanted_capacity,size_t element_size) override;

 private:

  size_t m_alignment;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Allocateur m�moire via malloc/realloc/free avec impression listing.
 *
 * Cet allocateur est principalement utilis� � des fins de debugging.
 * La sortie des informations se fait sur std::cout.
 */
class ARCCORE_COLLECTIONS_EXPORT PrintableMemoryAllocator
: public IMemoryAllocator
{
 public:

  bool hasRealloc() const override;
  void* allocate(size_t new_size) override;
  void* reallocate(void* current_ptr,size_t new_size) override;
  void deallocate(void* ptr) override;
  size_t adjustCapacity(size_t wanted_capacity,size_t element_size) override;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  

