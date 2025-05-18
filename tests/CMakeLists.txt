enable_testing()

add_executable(order_book_tests
  order_tests.cpp
)
target_link_libraries(order_book_tests PRIVATE order_book)


add_test(NAME order_struct   COMMAND order_book_tests order_struct)
add_test(NAME exact_fill     COMMAND order_book_tests exact_fill)
add_test(NAME partial_fill   COMMAND order_book_tests partial_fill)
add_test(NAME market_order   COMMAND order_book_tests market_order)
add_test(NAME cancel         COMMAND order_book_tests cancel)
