#ifndef APPLY_ITERABLE_HPP__
#define APPLY_ITERABLE_HPP__

#include <iostream>
#include <string>
#include <cstdio>
#include <utility>
#include <tuple>

namespace app {
  template <class T, class Enable = void>
  struct is_random_access_container : std::false_type
  {};
  template <class T>
  struct is_random_access_container<T,
      std::enable_if_t<
	std::is_convertible<
	  typename std::iterator_traits<decltype(std::begin(std::declval<T>()))>::iterator_category,
	  std::random_access_iterator_tag>::value>>
    : std::true_type
  {};
  template <class T>
  constexpr bool is_random_access_container_v = is_random_access_container<T>::value;
  
  template <unsigned size, class Func, class RandomAccessible,
	    class Tuple, std::size_t... I1, std::size_t... I2
	    >
  auto apply_n_impl(Func func, Tuple &&args, std::index_sequence<I1...>,
		    RandomAccessible && elements, std::index_sequence<I2...>)
    -> typename std::enable_if<std::is_rvalue_reference<decltype(elements)>::value,
			       decltype(func(std::get<I1>(std::forward<Tuple>(args))..., std::move(elements[I2])...))>::type
  {
    return func(std::get<I1>(std::forward<Tuple>(args))..., std::move(elements[I2])...);
  }
  template <unsigned size, class Func, class RandomAccessible,
	    class Tuple, std::size_t... I1, std::size_t... I2
	    >
  auto apply_n_impl(Func func, Tuple &&args, std::index_sequence<I1...>,
		    RandomAccessible && elements, std::index_sequence<I2...>)
    -> typename std::enable_if<!std::is_rvalue_reference<decltype(elements)>::value,
			       decltype(func(std::get<I1>(std::forward<Tuple>(args))..., std::move(elements[I2])...))>::type
  {
    return func(std::get<I1>(std::forward<Tuple>(args))..., elements[I2]...);
  }
  template <unsigned size, class Func, class RandomAccessible, class ...Args
	    , class Enable = std::enable_if_t<
		is_random_access_container_v<RandomAccessible>>
	    >
  auto apply_n(Func func, std::tuple<Args...> && args,
	       RandomAccessible && elements)
  {
    if (size != std::end(elements) - std::begin(elements))
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
	     RandomAccessible && elements, typename std::enable_if<(min >= max)>::type * = nullptr)
  {
    if (std::end(elements) - std::begin(elements) == min) {
      return apply_n<min>(func, std::forward<std::tuple<Args...>>(args),
			  std::forward<RandomAccessible>(elements));
    }
    
    throw std::runtime_error("Too many elements");
  }
  template <unsigned min=0, unsigned max=100,
	    class Func, class RandomAccessible, class ...Args,
	    class Type = decltype(std::declval<RandomAccessible>()[1]),
            class = typename std::enable_if<(min < max)>::type
	    >
  auto apply(Func func, std::tuple<Args...> && args,
	     RandomAccessible && elements)
  {
    if (std::end(elements) - std::begin(elements) == min) {
      return apply_n<min>(func, std::forward<std::tuple<Args...>>(args),
			  std::forward<RandomAccessible>(elements));
    }
    return apply<min+1, max>(func, std::forward<std::tuple<Args...>>(args),
			     std::forward<RandomAccessible>(elements));
  }
}
#endif // APPLY_ITERABLE_HPP__
