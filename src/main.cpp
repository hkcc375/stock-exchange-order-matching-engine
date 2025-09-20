#include "../include/OrderMatchingEngine/foo.hpp"
#include <iostream>
using OrderMatchingEngine::Foo;

int main()
{
    Foo f(42);
    f.greet();
    std::cout << "App finished" << std::endl;
    return 0;
}
