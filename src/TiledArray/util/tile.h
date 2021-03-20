
namespace TiledArray::tile {

template <typename F, typename T, typename Stride1, typename U,
          typename Stride2>
void apply1(F&& f, size_t N, T* ptr1, const Stride1 s1, U* ptr2,
            const Stride2 s2) {
  for (size_t i = 0; i != N; ++i) {
    f(*ptr1, *ptr2);
    ptr2 += s2;
    ptr1 += s1;
  }
}

template <typename F, typename T, typename... Stride1, typename U,
          typename... Stride2>
void apply2(F&& f, size_t M, size_t N, T* ptr1, const std::pair<Stride1...> s1,
            U* ptr2, const std::pair<Stride2...> s2) {
  for (size_t i = 0; i != M; ++i) {
    apply1(f, N, ptr1, s1.second, ptr2, s2.second);
    ptr1 += s1.first;
    ptr2 += s2.first;
  }
}

template <typename F, typename T, typename S0, typename U, typename S1>
void apply(F&& f, size_t rank, const size_t* dims, T* p0, const S0* s0, U* p1,
           const S1* s1) {
  if (rank == 0) return;
  if (rank == 1) {
    apply1(f, dims[0], p0, s0[0], p1, s1[0]);
    return;
  } else if (rank == 2) {
    apply2(f, dims[0], dims[1], p0, std::pair{s0[0], s0[1]}, p1,
           std::pair{s1[0], s1[1]});
    return;
  }
  for (size_t i = 0; i != dims[0]; ++i) {
    apply(f, rank - 1, p0, s0 + 1, p1, s1 + 1);
    p0 += s0[0];
    p1 += s1[0];
  }
}

template <typename Array, typename T>
void pack(const Array& A, std::vector<Range> tiles, Tensor<T>& buffer,
          IndexShuffle shuffle) {}

template <typename Array, typename T>
void unpack(Tensor<T>& buffer, const Array& A, std::vector<Range> tiles,
            IndexShuffle shuffle) {}

}  // namespace TiledArray::expressions
