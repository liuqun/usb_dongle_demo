cmake_minimum_required(VERSION 3.5)

project(demo2 LANGUAGES C)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/D_UNICODE>")
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/DUNICODE>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/D_UNICODE>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/DUNICODE>")

add_executable(demo2 main.c)
target_link_libraries(demo2 "$<$<C_COMPILER_ID:GNU>:dl>")
