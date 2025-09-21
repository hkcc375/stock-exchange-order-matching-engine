#include "../include/OrderMatchingEngine/foo.hpp"

namespace OrderMatchingEngine {

Foo::Foo(int v)
    : value(v)
{
}

int Foo::getValue() const
{
    return value;
}

}
