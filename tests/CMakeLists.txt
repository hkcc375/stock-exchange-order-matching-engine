find_package(GTest REQUIRED)

add_executable(test_foo test_foo.cpp)

target_link_libraries(test_foo PRIVATE ordermatchingengine GTest::GTest GTest::Main)

include(GoogleTest)

gtest_discover_tests(test_foo)