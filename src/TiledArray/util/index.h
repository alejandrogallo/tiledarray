// Samuel R. Powell, 2021
#include "TiledArray/expressions/fwd.h"

#include <TiledArray/error.h>
#include <TiledArray/util/vector.h>
#include <TiledArray/permutation.h>

#include <string>

namespace TiledArray::index {

template<typename T>
using small_vector = container::svector<T>;

small_vector<std::string> tokenize(const std::string &s);

small_vector<std::string> validate(const small_vector<std::string> &v);

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

  auto find(const T& v) const {
    return std::find(this->begin(), this->end(), v);
  }

  const auto& operator[](size_t idx) const { return data_.at(idx); }

  size_t indexof(const T& v) const {
    for (size_t i = 0; i < this->size(); ++i) {
      if (this[i] == v) return i;
    }
    return -1;
  }

  /// Returns true if argument exists in the Index object, else returns false
  bool contains(const T& v) const {
    return (this->find(v) != this->end());
  }

 private:
  container_type data_;
};

/// (stable) intersect of 2 Index objects
/// @param[in] a an Index object
/// @param[in] b an Index object
/// @pre a and b do not have duplicates
template<typename T>
Index<T> operator&(const Index<T> &a, const Index<T> &b) {
  typename Index<T>::container_type r;
  for (const auto &s : a) {
    if (!b.contains(s)) continue;
    r.push_back(s);
  }
  return Index<T>(r);
}
/// union of 2 Index objects
/// @param[in] a an Index object
/// @param[in] b an Index object
/// @pre a and b do not have duplicates
template<typename T>
Index<T> operator|(const Index<T> &a, const Index<T> &b) {
  typename Index<T>::container_type r;
  r.assign(a.begin(), a.end());
  for (const auto &s : b) {
    if (a.contains(s)) continue;
    r.push_back(s);
  }
  return Index<T>(r);
}

/// concatenation of 2 Index objects
/// @param[in] a an Index object
/// @param[in] b an Index object
/// @note unline operator| @p a and @p b can have have duplicates
template<typename T>
Index<T> operator+(const Index<T> &a, const Index<T> &b) {
  typename Index<T>::container_type r;
  r.assign(a.begin(), a.end());
  r.insert(r.end(), b.begin(), b.end());
  return Index<T>(r);
}

/// "difference" of  2 Index objects, i.e. elements of a that are not in b
/// @param[in] a an Index object
/// @param[in] b an Index object
/// @note unline operator& @p a and @p b can have have duplicates
template<typename T>
Index<T> operator-(const Index<T> &a, const Index<T> &b) {
  typename Index<T>::container_type r;
  for (const auto &s : a) {
    if (b.contains(s)) continue;
    r.push_back(s);
  }
  return Index<T>(r);
}

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

template <typename T>
Index<T> sorted(const Index<T>& a) {
  typename Index<T>::container_type r(a.begin(), a.end());
  std::sort(r.begin(), r.end());
  return Index<T>(r);
}

template<typename T>
Permutation permutation(const Index<T> &s, const Index<T> &p) {
  assert(sorted(s) == sorted(p));
  small_vector<size_t> m;
  m.reserve(p.size());
  for (size_t i = 0; i != p.size(); ++i) {
    m.push_back(s.indexof(p[i]));
  }
  return Permutation(m);
}

template<typename T, typename ... Args>
auto permute(const Permutation &p, const Index<T> &s, Args&& ... args) {
  using R = typename Index<T>::container_type;
  R r(p.size());
  detail::permute_n(p.size(), p.begin(), s.begin(), r.begin(), args...);
  return Index<T>{r};
}

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
  IndexMap(small_vector<std::pair<K, V> > d) {
    this->data_ = d;
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

  auto begin() const { return data_.begin(); }
  auto end() const { return data_.end(); }

 private:
  small_vector< std::pair<key_type, value_type> > data_;

};

/// TODO to be filled by Sam
template <typename K, typename T>
IndexMap<K,T> operator|(const IndexMap<K,T> &a, const IndexMap<K,T> &b) {
  small_vector< std::pair<K, T> > d(a.begin(), a.end());
  for (const auto [k,v] : b) {
    if (a.find(k)) {
      TA_ASSERT(a[k] == b[k]);
    } else {
      d.push_back(std::pair(k, v));
    }
  }
  return IndexMap(d);
}

}  // namespace TiledArray::index

namespace TiledArray {

using Index = TiledArray::index::Index<std::string>;
using TiledArray::index::IndexMap;

/// converts the annotation of an expression to an Index
template <typename A, bool Alias>
Index idx(const expressions::TsrExpr<A, Alias> &e) {
  return e.annotation();
}

}  // namespace TiledArray
