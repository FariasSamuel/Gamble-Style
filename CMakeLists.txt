cmake_minimum_required(VERSION 3.10)
project(Gamble-Style VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Gather project sources (do not overwrite existing files)
file(GLOB_RECURSE GAME_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")
add_library(game STATIC ${GAME_SOURCES})
target_link_libraries(game PUBLIC sfml-graphics sfml-window sfml-system)

# Simple runnable target
add_executable(gamble_run src/main.cpp)
target_link_libraries(gamble_run PRIVATE game sfml-graphics sfml-window sfml-system)

# Tests (optional, requires CppUnit)
option(WITH_CPPUNIT "Enable building tests with CppUnit" OFF)
if(WITH_CPPUNIT)
  # Try standard find_package first
  find_package(CPPUnit QUIET)
  if(NOT CPPUnit_FOUND AND NOT CPPUnit_FOUND)
    # Fallback: try to locate header and library directly
    find_path(CPPUNIT_INCLUDE_DIR NAMES cppunit/TestCase.h)
    find_library(CPPUNIT_LIBRARY NAMES cppunit cppunit-1.15)
    if(CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARY)
      set(CPPUnit_FOUND TRUE)
      set(CPPUnit_INCLUDE_DIRS ${CPPUNIT_INCLUDE_DIR})
      set(CPPUnit_LIBRARIES ${CPPUNIT_LIBRARY})
    endif()
  endif()

  if(CPPUnit_FOUND)
    message(STATUS "CppUnit found: enabling tests target")
    enable_testing()
    file(GLOB TEST_SOURCES ${PROJECT_SOURCE_DIR}/ressource_codeur/files/Test*.cpp)
    # TestMinijeu.cpp nécessite Minijeu.hpp (remplacé par ./Tron)
    list(FILTER TEST_SOURCES EXCLUDE REGEX ".*TestMinijeu\\.cpp$")
    add_executable(run_tests ressource_codeur/files/MainTestRunner.cpp ${TEST_SOURCES})
    target_include_directories(run_tests PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/ressource_codeur/files ${CPPUnit_INCLUDE_DIRS})
    target_compile_definitions(run_tests PRIVATE MAIN_WRAPPER)
    target_link_libraries(run_tests PRIVATE game ${CPPUnit_LIBRARIES})
    add_test(NAME AllTests COMMAND run_tests)
  else()
    message(FATAL_ERROR "WITH_CPPUNIT=ON but CppUnit was not found on the system. Install libcppunit-dev or set WITH_CPPUNIT=OFF.")
  endif()
else()
  message(STATUS "Tests disabled (WITH_CPPUNIT=OFF). To enable, set -DWITH_CPPUNIT=ON and ensure CppUnit is installed")
endif()
