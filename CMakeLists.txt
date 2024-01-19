project("PolicyBasedDesign")
cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(naive naive.cc)
add_executable(debug_print debug_print.cc)
add_executable(better_debug_print better_debug_print.cc)
add_executable(overflow overflow.cc)
add_executable(template_alias template_alias.cc)
add_executable(export_n_rebind export_n_rebind.cc)

