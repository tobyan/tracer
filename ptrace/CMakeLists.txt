# project (tracer)
# cmake_minimum_required(VERSION 3.0)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set (TRACER_SRC
  main.cc
  tracer.cc
  recorder.cc
)

add_executable(tracer ${TRACER_SRC} ${PROTO_GENERATED_SOURCES})

find_package(Boost REQUIRED COMPONENTS system filesystem program_options)
find_package (Protobuf REQUIRED)

if (Boost_FOUND)
  target_link_libraries (tracer ${Boost_LIBRARIES})
endif ()

if (PROTOBUF_FOUND)
  target_link_libraries (tracer ${PROTOBUF_LIBRARIES})
endif ()

target_link_libraries (tracer schema)
