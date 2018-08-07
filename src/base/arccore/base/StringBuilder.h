/*---------------------------------------------------------------------------*/
/* StringBuilder.h                                             (C) 2000-2018 */
/*                                                                           */
/* Constructeur de cha�ne de caract�re unicode.                              */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_STRINGBUILDER_H
#define ARCCORE_BASE_STRINGBUILDER_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/BaseTypes.h"

#include <string>
#include <sstream>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class String;
class StringImpl;
class StringFormatterArg;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Core
 * \brief Constructeur de cha�ne de caract�re unicode.
 *
 * Permet de construire de mani�re efficace une cha�ne de caract�re
 * par concat�nation.
 *
 * \not_thread_safe
 */
class ARCCORE_BASE_EXPORT StringBuilder
{
 public:

 public:

  //! Cr�e une cha�ne nulle
  StringBuilder() : m_p(0), m_const_ptr(0) {}
  //! Cr�� une cha�ne � partir de \a str dans l'encodage local
  StringBuilder(const char* str);
  //! Cr�� une cha�ne � partir de \a str dans l'encodage local
  StringBuilder(const char* str,Integer len);
  //! Cr�� une cha�ne � partir de \a str dans l'encodage local
  StringBuilder(const std::string& str);
  //! Cr�� une cha�ne � partir de \a str dans l'encodage Utf16
  StringBuilder(const UCharConstArrayView& ustr);
  //! Cr�� une cha�ne � partir de \a str dans l'encodage Utf8
  StringBuilder(const ByteConstArrayView& ustr);
  //! Cr�� une cha�ne � partir de \a str_builder
  StringBuilder(const StringBuilder& str_builder);
  //! Cr�� une cha�ne � partir de \a str dans l'encodage local
  explicit StringBuilder(StringImpl* impl);
  //! Cr�� une cha�ne � partir de \a str
  StringBuilder(const String& str);

  //! Copie \a str dans cette instance.
  const StringBuilder& operator=(const String& str);
  //! Copie \a str dans cette instance.
  const StringBuilder& operator=(const char* str);
  //! Copie \a str dans cette instance.
  const StringBuilder& operator=(const StringBuilder& str);

  ~StringBuilder(); //!< Lib�re les ressources.

 public:

 public:

  /*!
   * \brief Retourne la cha�ne de caract�res construite.
   */
  operator String() const;

  /*!
   * \brief Retourne la cha�ne de caract�res construite.
   */
  String toString() const;

 public:

 public:

  //! Ajoute \a str.
  StringBuilder& append(const String& str);

  //! Clone cette cha�ne.
  StringBuilder clone() const;

  /*! \brief Effectue une normalisation des caract�res espaces.
   * Tous les caract�res espaces sont remplac�s par des blancs espaces #x20,
   * � savoir #xD (Carriage Return), #xA (Line Feed) et #x9 (Tabulation).
   * Cela correspond � l'attribut xs:replace de XMLSchema 1.0
   */
  StringBuilder& replaceWhiteSpace();

  /*! \brief Effectue une normalisation des caract�res espaces.
   * Le comportement est identique � replaceWhiteSpace() avec en plus:
   * - remplacement de tous les blancs cons�cutifs par un seul.
   * - suppression des blancs en d�but et fin de cha�ne.
   * Cela correspond � l'attribut xs:collapse de XMLSchema 1.0
   */
  StringBuilder& collapseWhiteSpace();

  //! Transforme tous les caracteres de la chaine en majuscules.
  StringBuilder& toUpper();

  //! Transforme tous les caracteres de la chaine en minuscules.
  StringBuilder& toLower();

  void operator+=(const char* str);
  void operator+=(const String& str);
  void operator+=(unsigned long v);
  void operator+=(unsigned int v);
  void operator+=(double v);
  void operator+=(long double v);
  void operator+=(int v);
  void operator+=(char v);
  void operator+=(long v);
  void operator+=(unsigned long long v);
  void operator+=(long long v);
  void operator+=(const APReal& v);

 public:
  /*!
   * \brief Affiche les infos internes de la classe.
   *
   * Cette m�thode n'est utile que pour d�bugger %Arccore
   */
  void internalDump(std::ostream& ostr) const;

 private:

  mutable StringImpl* m_p; //!< Impl�mentation de la classe
  mutable const char* m_const_ptr;

  void _checkClone() const;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

//! Op�rateur d'�criture d'une StringBuilder
ARCCORE_BASE_EXPORT std::ostream& operator<<(std::ostream& o,const StringBuilder&);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif
