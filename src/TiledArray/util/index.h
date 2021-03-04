// Samuel R. Powell, 2021
#include "TiledArray/expressions/fwd.h"

#include <string>
#include <vector>

namespace TiledArray::index {

  std::vector<std::string> tokenize(const std::string &s);

  std::vector<std::string> validite(const std::vector<std::string> &v);

  std::string join(const std::vector<std::string> &v);

  class Index {
  public:

    Index() = default;
    Index(const std::vector<std::string> &s) : data_(index::validite(s)) {}
    Index(const std::string &s) : Index(index::tokenize(s)) {}
    Index(const char *s) : Index(std::string(s)) {}

    operator std::string() const {
      return index::join(data_);
    }

    operator bool() const {
      return !data_.empty();
    }

    bool operator==(const Index& other) const {
      return (this->data_ == other.data_);
    }

    size_t size() const { return data_.size(); }

    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }

  private:
    std::vector<std::string> data_;
  };

  Index operator & (const Index &a, const Index &b);

  Index operator | (const Index &a, const Index &b);

  Index operator + (const Index &a, const Index &b);

  Index operator - (const Index &a, const Index &b);

  inline Index operator ^ (const Index &a, const Index &b) {
    return (a | b) - (a & b);
  }

  inline size_t rank(const Index &idx) {
    return idx.size();
  }

}


namespace TiledArray {

  using TiledArray::index::Index;

  template<typename A, bool Alias>
  Index idx(const expressions::TsrExpr<A,Alias> &e) {
    return e.annotation();
  }

}
