cmake_minimum_required(VERSION 3.16)

project(cpp_cache LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===== Catch2 =====
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)

FetchContent_MakeAvailable(Catch2)

# ===== cache headers (header-only) =====
add_library(cache_lib INTERFACE)
target_include_directories(cache_lib INTERFACE
  ${CMAKE_SOURCE_DIR}/include
)

# ===== Common warnings helper =====
function(set_warnings target_name)
  if(MSVC)
    target_compile_options(${target_name} PRIVATE /W4)
  else()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

# ===== (1) Keep your big comparison test =====
add_executable(cache_tests
  tests/testAllCachePolicy.cpp
)

target_link_libraries(cache_tests PRIVATE
  cache_lib
  Catch2::Catch2WithMain
)

set_warnings(cache_tests)


file(GLOB_RECURSE UNIT_TEST_FILES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/tests/*.test.cpp"
)

# Aggregate target: build all unit tests
add_custom_target(tests)

foreach(test_file IN LISTS UNIT_TEST_FILES)
  # file name without last extension: lru.test.cpp -> lru.test
  get_filename_component(test_name_we "${test_file}" NAME_WE)

  # make a safe target name: lru.test -> lru_test
  set(test_target "${test_name_we}")
  string(REPLACE "." "_" test_target "${test_target}")
  string(REPLACE "-" "_" test_target "${test_target}")

  add_executable(${test_target} ${test_file})

  target_link_libraries(${test_target} PRIVATE
    cache_lib
    Catch2::Catch2WithMain
  )

  set_warnings(${test_target})

  add_dependencies(tests ${test_target})
endforeach()
