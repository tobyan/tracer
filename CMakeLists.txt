project (tracer)

cmake_minimum_required (VERSION 3.0)

set (CMAKE_C_COMPILER clang)
set (CMAKE_CXX_COMPILER clang++)

add_subdirectory(schema)
include_directories (${PROTO_META_BASE_DIR})
add_subdirectory(ptrace)
add_dependencies(tracer schema)
