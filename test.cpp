#include <deque>
#include <vector>
#include <apply_iterable.hpp>
#include <memory>
#include <array>

int main()
{
    std::vector<const char*> elements{"hello", "nice", "world"};
    app::apply(std::printf, std::make_tuple("%s %s %s\n"), elements);
    std::deque<int> int_elements{1,3,0};
    app::apply<1,3>(std::fprintf, std::make_tuple(stdout, "%d,%d,%d\n"), int_elements);
    char buf[100];
    
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
      
    app::apply_n<3>(std::sprintf, std::tuple{buf, "%.1f %.2f %.3f"}, nums);
    puts(buf);

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
}
