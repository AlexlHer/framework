/*---------------------------------------------------------------------------*/
/* String.h                                                    (C) 2000-2018 */
/*                                                                           */
/* Cha�ne de caract�re unicode.                                              */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_STRING_H
#define ARCCORE_BASE_STRING_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/BaseTypes.h"

#include <string>
#include <sstream>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class StringFormatterArg;
class StringBuilder;
class StringImpl;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Core
 * \brief Chaine de caract�res unicode.
 *
 * Utilise un compteur de r�f�rence avec s�mantique indentique � QString.
 * A terme la class 'String' doit �tre immutable pour �viter des
 * probl�mes en multi-thread.
 * Pour construire par morceaux une chaine de caract�re, il faut
 * utiliser la classe 'StringBuilder'.
 * \warning Il ne doit pas y avoir de variables globales de cette classe
 */
class ARCCORE_BASE_EXPORT String
{
 public:

  friend ARCCORE_BASE_EXPORT bool operator==(const String& a,const String& b);
  friend ARCCORE_BASE_EXPORT bool operator<(const String& a,const String& b);
  friend class StringBuilder;

 public:

  //! Cr�e une cha�ne nulle
  String() : m_p(nullptr), m_const_ptr(nullptr) {}
  /*!
   * \brief Cr�� une cha�ne � partir de \a str dans l'encodage local.
   *
   * \warning Attention, la chaine est suppos�e constante sa validit�
   * infinie (i.e il s'agit d'une cha�ne constante � la compilation.
   * Si la cha�ne pass�e en argument peut �tre d�sallou�e,
   * il faut utiliser le constructeur avec allocation.
   */   
  String(const char* str) : m_p(nullptr), m_const_ptr(str) {}
  //! Cr�� une cha�ne � partir de \a str dans l'encodage local
  String(char* str);
  //! Cr�� une cha�ne � partir de \a str dans l'encodage local
  String(const char* str,bool do_alloc);
  //! Cr�� une cha�ne � partir de \a str dans l'encodage local
  String(const char* str,Integer len);
  //! Cr�� une cha�ne � partir de \a str dans l'encodage local
  String(const std::string& str);
  //! Cr�� une cha�ne � partir de \a str dans l'encodage Utf16
  String(const UCharConstArrayView& ustr);
  //! Cr�� une cha�ne � partir de \a str dans l'encodage Utf8
  String(const ConstLargeArrayView<Byte>& ustr);
  //! Cr�� une cha�ne � partir de \a str dans l'encodage local
  explicit String(StringImpl* impl);
  //! Cr�� une cha�ne � partir de \a str
  String(const String& str);

  //! Copie \a str dans cette instance.
  const String& operator=(const String& str);
  //! Copie \a str dans cette instance.
  const String& operator=(const char* str);

  ~String(); //!< Lib�re les ressources.

 public:

  static String fromUtf8(ConstLargeArrayView<Byte> bytes);

 public:

  /*!
   * \brief Retourne la conversion de l'instance dans l'encodage UTF-16.
   *
   * Le tableau retourn� contient toujours un z�ro terminal si la cha�ne n'est
   * par nulle. Par cons�quent, la taille de toute cha�ne non nulle est
   * celle du tableau moins 1.
   *
   * \warning L'instance reste propri�taire de la valeur retourn�e et cette valeur
   * est invalid�e par toute modification de cette instance.
   */
  ConstArrayView<UChar> utf16() const;

  /*!
   * \brief Retourne la conversion de l'instance dans l'encodage UTF-8.
   *
   * Le tableau retourn� contient toujours un z�ro terminal si la cha�ne n'est
   * par nulle. Par cons�quent, la taille de toute cha�ne non nulle est
   * celle du tableau moins 1.
   *
   * \warning L'instance reste propri�taire de la valeur retourn�e et cette valeur
   * est invalid�e par toute modification de cette instance.
   */
  ByteConstArrayView utf8() const;

  /*!
   * \brief Retourne la conversion de l'instance dans l'encodage UTF-8.
   *
   * L'instance retourn�e ne contient pas de z�ro terminal.
   *
   * \warning L'instance reste propri�taire de la valeur retourn�e et cette valeur
   * est invalid�e par toute modification de cette instance.
   */
  ConstLargeArrayView<Byte> bytes() const;

  /*!
   * \brief Retourne la conversion de l'instance dans l'encodage local.
   *
   * La conversion n'est pas garanti si certaines valeurs unicode n'existent
   * pas dans l'encodage local.
   *
   * \warning L'instance reste propri�taire de la valeur retourn�e et cette valeur
   * est invalid�e par toute modification de cette instance.
   */
  const char* localstr() const;

 public:

  //! Clone cette cha�ne.
  String clone() const;

  /*!
   * \brief Effectue une normalisation des caract�res espaces.
   *
   * Tous les caract�res espaces sont remplac�s par des blancs espaces #x20,
   * � savoir #xD (Carriage Return), #xA (Line Feed) et #x9 (Tabulation).
   * Cela correspond � l'attribut xs:replace de XMLSchema 1.0
   */
  static String replaceWhiteSpace(const String& rhs);

  /*!
   * \brief Effectue une normalisation des caract�res espaces.
   *
   * Le comportement est identique � replaceWhiteSpace() avec en plus:
   * - remplacement de tous les blancs cons�cutifs par un seul.
   * - suppression des blancs en d�but et fin de cha�ne.
   * Cela correspond � l'attribut xs:collapse de XMLSchema 1.0
   */
  static String collapseWhiteSpace(const String& rhs);


  //! Transforme tous les caracteres de la chaine en majuscules.
  String upper() const;

  //! Transforme tous les caracteres de la chaine en minuscules.
  String lower() const;

  //! Retourne \a true si la cha�ne est nulle.
  bool null() const;

  //! Retourne la longueur de la cha�ne.
  Integer len() const;
  
  //! Vrai si la cha�ne est vide (nulle ou "")
  bool empty() const;

  //! Calcule une valeur de hashage cette la cha�ne de caract�re
  Int32 hashCode() const;

  //! �crit la cha�ne au format UTF-8 sur le flot \a o
  void writeBytes(std::ostream& o) const;

 public:

  String operator+(const char* str) const;
  String operator+(const String& str) const;
  String operator+(unsigned long v) const;
  String operator+(unsigned int v) const;
  String operator+(double v) const;
  String operator+(long double v) const;
  String operator+(int v) const;
  String operator+(long v) const;
  String operator+(unsigned long long v) const;
  String operator+(long long v) const;
  String operator+(const APReal& v) const;

  static String fromNumber(unsigned long v);
  static String fromNumber(unsigned int v);
  static String fromNumber(double v);
  static String fromNumber(double v,Integer nb_digit_after_point);
  static String fromNumber(long double v);
  static String fromNumber(int v);
  static String fromNumber(long v);
  static String fromNumber(unsigned long long v);
  static String fromNumber(long long v);
  static String fromNumber(const APReal& v);

 public:

  static String format(const String& str);
  static String format(const String& str,const StringFormatterArg& arg1);
  static String format(const String& str,const StringFormatterArg& arg1,
                       const StringFormatterArg& arg2);
  static String format(const String& str,const StringFormatterArg& arg1,
                       const StringFormatterArg& arg2,
                       const StringFormatterArg& arg3);
  static String format(const String& str,const StringFormatterArg& arg1,
                       const StringFormatterArg& arg2,
                       const StringFormatterArg& arg3,
                       const StringFormatterArg& arg4);
  static String format(const String& str,const StringFormatterArg& arg1,
                       const StringFormatterArg& arg2,
                       const StringFormatterArg& arg3,
                       const StringFormatterArg& arg4,
                       const StringFormatterArg& arg5);
  static String format(const String& str,const StringFormatterArg& arg1,
                       const StringFormatterArg& arg2,
                       const StringFormatterArg& arg3,
                       const StringFormatterArg& arg4,
                       const StringFormatterArg& arg5,
                       const StringFormatterArg& arg6);
  static String format(const String& str,const StringFormatterArg& arg1,
                       const StringFormatterArg& arg2,
                       const StringFormatterArg& arg3,
                       const StringFormatterArg& arg4,
                       const StringFormatterArg& arg5,
                       const StringFormatterArg& arg6,
                       const StringFormatterArg& arg7);
  static String format(const String& str,const StringFormatterArg& arg1,
                       const StringFormatterArg& arg2,
                       const StringFormatterArg& arg3,
                       const StringFormatterArg& arg4,
                       const StringFormatterArg& arg5,
                       const StringFormatterArg& arg6,
                       const StringFormatterArg& arg7,
                       const StringFormatterArg& arg8);
  static String format(const String& str,const StringFormatterArg& arg1,
                       const StringFormatterArg& arg2,
                       const StringFormatterArg& arg3,
                       const StringFormatterArg& arg4,
                       const StringFormatterArg& arg5,
                       const StringFormatterArg& arg6,
                       const StringFormatterArg& arg7,
                       const StringFormatterArg& arg8,
                       const StringFormatterArg& arg9);
  static String concat(const StringFormatterArg& arg1);
  static String concat(const StringFormatterArg& arg1,
                       const StringFormatterArg& arg2);
  static String concat(const StringFormatterArg& arg1,
                       const StringFormatterArg& arg2,
                       const StringFormatterArg& arg3);
  static String concat(const StringFormatterArg& arg1,
                       const StringFormatterArg& arg2,
                       const StringFormatterArg& arg3,
                       const StringFormatterArg& arg4);

  //! Forme standard du pluriel par ajout d'un 's'
  static String plural(const Integer n, const String & str, const bool with_number = true);
  //! Forme particuli�re du pluriel par variante
  static String plural(const Integer n, const String & str, const String & str2, const bool with_number = true);

  //! Indique si la cha�ne contient \a s
  bool contains(const String& s) const;

  //! Indique si la cha�ne commence par les caract�res de \a s
  bool startsWith(const String& s) const;

  //! Indique si la cha�ne se termine par les caract�res de \a s
  bool endsWith(const String& s) const;

  //! Sous-cha�ne commen�ant � la position \a pos
  String substring(Integer pos) const;

  //! Sous-cha�ne commen�ant � la position \a pos et de longueur \a len
  String substring(Integer pos,Integer len) const;

  static String join(String delim,ConstArrayView<String> strs);

  //! D�coupe la cha�ne suivant le caract�re \a c
  template<typename StringContainer> void
  split(StringContainer& str_array,char c) const
  {
    const String& str = *this;
    const char* str_str = str.localstr();
    Integer offset = 0;
    Integer len = str.len();
    for( Integer i=0; i<len; ++i ){
      // GG: remet temporairement l'ancienne s�mantique (�quivalente � strtok())
      // et supprime la modif IFPEN car cela cause trop d'incompatibilit�s avec
      // le code existant. A noter que l'impl�mentation de l'ancienne s�mantique
      // a plusieurs bugs:
      //   1. dans le cas o� on r�p�te 3 fois ou plus cons�cutivement le
      // d�limiteur. Par exemple 'X:::Y' retourne {'X',':','Y'} au lieu de
      // {'X','Y'}
      //   2. Si on commence par le d�limiteur, ce dernier est retourn�:
      // Avec ':X:Y', on retourne {':X','Y'} au lieu de {'X','Y'}
      //if (str_str[i]==c){
      if (str_str[i]==c && i!=offset){
        str_array.push_back(String(str_str+offset,i-offset));
        offset = i+1;
      }
    }
    if (len!=offset)
      str_array.push_back(String(str_str+offset,len-offset));
  }

 public:
  /*!
   * \brief Affiche les infos internes de la classe.
   *
   * Cette m�thode n'est utile que pour d�bugger Arccore
   */
  void internalDump(std::ostream& ostr) const;

 private:

  mutable StringImpl* m_p; //!< Impl�mentation de la classe
  mutable const char* m_const_ptr;

  void _checkClone() const;
  bool isLess(const String& s) const;
  String& _append(const String& str);
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

//! Op�rateur d'�criture d'une String
ARCCORE_BASE_EXPORT std::ostream& operator<<(std::ostream& o,const String&);
//! Op�rateur de lecture d'une String
ARCCORE_BASE_EXPORT std::istream& operator>>(std::istream& o,String&);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Compare deux cha�nes unicode.
 * \retval true si elles sont �gales,
 * \retval false sinon.
 * \relate String
 */
extern "C++" ARCCORE_BASE_EXPORT bool operator==(const String& a,const String& b);
/*!
 * \brief Compare deux cha�nes unicode.
 * \retval true si elles sont diff�rentes,
 * \retval false si elles sont �gales.
 * \relate String
 */
inline bool operator!=(const String& a,const String& b)
{
  return !operator==(a,b);
}

/*!
 * \brief Compare deux cha�nes unicode.
 * \retval true si elles sont �gales,
 * \retval false sinon.
 * \relate String
 */
extern "C++" ARCCORE_BASE_EXPORT bool operator==(const char* a,const String& b);

/*!
 * \brief Compare deux cha�nes unicode.
 * \retval true si elles sont diff�rentes,
 * \retval false si elles sont �gales.
 * \relate String
 */
inline bool operator!=(const char* a,const String& b)
{
  return !operator==(a,b);
}

/*!
 * \brief Compare deux cha�nes unicode.
 * \retval true si elles sont �gales,
 * \retval false sinon.
 * \relate String
 */
extern "C++" ARCCORE_BASE_EXPORT bool operator==(const String& a,const char* b);
/*!
 * \brief Compare deux cha�nes unicode.
 * \retval true si elles sont diff�rentes,
 * \retval false si elles sont �gales.
 * \relate String
 */
inline bool operator!=(const String& a,const char* b)
{
  return !operator==(a,b);
}

//! Ajoute deux cha�nes.
extern "C++" ARCCORE_BASE_EXPORT String operator+(const char* a,const String& b);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Compare deux cha�nes unicode.
 * \retval true si a<b
 * \retval false sinon.
 * \relate String
 */
extern "C++" ARCCORE_BASE_EXPORT bool operator<(const String& a,const String& b);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename U>
class StringFormatterArgToString
{
 public:
  static void toString(const U& v,String& s)
  {
    std::ostringstream ostr;
    ostr << v;
    s = ostr.str();
  }
};

/*!
 * \internal
 * \brief Classe utilis�e pour formatter une cha�ne de caract�res.
 */
class ARCCORE_BASE_EXPORT StringFormatterArg
{
 public:
  template<typename U>
  StringFormatterArg(const U& avalue)
  {
    StringFormatterArgToString<U>::toString(avalue,m_str_value);
  }
  StringFormatterArg(Real avalue)
  {
    _formatReal(avalue);
  }
  StringFormatterArg(const String& s)
  : m_str_value(s)
  {
  }
 public:
  const String& value() const { return m_str_value; }
 private:
  String m_str_value;
 private:
  void _formatReal(Real avalue);
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif
