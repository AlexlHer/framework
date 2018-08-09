/*---------------------------------------------------------------------------*/
/* Array4View.h                                                (C) 2000-2018 */
/*                                                                           */
/* Vue d'un tableau 4D.                                                      */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_ARRAY4VIEW_H
#define ARCCORE_BASE_ARRAY4VIEW_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/Array3View.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Collection
 * \brief Vue pour un tableau 4D.
 *
 * Cette classe permet d'obtenir une vue 4D � partir d'une zone contigue
 * en m�moire, comme par exemple celle obtenue via la classe Array.
 *
 * La vue peut s'utiliser comme un tableau C classique, par exemple:
 * \code
 * Array4View<Real> a;
 * a[0][1][2][3] = 5.0;
 * \endcode
 *
 * Il est n�ammoins pr�f�rable d'utiliser directement les m�thodes
 * item() ou setItem() pour acc�der en lecture ou �criture � un �l�ment
 * du tableau.
 */
template<class DataType>
class Array4View
{
 public:
  //! Construit une vue
  Array4View(DataType* ptr,Integer dim1_size,Integer dim2_size,
             Integer dim3_size,Integer dim4_size)
  : m_ptr(ptr), m_dim1_size(dim1_size), m_dim2_size(dim2_size), m_dim3_size(dim3_size),
    m_dim4_size(dim4_size), m_dim34_size(dim3_size*dim4_size),
    m_dim234_size(m_dim34_size*dim2_size)
  {
  }
  //! Construit une vue vide
  Array4View()
  : m_ptr(0), m_dim1_size(0), m_dim2_size(0), m_dim3_size(0), m_dim4_size(0),
    m_dim34_size(0), m_dim234_size(0)
  {
  }
 public:
  //! Valeur de la premi�re dimension
  Integer dim1Size() const { return m_dim1_size; }
  //! Valeur de la deuxi�me dimension
  Integer dim2Size() const { return m_dim2_size; }
  //! Valeur de la troisi�me dimension
  Integer dim3Size() const { return m_dim3_size; }
  //! Valeur de la quatri�me dimension
  Integer dim4Size() const { return m_dim4_size; }
  //! Nombre total d'�l�ments du tableau
  Integer totalNbElement() const { return m_dim1_size*m_dim234_size; }
 public:
  Array3View<DataType> operator[](Integer i)
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    return Array3View<DataType>(m_ptr + (m_dim234_size*i),m_dim2_size,m_dim3_size,m_dim4_size);
  }
  ConstArray3View<DataType> operator[](Integer i) const
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    return ConstArray3View<DataType>(m_ptr + (m_dim234_size*i),m_dim2_size,m_dim3_size,m_dim4_size);
  }
  //! Valeur pour l'�l�ment \a i,j,k,l
  DataType item(Integer i,Integer j,Integer k,Integer l) const
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    ARCCORE_CHECK_AT(j,m_dim2_size);
    ARCCORE_CHECK_AT(k,m_dim3_size);
    ARCCORE_CHECK_AT(l,m_dim4_size);
    return m_ptr[(m_dim234_size*i) + m_dim34_size*j + m_dim4_size*k + l];
  }
  //! Positionne la valeur pour l'�l�ment \a i,j,k,l
  void setItem(Integer i,Integer j,Integer k,Integer l,const DataType& value)
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    ARCCORE_CHECK_AT(j,m_dim2_size);
    ARCCORE_CHECK_AT(k,m_dim3_size);
    ARCCORE_CHECK_AT(l,m_dim4_size);
    m_ptr[(m_dim234_size*i) + m_dim34_size*j + m_dim4_size*k + l] = value;
  }
 public:
  /*!
   * \brief Pointeur sur le premier �l�ment du tableau.
   */
  inline DataType* unguardedBasePointer()
  { return m_ptr; }
 private:
  DataType* m_ptr;
  Integer m_dim1_size; //!< Taille de la 1ere dimension
  Integer m_dim2_size; //!< Taille de la 2eme dimension
  Integer m_dim3_size; //!< Taille de la 3eme dimension
  Integer m_dim4_size; //!< Taille de la 4eme dimension
  Integer m_dim34_size; //!< dim3 * dim4
  Integer m_dim234_size; //!< dim2 * dim3 * dim4
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Collection
 * \brief Vue constante pour un tableau 4D
 */
template<class DataType>
class ConstArray4View
{
 public:
  ConstArray4View(DataType* ptr,Integer dim1_size,Integer dim2_size,
                  Integer dim3_size,Integer dim4_size)
  : m_ptr(ptr), m_dim1_size(dim1_size), m_dim2_size(dim2_size), m_dim3_size(dim3_size),
    m_dim4_size(dim4_size), m_dim34_size(dim3_size*dim4_size),
    m_dim234_size(m_dim34_size*dim2_size)
  {
  }
  ConstArray4View()
  : m_ptr(0), m_dim1_size(0), m_dim2_size(0), m_dim3_size(0), m_dim4_size(0),
    m_dim34_size(0), m_dim234_size(0)
  {
  }
 public:
  Integer dim1Size() const { return m_dim1_size; }
  Integer dim2Size() const { return m_dim2_size; }
  Integer dim3Size() const { return m_dim3_size; }
  Integer dim4Size() const { return m_dim4_size; }
  Integer totalNbElement() const { return m_dim1_size*m_dim234_size; }
 public:
  ConstArray3View<DataType> operator[](Integer i) const
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    return ConstArray3View<DataType>(m_ptr + (m_dim234_size*i),m_dim2_size,m_dim3_size,m_dim4_size);
  }
  DataType item(Integer i,Integer j,Integer k,Integer l) const
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    ARCCORE_CHECK_AT(j,m_dim2_size);
    ARCCORE_CHECK_AT(k,m_dim3_size);
    ARCCORE_CHECK_AT(l,m_dim4_size);
    return m_ptr[(m_dim234_size*i) + m_dim34_size*j + m_dim4_size*k + l];
  }
 public:
  /*!
   * \brief Pointeur sur la m�moire allou�e.
   */
  inline const DataType* unguardedBasePointer()
  { return m_ptr; }
 private:
  const DataType* m_ptr;
  Integer m_dim1_size; //!< Taille de la 1ere dimension
  Integer m_dim2_size; //!< Taille de la 2eme dimension
  Integer m_dim3_size; //!< Taille de la 3eme dimension
  Integer m_dim4_size; //!< Taille de la 4eme dimension
  Integer m_dim34_size; //!< dim3 * dim4
  Integer m_dim234_size; //!< dim2 * dim3 * dim4
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  
