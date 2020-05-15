//
// Created by dechaiss on 5/15/20.
//

#ifndef NEO_UTILS_H
#define NEO_UTILS_H

/*-------------------------
 * sdc - (C) 2020
 * NEtwork Oriented kernel
 * Utils
 *--------------------------
 */

#include <iostream>
#include <vector>
#include <cassert>

namespace Neo {
namespace utils {
  using Int64 = long int;
  using Int32 = int;
  struct Real3 { double x,y,z;};
  template <typename T>
  struct ArrayView {
    std::size_t m_size = 0;
    T* m_ptr = nullptr;
    T& operator[](int i) {assert(i<m_size); return *(m_ptr+i);}
    T* begin() {return m_ptr;}
    T* end()   {return m_ptr+m_size;}
    std::size_t size() const {return m_size;}
    std::vector<T> copy() { std::vector<T> vec(m_size);
      std::copy(this->begin(), this->end(), vec.begin());
      return vec;
    }
  };

  template <typename T>
  struct ConstArrayView {
    std::size_t m_size = 0;
    const T* m_ptr = nullptr;
    const T& operator[](int i) const {assert(i<m_size); return *(m_ptr+i);}
    const T* begin() const {return m_ptr;}
    const T* end() const  {return m_ptr+m_size;}
    std::size_t size() const {return m_size;}
    std::vector<T> copy() { std::vector<T> vec(m_size);
      std::copy(this->begin(), this->end(), vec.begin());
      return vec;
    }
  };

  template <typename T>
  struct Array2View {
    int m_dim1_size = 0;
    int m_dim2_size = 0;
    T* m_ptr = nullptr;
    ArrayView<T> operator[](int i) {assert(i<m_dim1_size); return {(std::size_t)m_dim2_size,m_ptr+i*m_dim2_size};}
    int dim1Size() const { return m_dim1_size; }
    int dim2Size() const { return m_dim2_size; }
    std::vector<T> copy() { std::vector<T> vec(m_dim1_size+m_dim2_size);
      std::copy(m_ptr, m_ptr+m_dim1_size+m_dim2_size+1, vec.begin());
      return vec;
    }
  };

  template <typename T>
  struct ConstArray2View {
    int m_dim1_size = 0;
    int m_dim2_size = 0;
    T* m_ptr = nullptr;
    ConstArrayView<T> operator[](int i) const {assert(i<m_dim1_size); return {(std::size_t)m_dim2_size,m_ptr+i*m_dim2_size};}
    int dim1Size() const { return m_dim1_size; }
    int dim2Size() const { return m_dim2_size; }
    std::vector<T> copy() { std::vector<T> vec(m_dim1_size+m_dim2_size);
      std::copy(m_ptr, m_ptr+m_dim1_size+m_dim2_size+1, vec.begin());
      return vec;
    }
  };

  static constexpr utils::Int32 NULL_ITEM_LID = -1;

}// end namespace utils

inline
std::ostream& operator<<(std::ostream& oss, const Neo::utils::Real3& real3){
  oss << "{" << real3.x  << ","  << real3.y << "," << real3.z << "}";
  return oss;
}
}

#endif // NEO_UTILS_H
