# Order Matching Engine for a Stock Exchange
A Simple C++ Implementation of an Order Matching Engine.
It demonstrates the core functionalities of an exchange's matching system - handling orders, maintaing an order book and generating trades.

---

## Setup :

### Dependencies :
- Please ensure you have a C++ Compiler (gcc / clang) available. Here, I use clang version 19.1.1
- Please ensure you have installed GTest framework (for running Unit Tests). Here, I use GoogleTest 1.17.0

### Steps to run :
- Remove any available old builds by `rm -rf build/` in the root project directory
- Create a build folder by `mkdir build && cd build/`. This folder will contain the built artifacts
- Configure the project for building artifacts by `cmake .. -DCMAKE_CXX_COMPILER=clang++`. Here, I explicitly specify that I want to use clang++. This reads all the CMakeLists.txt files and prepares a recipe for build
- Build the project by `cmake --build .`. This generates an application binary (for execution) and a test binary (for running tests)
- Execute the application by `./run_app`
- Test the application by `./test_app`
