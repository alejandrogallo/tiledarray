// Samuel R. Powell, 2021
#include "TiledArray/expressions/fwd.h"

#include <TiledArray/error.h>
#include <TiledArray/util/vector.h>

#include <string>
#include <vector>

namespace TiledArray::index {

template<typename T>
using small_vector = container::svector<T>;

small_vector<std::string> tokenize(const std::string &s);

small_vector<std::string> validate(const std::vector<std::string> &v);

std::string join(const small_vector<std::string> &v);

template<typename T, typename U>
using enable_if_string = std::enable_if_t< std::is_same_v<T,std::string>, U>;

/// an n-index, with n a runtime parameter
template<typename T>
class Index {
public:
  using container_type = small_vector<T>;
  using value_type = typename container_type::value_type;

  Index() = default;
  Index(container_type &&s) : data_(std::move(s)) {}

  template<typename S, typename U = void>
  Index(const S &s) : data_(s.begin(), s.end()) {}

  template<typename U = void>
  Index(const std::string &s) : Index(index::tokenize(s)) {}

  template<typename U = void>
  Index(const char *s) : Index(std::string(s)) {}

  operator std::string() const { return index::join(data_); }

  operator bool() const { return !data_.empty(); }

  bool operator==(const Index &other) const {
    return (this->data_ == other.data_);
  }

  size_t size() const { return data_.size(); }

  auto begin() const { return data_.begin(); }
  auto end() const { return data_.end(); }

  auto operator[](size_t idx) const { return data_.at(idx); }

 private:
  container_type data_;
};

/// (stable) intersect of 2 Index objects
/// @param[in] a an Index object
/// @param[in] b an Index object
/// @pre a and b do not have duplicates
template<typename T>
Index<T> operator&(const Index<T> &a, const Index<T> &b);

/// union of 2 Index objects
/// @param[in] a an Index object
/// @param[in] b an Index object
/// @pre a and b do not have duplicates
template<typename T>
Index<T> operator|(const Index<T> &a, const Index<T> &b);

/// concatenation of 2 Index objects
/// @param[in] a an Index object
/// @param[in] b an Index object
/// @note unline operator| @p a and @p b can have have duplicates
template<typename T>
Index<T> operator+(const Index<T> &a, const Index<T> &b);

/// "difference" of  2 Index objects, i.e. elements of a that are not in b
/// @param[in] a an Index object
/// @param[in] b an Index object
/// @note unline operator& @p a and @p b can have have duplicates
template<typename T>
Index<T> operator-(const Index<T> &a, const Index<T> &b);

/// elements that are exclusively in @p a or @p b
/// @param[in] a an Index object
/// @param[in] b an Index object
/// @pre a and b do not have duplicates
template<typename T>
inline Index<T> operator^(const Index<T> &a, const Index<T> &b) {
  return (a | b) - (a & b);
}

template<typename T>
size_t rank(const Index<T> &idx) { return idx.size(); }

/// permutation taking one Index to another
/// TODO replace by Permutation
struct IndexShuffle {
  IndexShuffle() = default;

  template<typename T>
  IndexShuffle(const Index<T> &s, const Index<T> &p) {
    assert(!(s ^ p));
    for (size_t i = 0; i != p.size(); ++i) {
      for (size_t j = 0; j != s.size(); ++j) {
        if (s[j] == p[i]) {
          mapping_.push_back(j);
          break;
        }
      }
    }
  }

  size_t operator[](size_t idx) const { return mapping_[idx]; }

  IndexShuffle operator~() const;

  template <typename S>
  S operator()(const S &s) const {
    S t = s;
    for (size_t i = 0; i < mapping_.size(); ++i) {
      t[i] = s[mapping_.at(i)];
    }
    return t;
  }

 private:
  small_vector<size_t> mapping_;
};

/// @brief Index-annotated collection of objects
/// @tparam Value
/// This is a map using Index::element_type as key
template<typename K, typename V>
struct IndexMap {
  using key_type = K;
  using value_type = V;

  template <typename Seq>
  IndexMap(const Index<K> &keys, Seq &&seq) {
    data_.reserve(keys.size());
    constexpr bool seq_is_movable = !std::is_reference_v<Seq>;
    auto it = std::begin(seq);
    auto end = std::end(seq);
    for (auto &&key : keys) {
      assert(it != end);
      if constexpr (seq_is_movable)
        data_.emplace_back(std::make_pair(key, std::move(*it)));
      else
        data_.emplace_back(std::make_pair(key, *it));
      ++it;
    }
    assert(it == end);
  }

  /// @return const iterator pointing to the element associated with @p key
  auto find(const key_type &key) const {
    return std::find_if(data_.begin(), data_.end(),
                        [&key](const auto &v) { return key == v.first; });
  }

  /// @return reference to the element associated with @p key
  /// @throw TA::Exception if @p key is not in this map
  const value_type &at(const key_type &key) const {
    auto it = find(key);
    if (it != data_.end())
      return *it;
    else
      throw TiledArray::Exception("IndexMap::at(key): key not found");
  }

  /// @param[in] idx an Index object
  /// @return directly-addressable sequence of elements corresponding to the
  /// keys in @p idx
  auto operator[](const Index<K> &idx) const {
    container::svector<value_type> result;
    result.reserve(idx.size());
    for (auto &&key : idx) {
      result.emplace_back(this->at(key));
    }
    return result;
  }

 private:
  small_vector< std::pair<key_type, value_type> > data_;

};

/// TODO to be filled by Sam
template <typename K, typename T>
IndexMap<K,T> operator|(const IndexMap<K,T> &a, const IndexMap<K,T> &b);


}  // namespace TiledArray::index

namespace TiledArray {

using Index = TiledArray::index::Index<std::string>;
using TiledArray::index::IndexMap;
using TiledArray::index::IndexShuffle;

/// converts the annotation of an expression to an Index
template <typename A, bool Alias>
Index idx(const expressions::TsrExpr<A, Alias> &e) {
  return e.annotation();
}

}  // namespace TiledArray
