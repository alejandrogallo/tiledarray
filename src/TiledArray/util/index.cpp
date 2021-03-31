// Samuel R. Powell, 2021
#include "TiledArray/util/index.h"
#include "TiledArray/util/annotation.h"

#include <boost/algorithm/string.hpp>
#include <set>
#include <string>
#include <vector>

namespace TiledArray::index {

// std::vector<std::string> validate(const std::vector<std::string> &v) {
//   return v;
// }

// std::vector<std::string> tokenize(const std::string &s) {
//   // std::vector<std::string> r;
//   // boost::split(r, s, boost::is_any_of(", \t"));
//   // return r;
//   auto r = detail::tokenize_index(s, ',');
//   if (r == std::vector<std::string>{""}) return {};
//   return r;
// }

// std::string join(const std::vector<std::string> &v) {
//   return boost::join(v, ",");
// }

// Index operator&(const Index &a, const Index &b) {
//   std::vector<std::string> r;
//   std::set<std::string> bset(b.begin(), b.end());
//   for (const auto &s : a) {
//     if (!bset.count(s)) continue;
//     r.push_back(s);
//   }
//   return Index(r);
// }

// Index operator|(const Index &a, const Index &b) {
//   std::vector<std::string> r;
//   r.assign(a.begin(), a.end());
//   std::set<std::string> aset(a.begin(), a.end());
//   for (const auto &s : b) {
//     if (aset.count(s)) continue;
//     r.push_back(s);
//   }
//   return Index(r);
// }

// Index operator+(const Index &a, const Index &b) {
//   std::vector<std::string> r;
//   r.assign(a.begin(), a.end());
//   r.insert(r.end(), b.begin(), b.end());
//   return Index(r);
// }

// Index operator-(const Index &a, const Index &b) {
//   std::vector<std::string> r;
//   std::set<std::string> bset(b.begin(), b.end());
//   for (const auto &s : a) {
//     if (bset.count(s)) continue;
//     r.push_back(s);
//   }
//   return Index(r);
// }

}  // namespace TiledArray::index
