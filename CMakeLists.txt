cmake_minimum_required(VERSION 3.15.0)
project(maia VERSION 0.1.0)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall")

# find_package(Boost REQUIRED)
# if(Boost_FOUND)
#   message(STATUS "Boost -- found")
#   message(STATUS "Boost version: ${Boost_VERSION_STRING}")
# endif()

include(spdlog)
include(sfml)

# include_directories("lib/imgui")
# include_directories("src")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wnon-virtual-dtor -Wno-missing-field-initializers")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++20-designator")
endif()

add_executable(
  maia
  "src/main.cpp"
)
target_link_libraries(
  maia PUBLIC
  sfml-window
  sfml-system
  spdlog::spdlog
)
target_compile_features(maia PUBLIC cxx_std_20)