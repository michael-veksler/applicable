#include <apply_iterable.hpp>

int main()
{
    std::vector<const char*> elements{"hello", "nice", "world"};
    app::apply(std::printf, std::tuple{"%s %s %s"}, elements);
}
