find_package(GTest REQUIRED)

add_executable(test_stock_market test_stock_market.cpp)
target_link_libraries(test_stock_market PRIVATE ordermatchingengine GTest::GTest GTest::Main)

include(GoogleTest)
gtest_discover_tests(test_stock_market)
