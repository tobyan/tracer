project (tracer)

cmake_minimum_required (VERSION 3.0)

add_subdirectory(schema)
include_directories (${PROTO_META_BASE_DIR})
add_subdirectory(ptrace)


# add_executable(tracer ptrace schema)

# target_link_libraries (tracer ptrace schema)

