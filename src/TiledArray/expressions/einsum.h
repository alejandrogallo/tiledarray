#ifndef TILEDARRAY_EINSUM_H__INCLUDED
#define TILEDARRAY_EINSUM_H__INCLUDED

#include "TiledArray/conversions/vector_of_arrays.h"
#include "TiledArray/expressions/fwd.h"
#include "TiledArray/util/index.h"
#include "TiledArray/util/range.h"
#include "TiledArray/util/tile.h"

namespace TiledArray::expressions {

struct Range {
  using value_type = int64_t;
  using iterator = boost::counting_iterator<value_type>;
  Range(value_type, value_type);
  template <typename T>
  Range(std::pair<T, T> r) : Range(r.first, r.second) {}
  iterator begin() const;
  iterator end() const;
};

/// einsum function without result indices assumes every index present
/// in both @p A and @p B is contracted, or, if there are no free indices,
/// pure Hadamard product is performed.
/// @param[in] A first argument to the product
/// @param[in] B second argument to the product
/// @warning just as in the plain expression code, reductions are a special
/// case; use Expr::reduce()
template <typename Array>
auto einsum(TsrExpr<Array> A, TsrExpr<Array> B) {
  auto a = idx(A);
  auto b = idx(B);
  Array R;
  R(a ^ b) = A * B;
  return R;
}

/// einsum function with result indices explicitly specified
/// @param[in] A first argument to the product
/// @param[in] B second argument to the product
/// @param[in] r result indices
/// @warning just as in the plain expression code, reductions are a special
/// case; use Expr::reduce()
template<typename Array>
auto einsum(TsrExpr<Array> A, TsrExpr<Array> B, const Index &c) {
  auto a = idx(A);
  auto b = idx(B);

  // these are "Hadamard" (fused) indices
  auto h = a & b & c;

  // no Hadamard indices => standard contraction (or even outer product)
  // same a, b, and c => pure Hadamard
  if (!h || (a == b && b == c)) {
    Array C;
    C(c) = A*B;
    return C;
  }

  auto e = (a ^ b);
  // contacted indices
  auto i = (a & b) - h;

  using RangeMap = IndexMap<std::string,TiledRange1>;
  auto range_map = (
    RangeMap(a, A.array().trange()) |
    RangeMap(b, B.array().trange())
  );

  struct Term {
    Array array;
    IndexShuffle shuffle;
    std::vector<Range> rs;
    //DistArray< BatchTile<T> > local;
  };

  Term terms[2] = {
    { A.array(), IndexShuffle(a, h + (a & e) + i) },
    { B.array(), IndexShuffle(b, h + (b & e) + i) }
  };

  Term C = {
    Array(),//range_map[c]),
    IndexShuffle(h+e, c)
  };

  std::vector<Range> hr;
  for (size_t i = 0; i < h.size(); ++i) {
    size_t k = C.shuffle[i];
    auto tr = C.array.trange().at(k);
    hr.push_back(tr.tiles_range());
  }

  auto h_tile_task = [&terms,&C](auto h) {
    for (auto &term : terms) {
      auto pack = [&](auto ei) {
        h + ei;
        // tile::copy(
        //   term.array.tile(term.shuffle.reverse(h + ei)),
        //   term.local.tile(ei).data(),
        //   term.shuffle
        // );
      };
      range::cartesian_foreach(term.rs, pack);
    }
    //C.local("i,j") = terms[0].local("k,i") * terms[1].local("k,j");
    //unpack(C.local, C.array, tr, C.shuffle);
  };

  // iterates over tiles of hadamard indices
  range::cartesian_foreach(hr, h_tile_task);

  return C.array;

}

}  // namespace TiledArray::expressions

#endif /* TILEDARRAY_EINSUM_H__INCLUDED */
