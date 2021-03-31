
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
void apply(F&& f, size_t rank, const size_t* dims,
           T* p0, const S0* s0, U* p1, const S1* s1)
{
  if (rank == 0) return;
  if (rank == 1) {
    apply1(f, dims[0], p0, s0[0], p1, s1[0]);
    return;
  }
  if (rank == 2) {
    apply2(f, dims[0], dims[1],
           p0, std::pair{s0[0], s0[1]},
           p1, std::pair{s1[0], s1[1]}
    );
    return;
  }
  for (size_t i = 0; i != dims[0]; ++i) {
    apply(f, rank - 1, dims+1, p0, s0 + 1, p1, s1 + 1);
    p0 += s0[0];
    p1 += s1[0];
  }
}

template<typename T>
using small_vector = container::svector<T>;

template<typename Dims, typename In, typename Out, typename Order>
void copy(const Dims &dims, const In *in, Out *out, const Order &p) {
  size_t rank = dims.size();
  small_vector<size_t> s_in(rank), s_out(rank);
  for (size_t i = 0, n_in = 1, n_out = 1; i < rank; ++i) {
    size_t k = p[i];
    std::rbegin(s_in)[i] = n_in;
    std::rbegin(s_out)[k] = n_out;
    n_in *= std::rbegin(dims)[i];
    n_out *= std::rbegin(dims)[k];
  }
  apply(
    [](const auto &a, auto &b) { b = a; },
    rank, dims.data(),
    in, s_in.data(), out, s_out.data()
  );
}

}  // namespace TiledArray::expressions
