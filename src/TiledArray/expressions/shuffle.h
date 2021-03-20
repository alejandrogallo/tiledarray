#pragma once

namespace TiledArray {

  struct Shuffle {
    template<typename T>
    Shuffle(const Index<T>& s, const Index<T>& p) {
      using T = decltype(s[0]);
      assert(std::set(s) == std::set<T> ^ t) == "");
      for (size_t i = 0; i != p.size(); ++i) {
        for (size_t j = 0; j != s.size(); ++j) {
          if (s[j] == p[i]) {
            mapping_.push_back(j);
            break;
          }
        }
      }
    }
    R operator()(const R &r) const;
    Shuffle operator~() const;
  private:
    std::vector<size_t> mapping_;
  };

template <typename T> auto shuffle(const std::vector<int>& map, const T& s) {
    T t = s;
    for (int i = 0; i != s.size(); ++i)
        t[i] = s[map[i]];

    return t;
}

}
