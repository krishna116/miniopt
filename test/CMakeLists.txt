# Test C language.
add_executable(test test.c ${CMAKE_SOURCE_DIR}/src/miniopt.c)
target_include_directories(test PUBLIC ${CMAKE_SOURCE_DIR}/src)

# Test C++ language.
add_executable(test2 test2.cpp ${CMAKE_SOURCE_DIR}/src/miniopt.c)
target_include_directories(test2 PUBLIC ${CMAKE_SOURCE_DIR}/src)

# Test code generator generated code.
add_executable(test3 test3.c ${CMAKE_SOURCE_DIR}/src/miniopt.c)
target_include_directories(test3 PUBLIC ${CMAKE_SOURCE_DIR}/src)