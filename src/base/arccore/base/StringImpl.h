/*---------------------------------------------------------------------------*/
/* StringImpl.h                                                (C) 2000-2018 */
/*                                                                           */
/* Impl�mentation d'une cha�ne de caract�re de unicode.                      */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_STRINGIMPL_H
#define ARCCORE_BASE_STRINGIMPL_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/CoreArray.h"
#include "arccore/base/BaseTypes.h"

#include <atomic>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 *
 * \brief Impl�mentation de la classe String.
 */
class ARCCORE_BASE_EXPORT StringImpl
{
 public:
  StringImpl(const char* str);
  StringImpl(const char* str,Int64 len);
  StringImpl(const UChar* str);
  StringImpl(const StringImpl& str);
  StringImpl(Span<const Byte> bytes);
 private:
  StringImpl();
 public:
  const std::string& local();
  //TODO: rendre obsol�te
  UCharConstArrayView utf16();
  //TODO: rendre obsol�te
  ByteConstArrayView utf8();
  Span<const Byte> largeUtf8();
  bool isEqual(StringImpl* str);
  bool isLessThan(StringImpl* str);
  bool isEqual(const char* str);
  bool isLessThan(const char* str);
 public:
  void addReference();
  void removeReference();
  Int32 nbReference() { return m_nb_ref.load(); }
 public:
  void internalDump(std::ostream& ostr);
 public:
  StringImpl* clone();
  StringImpl* append(StringImpl* str);
  StringImpl* append(const char* str);
  StringImpl* replaceWhiteSpace();
  StringImpl* collapseWhiteSpace();
  StringImpl* toUpper();
  StringImpl* toLower();
  static StringImpl* substring(StringImpl* str,Int64 pos,Int64 len);

 public:
  bool null() { return false; }
  bool empty();
  bool hasLocal() const { return (m_flags & eValidLocal); }
  bool hasUtf8() const { return (m_flags & eValidUtf8); }
  bool hasUtf16() const { return (m_flags & eValidUtf16); }
 private:
  enum
  {
    eValidUtf16 = 1 << 0,
    eValidUtf8 = 1 << 1,
    eValidLocal = 1 << 2
  };
  std::atomic<Int32> m_nb_ref;
  int m_flags;
  std::string m_local_str;
  CoreArray<UChar> m_utf16_array;
  CoreArray<Byte> m_utf8_array;

  void _setUtf16(const UChar* src);
  void _createUtf16();
  void _setUtf8(const Byte* src);
  void _createUtf8();
  void _createLocal();
  inline void _checkReference();
  void _invalidateLocal();
  void _invalidateUtf16();
  void _invalidateUtf8();
  void _setArray();
  void _setStrFromArray(Int64 ulen);
  void _printStrUtf16(std::ostream& o,Span<const UChar> str);
  void _printStrUtf8(std::ostream& o,Span<const Byte> str);
  void _appendUtf8(Span<const Byte> ref_str);
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif

