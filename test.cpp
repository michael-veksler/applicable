#include <deque>
#include <vector>
#include <apply_iterable.hpp>
#include <memory>
#include <array>
#include <cstring>
#include <sstream>
#include <iostream>

void assertEqualStr(const char *a, const char *b, const char * msg = nullptr)
{
  if (strcmp(a, b) == 0)
    return;
  std::string msg_str;
  if (msg) {
    msg_str = msg;
  } else {
    std::ostringstream out;
    out << "'" << a << "' != '" << b << "'";
    msg_str = out.str();
  }
  std::cerr << msg_str << '\n';
  throw std::runtime_error(msg_str);
}

int main()
{
    std::vector<const char*> elements{"hello", "nice", "world"};
    char buf[100];
    app::apply(std::sprintf, std::make_tuple(buf, "%s %s %s"), elements);
    assertEqualStr(buf, "hello nice world");
    
    std::deque<int> int_elements{1,3,0};
    app::apply<1,3>(std::sprintf, std::make_tuple(buf, "%d,%d,%d"), int_elements);
    assertEqualStr(buf, "1,3,0");
    
    const double nums[3] = { 1./9, 1./9, 1./9};
    bool had_exception = false;
    try {
      app::apply_n<2>(std::sprintf, std::make_tuple(buf, "%.1f %.2f %.3f"), nums);
    } catch (std::runtime_error &) {
      had_exception = true;
    }
    if (!had_exception) {
      throw std::runtime_error("Expecting exception");
    }
      
    app::apply_n<3>(std::sprintf, std::make_tuple(buf, "%.1f %.2f %.3f"), nums);
    assertEqualStr(buf, "0.1 0.11 0.111");

    std::array<std::unique_ptr<int>, 2> ptrs{std::make_unique<int>(1),
				    std::make_unique<int>(2)};
    std::vector<std::unique_ptr<int>> target;
    auto move_pointers = [&target](auto && ...ptrs) {
	 int x[] = {
		    (target.emplace_back(std::forward<decltype(ptrs)>(ptrs)),
		     0)...
	 };
    };
    
    app::apply<2,2>(move_pointers, std::tuple<>{}, std::move(ptrs));
    for (auto & x: ptrs) {
      if (x) throw std::runtime_error("should be deleted");
    }
    std::cout << "OK\n";
}
