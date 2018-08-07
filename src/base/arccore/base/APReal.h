/*---------------------------------------------------------------------------*/
/* APReal.h                                                    (C) 2000-2018 */
/*                                                                           */
/* R�el en pr�cision arbitraire.                                             */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_APREAL_H
#define ARCCORE_BASE_APREAL_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArccoreGlobal.h"

#ifndef ARCCORE_REAL_USE_APFLOAT
#include <iostream>
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Emulation d'un r�el en pr�cision arbitraire.
 *
 * Si on souhaite une pr�cision arbitraire, il faut utiliser la biblioth�que
 * 'apfloat'. Cette classe d�finit un type pour le cas o� cette biblioth�que
 * n'est pas disponible mais n'assure pas une pr�cision arbitraire.
 * apfloat Cette classe �mule la clas
 */
#ifndef ARCCORE_REAL_USE_APFLOAT

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline bool
operator<(const APReal& a,const APReal& b)
{
  return a.v[0] < b.v[0];
}

inline bool
operator>(const APReal& a,const APReal& b)
{
  return a.v[0] > b.v[0];
}

inline bool
operator==(const APReal& a,const APReal& b)
{
  return a.v[0]==b.v[0];
}

inline bool
operator!=(const APReal& a,const APReal& b)
{
  return !operator==(a,b);
}

inline APReal
operator+(const APReal& a,const APReal& b)
{
  APReal result;
  result.v[0] = a.v[0] + b.v[0];
  return result;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline std::ostream&
operator<< (std::ostream& o,APReal t)
{
  o << t.v[0];
  return o;
}

inline std::istream&
operator>> (std::istream& i,APReal& t)
{
  i >> t.v[0];
  return i;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif

