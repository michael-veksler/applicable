# applicable
C++: Pass elements of a container to a variadic function.

This library allows passing all elements of a container as parameters of a variadic funciton.

It allows to do in  C++ what is achieved in Python by:
```python
def expand(func, tup, elements):
   func(*tup, *elements);
```    
## Interface
The app::expand function can be used by calling 
```c++
app::expand(callable, tuple, random_accessible);
```
or
```c++
app::expand<min, max>(callable, tuple, random_accessible);
``` 
where `min` and `max` are the smallest number of expected elements in `random_accessible` and `max` is the biggest number.

## Usage examples:
```c++
std::vector<const char*> elements{"first", "second", "third"};
app::expand(std::printf, std::tuple{"%s %s %s"}, elements);

std::deque<int> int_elements{1,3,0};
app::expand<1,3>(std::fprintf, std::make_tuple(stdout, "%d,%d,%d\n"), int_elements);

```
and with move only objects:
```c++
std::array<std::unique_ptr<int>, 2> ptrs{std::make_unique<int>(1),
                                         std::make_unique<int>(2)};
std::array<std::unique_ptr<int>, 2> ptrs2{std::make_unique<int>(1),
                                         std::make_unique<int>(2)};
std::vector<std::unique_ptr<int>> target;
auto move_pointers = [&target](auto && ...ptrs) {
     int x[] = {
                (target.emplace_back(std::forward<decltype(ptrs)>(ptrs)),
                 0)...
     };
};

app::expand<2,2>(move_pointers, std::tuple{}, std::move(ptrs)); // OK
app::expand<2,2>(move_pointers, std::tuple{}, ptrs2); // error
```

NOTE: This is based on code I have posted to [stackOverflow](https://stackoverflow.com/a/55453691/4955498)
