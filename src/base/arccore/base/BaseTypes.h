/*---------------------------------------------------------------------------*/
/* BaseTypes.h                                                 (C) 2000-2018 */
/*                                                                           */
/* D�finition des types de la composante 'base' de Arccore.                  */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_BASETYPES_H
#define ARCCORE_BASE_BASETYPES_H
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
 * \file BaseTypes.h
 *
 * \brief D�clarations des types de la composante 'base' de %Arccore.
 */

template<typename T> class IterT;
template<typename T> class ConstIterT;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

//! Type d'un octet
typedef unsigned char Byte;
//! Type d'un octet
typedef signed char SByte;
//! Type d'un caract�re unicode
typedef unsigned short UChar;
//! Type d'un Int16 non sign�
typedef unsigned short UInt16;
//! Type d'un r�el simple pr�cision
typedef float Single;

template<typename T> class ConstArrayView;
template<typename T> class ArrayView;
template<class DataType> class CoreArray;

class StringImpl;
class String;
class StringBuilder;
class StringFormatterArg;

class TraceInfo;
class StackTrace;
class Exception;
class StackFrame;
class FixedStackFrameArray;
class IStackTraceService;

class ArgumentException;
class IndexOutOfRangeException;
class FatalErrorException;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
				
//! Equivalent C d'un tableau � une dimension de pointeurs
typedef ArrayView<Pointer> PointerArrayView;
//! Equivalent C d'un tableau � une dimension de caract�res
typedef ArrayView<Byte> ByteArrayView;
//! Equivalent C d'un tableau � une dimension de caract�res unicode
typedef ArrayView<UChar> UCharArrayView;
//! Equivalent C d'un tableau � une dimension d'entiers 64 bits
typedef ArrayView<Int64> Int64ArrayView;
//! Equivalent C d'un tableau � une dimension d'entiers 32 bits
typedef ArrayView<Int32> Int32ArrayView;
//! Equivalent C d'un tableau � une dimension d'entiers 16 bits
typedef ArrayView<Int16> Int16ArrayView;
//! Equivalent C d'un tableau � une dimension d'entiers
typedef ArrayView<Integer> IntegerArrayView;
//! Equivalent C d'un tableau � une dimension de r�els
typedef ArrayView<Real> RealArrayView;
//! Equivalent C d'un tableau � une dimension de bool�ens
typedef ArrayView<bool> BoolArrayView;

//! Equivalent C d'un tableau � une dimension de pointeurs
typedef ConstArrayView<Pointer> PointerConstArrayView;
//! Equivalent C d'un tableau � une dimension de caract�res
typedef ConstArrayView<Byte> ByteConstArrayView;
//! Equivalent C d'un tableau � une dimension de caract�res unicode
typedef ConstArrayView<UChar> UCharConstArrayView;
//! Equivalent C d'un tableau � une dimension d'entiers 64 bits
typedef ConstArrayView<Int64> Int64ConstArrayView;
//! Equivalent C d'un tableau � une dimension d'entiers 32 bits
typedef ConstArrayView<Int32> Int32ConstArrayView;
//! Equivalent C d'un tableau � une dimension d'entiers 16 bits
typedef ConstArrayView<Int16> Int16ConstArrayView;
//! Equivalent C d'un tableau � une dimension d'entiers
typedef ConstArrayView<Integer> IntegerConstArrayView;
//! Equivalent C d'un tableau � une dimension de r�els
typedef ConstArrayView<Real> RealConstArrayView;
//! Equivalent C d'un tableau � une dimension de bool�ens
typedef ConstArrayView<bool> BoolConstArrayView;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  
