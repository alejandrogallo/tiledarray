namespace TiledArray::range {

struct Range {
  using value_type = int64_t;
  using iterator = boost::counting_iterator<value_type>;
  Range(value_type begin, value_type end) :
    begin_(begin),
    end_(end)
  {}
  template <typename T>
  Range(std::pair<T, T> r)
      : Range(r.first, r.second + 1)
      {}
  auto begin() const {
      return iterator(begin_);
  }
  auto end() const {
      return iterator(end_);
  }

 protected:
  const value_type begin_, end_;

};

RangeProduct {
    RangeProduct() = default;
    RangeProduct(std::initializer_list<Range> rs)
        : product_(rs.begin(), rs.end()) {}

    RangeProduct& operator *= (Range a) {
      return *this
    }

   protected:
    std::vector<Range> product_;

};

RangeProduct operator * (Range a, Range b){
    return RangeProduct({a, b});
};

RangeProduct operator * (const RangeProduct& a, Range b) {
    return RangeProduct(a) *= b;
};

}

template<typename R, typename F>
void cartesian_foreach(const std::vector<R>& rs, F f) {
  using It = decltype(std::begin(rs[0]));
  using T = typename R::value_type;
  small_vector<It> its, ends;
  for (const auto& r : rs) {
    its.push_back(std::begin(r));
    ends.push_back(std::end(r));
  }
  while (its.front() != ends.front()) {
    small_vector<T> s;
    s.reserve(its.size());
    for (auto& it : its) {
      s.push_back(*it);
    }
    f(s);
    size_t i = its.size();
    while (i > 0) {
      --i;
      ++its[i];
      if (i == 0) break;
      if (its[i] != ends[i]) break;
      its[i] = std::begin(rs[i]);
    }
  }
}

}  // namespace TiledArray::expressions
