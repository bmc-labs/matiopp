# Copyright 2019 bmc::labs GmbH.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# i like big Boost and i cannot lie
find_package (Boost COMPONENTS regex REQUIRED)

set (TARGET matiopp-test)

add_executable (${TARGET} main.cc ${TARGET}.cc)

target_include_directories (${TARGET} PUBLIC
  ${matiopp_SOURCE_DIR}/include
  ${Boost_INCLUDE_DIRS}
)

target_link_libraries (${TARGET} matio gcov ${Boost_LIBRARIES})

# compile options for test coverage
target_compile_options (${TARGET} PRIVATE
  -fprofile-arcs
  -ftest-coverage
  -fno-inline
  -fno-inline-small-functions
  -fno-default-inline
  -fPIC
  -O0
  --coverage
)

# required for CTest -- CURRENTLY NOT WORKING
# add_test (NAME ${TARGET} COMMAND ${TARGET})

# copy test file to binary directory
file (GLOB TESTDATA TESTDATA-*.mat)
file (COPY ${TESTDATA} DESTINATION ${CMAKE_BINARY_DIR})

