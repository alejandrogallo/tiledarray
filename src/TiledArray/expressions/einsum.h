#ifndef TILEDARRAY_EINSUM_H__INCLUDED
#define TILEDARRAY_EINSUM_H__INCLUDED

#include "TiledArray/expressions/fwd.h"
#include "TiledArray/util/index.h"
#include "TiledArray/conversions/vector_of_arrays.h"

namespace TiledArray::expressions {

template <typename R, typename F>
void cartesian_foreach(const std::vector<R> &rs, F f) {
  using It = decltype(std::begin(rs[0]));
  std::vector<It> its, end;
  for (const auto &r : rs) {
    its.push_back(std::begin(r));
    end.push_back(std::end(r));
  }
  while (its.front() != end.front()) {
    std::vector<typename R::value_type> r;
    for (auto& it : its) {
      r.push_back(*it);
    }
    f(r);
    size_t i = its.size();
    while (i > 0) {
      --i;
      ++its[i];
      if (i == 0) break;
      if (its[i] != end[i]) break;
      its[i] = std::begin(rs[i]);
    }
  }
}

template<typename Array>
auto einsum(TsrExpr<Array> A, TsrExpr<Array> B) {
  auto a = idx(A);
  auto b = idx(B);
  Array R;
  R(a^b) = A*B;
  return R;
}

template<typename Array>
auto einsum(TsrExpr<Array> A, TsrExpr<Array> B, const Index &r) {
  auto a = idx(A);
  auto b = idx(B);
  Array R;

  auto h = a & b & r;

  if (!h || (a == b && b == r)) {
    R(r) = A*B;
    return R;
  }

  auto& world = get_default_world();
  auto& local = world; // *get_local_world();

  std::vector< std::vector<size_t> > h_tiles;

  auto h_tile_task = [](std::vector<size_t> idx) {
  };

  cartesian_foreach(h_tiles, h_tile_task);

  return R;

}

}

#endif /* TILEDARRAY_EINSUM_H__INCLUDED */
