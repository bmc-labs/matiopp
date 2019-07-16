# Copyright 2019 Florian Eich <florian@bmc-labs.com>
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

# matio, HDF5 & Boost
find_library (MATIO_FOUND matio)
if (NOT MATIO_FOUND)
  message (FATAL_ERROR "matio library not found, aborting")
endif ()

find_package (HDF5 COMPONENTS HL REQUIRED)
find_package (Boost COMPONENTS regex REQUIRED)

# define target (INTERFACE bc header only)
add_library (matiopp INTERFACE)
target_include_directories (matiopp INTERFACE
  $${matiopp_SOURCE_DIR}/include
  ${Boost_INCLUDE_DIRS}
)

target_link_libraries (matiopp INTERFACE
  matio
  ${HDF5_LIBRARIES}
  ${Boost_LIBRARIES}
)

