# Build the core library
add_library(order_book
  book.cpp
  matching_engine.cpp
)

# Tell the compiler where to find your headers
target_include_directories(order_book PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Build the CLI executable
add_executable(ob_engine main.cpp)
target_link_libraries(ob_engine PRIVATE order_book)
