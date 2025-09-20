add_library(ordermatchingengine foo.cpp)

target_include_directories(ordermatchingengine PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Executable
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE ordermatchingengine)