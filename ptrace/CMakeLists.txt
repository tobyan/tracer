# project (tracer)
# cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.



set (TRACER_SRC
  main.cc
  tracer.cc
  recorder.cc
)

add_executable(tracer ${TRACER_SRC} ${PROTO_GENERATED_SOURCES})

find_package(Boost REQUIRED COMPONENTS system filesystem program_options)
find_package (Protobuf REQUIRED)
find_library(CAPSTONE_LIBRARY NAMES capstone)
# find_package (Capstone REQUIRED)
#target_include_directories (${CAPSTONE_INCLUDE_DIRS})
target_link_libraries (tracer ${CAPSTONE_LIBRARY})

if (Boost_FOUND)
  target_link_libraries (tracer ${Boost_LIBRARIES})
endif ()

if (PROTOBUF_FOUND)
  target_link_libraries (tracer ${PROTOBUF_LIBRARIES})
endif ()

target_link_libraries (tracer schema)
