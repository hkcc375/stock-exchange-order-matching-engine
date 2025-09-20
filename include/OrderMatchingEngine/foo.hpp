#pragma once
#include <string>
#include <iostream>

namespace OrderMatchingEngine {

class Foo {
public:
    Foo(int v);
    int getValue() const;
    std::string greet() const {return "Greet";}

private:
    int value;
};

}
