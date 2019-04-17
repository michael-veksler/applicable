#ifndef APPLY_ITERABLE_HPP__
#define APPLY_ITERABLE_HPP__

#include <iostream>
#include <string>
#include <cstdio>
#include <utility>
#include <tuple>

namespace app {
  template <unsigned size, class Func, class RandomAccessible,
	    class Tuple, std::size_t... I1, std::size_t... I2
	    >
  auto apply_n_impl(Func func, Tuple &&args, std::index_sequence<I1...>,
		    RandomAccessible && elements, std::index_sequence<I2...>)
  {
    if constexpr (std::is_rvalue_reference_v<decltype(elements)>)
      return func(std::get<I1>(std::forward<Tuple>(args))..., std::move(elements[I2])...);
    else
      return func(std::get<I1>(std::forward<Tuple>(args))..., elements[I2]...);
  }
  template <unsigned size, class Func, class RandomAccessible, class ...Args,
	    class Type = decltype(std::declval<RandomAccessible>()[1])>
  auto apply_n(Func func, std::tuple<Args...> && args,
	       RandomAccessible && elements)
  {
    if (size != std::size(elements))
      throw std::runtime_error("wrong number of elements");
    return apply_n_impl<size>(func,
			      std::forward<std::tuple<Args...>>(args),
			      std::make_index_sequence<sizeof...(Args)>(),
			      std::forward<RandomAccessible>(elements),
			      std::make_index_sequence<size>());
  }

  template <unsigned min=0, unsigned max=100,
	    class Func, class RandomAccessible, class ...Args,
	    class Type = decltype(std::declval<RandomAccessible>()[1])
	    >
  auto apply(Func func, std::tuple<Args...> && args,
	     RandomAccessible && elements)
  {
    if (std::size(elements) == min) {
      return apply_n<min>(func, std::forward<std::tuple<Args...>>(args),
			  std::forward<RandomAccessible>(elements));
    }
    if constexpr(min < max)
      return apply<min+1, max>(func, std::forward<std::tuple<Args...>>(args),
			       std::forward<RandomAccessible>(elements));
    else
      throw std::runtime_error("Too many elements");
  }
}
#endif // APPLY_ITERABLE_HPP__
