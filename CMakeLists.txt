cmake_minimum_required(VERSION 3.10)
project(Gamble-Style VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Gather project sources (do not overwrite existing files)
file(GLOB_RECURSE GAME_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(game STATIC ${GAME_SOURCES})

# Simple runnable target (skeleton main)
add_executable(gamble_run src/main.cpp)
target_link_libraries(gamble_run PRIVATE game)

# Tests (optional, requires CppUnit)
find_package(CPPUnit QUIET)
if(CPPUnit_FOUND)
  message(STATUS "CppUnit found: enabling tests target")
  enable_testing()
  add_executable(run_tests ressource_codeur/files/MainTestRunner.cpp)
  target_include_directories(run_tests PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/ressource_codeur/files)
  target_link_libraries(run_tests PRIVATE game ${CPPUnit_LIBRARIES})
  add_test(NAME AllTests COMMAND run_tests)
else()
  message(WARNING "CppUnit not found - tests target will be skipped")
endif()
