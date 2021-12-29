#pragma once

#include <cassert>

namespace sn {

// Container must have more than one element
template <typename FIter, typename Fn>
void adjacent_iter(FIter first, FIter last, Fn fn) {
  auto trailer = first;
  ++first;
  for (; first != last; ++first, ++trailer) {
    fn(*first, *trailer);
  }
}

}  // namespace sn