enable_testing()

add_executable(order_book_tests
  order_tests.cpp
)

target_link_libraries(order_book_tests PRIVATE order_book)
add_test(NAME basic_tests COMMAND order_book_tests)
