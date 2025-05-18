# Build the core library
add_library(order_book
  book.cpp
  matching_engine.cpp
)

# Tell CMake and the compiler where to find headers
target_include_directories(order_book
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${BOOST_INC}
)

# Build the CLI executable
add_executable(ob_engine main.cpp)
target_link_libraries(ob_engine PRIVATE order_book)
