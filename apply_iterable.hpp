#ifndef APPLY_ITERABLE_HPP__
#define APPLY_ITERABLE_HPP__

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <utility>
#include <tuple>

namespace app {
  template <unsigned size, class Func, class Type,
	    class Tuple, std::size_t... I1, std::size_t... I2>
  auto apply_n_impl(Func func, Tuple &&args, std::index_sequence<I1...>, const std::vector<Type> & vec, std::index_sequence<I2...>)
  {
    return func(std::get<I1>(std::forward<Tuple>(args))..., vec[I2]...);
  }
  template <unsigned size, class Func, class Type, class ...Args>
  auto apply_n(Func func, std::tuple<Args...> && args, const std::vector<Type> & vec)
  {
    return apply_n_impl<size>(func,
			      std::forward<std::tuple<Args...>>(args), std::make_index_sequence<sizeof...(Args)>(),
			      vec, std::make_index_sequence<size>());
  }

  template <unsigned min=0, unsigned max=100, class Func, class Type, class ...Args>
  auto apply(Func func, std::tuple<Args...> && args, std::vector<Type> & elements)
  {
    if (elements.size() == min) {
      return apply_n<min>(func, std::forward<std::tuple<Args...>>(args), elements);
    }
    if constexpr(min < max)
      return apply<min+1, max>(func, std::forward<std::tuple<Args...>>(args), elements);
    else
      throw std::runtime_error("Too many elements");
  }
}
#endif // APPLY_ITERABLE_HPP__
