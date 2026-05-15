cmake_minimum_required(VERSION 3.10)
project(Gamble-Style VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tous les exécutables sont produits dans bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# ── Bibliothèque logique métier (sans SFML, testable sans installation SFML) ──
# On exclut : les entrées contenant un main() séparé ou des dépendances SFML.
file(GLOB_RECURSE GAME_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*/PlateauSFML\\.cpp$")
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*/CartePosition\\.cpp$")
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*/CartePrison\\.cpp$")
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*/Tron\\.cpp$")
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*/DiceBattle\\.cpp$")
add_library(game STATIC ${GAME_SOURCES})

# ── Exécutable principal : logique + couche SFML ─────────────────────────────
add_executable(gamble_run src/main.cpp src/PlateauSFML.cpp)
target_link_libraries(gamble_run PRIVATE game sfml-graphics sfml-window sfml-system)

# ── Mini-jeu Tron (lancé en sous-processus par CasePropriete / CaseEvenement) ─
add_executable(Tron src/Tron.cpp)
target_link_libraries(Tron PRIVATE sfml-graphics sfml-window sfml-system)

# ── Mini-jeu DiceBattle (lancé en sous-processus par CaseEvenement) ──────────
add_executable(DiceBattle src/DiceBattle.cpp)
target_link_libraries(DiceBattle PRIVATE sfml-graphics sfml-window sfml-system)

# ── Tests CppUnit (optionnel) ─────────────────────────────────────────────────
option(WITH_CPPUNIT "Enable building tests with CppUnit" OFF)
if(WITH_CPPUNIT)
  find_package(CPPUnit QUIET)
  if(NOT CPPUnit_FOUND)
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
    file(GLOB TEST_SOURCES ${PROJECT_SOURCE_DIR}/tests/Test*.cpp)
    # TestMinijeu.cpp référence une classe Minijeu remplacée par l'exécutable Tron
    list(FILTER TEST_SOURCES EXCLUDE REGEX ".*TestMinijeu\\.cpp$")
    add_executable(run_tests tests/MainTestRunner.cpp ${TEST_SOURCES})
    target_include_directories(run_tests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/tests
        ${CPPUnit_INCLUDE_DIRS})
    target_compile_definitions(run_tests PRIVATE MAIN_WRAPPER)
    target_link_libraries(run_tests PRIVATE game ${CPPUnit_LIBRARIES})
    add_test(NAME AllTests COMMAND run_tests)
  else()
    message(FATAL_ERROR
        "WITH_CPPUNIT=ON but CppUnit was not found. "
        "Install libcppunit-dev or set -DWITH_CPPUNIT=OFF.")
  endif()
else()
  message(STATUS
      "Tests disabled (WITH_CPPUNIT=OFF). "
      "To enable: cmake -DWITH_CPPUNIT=ON (requires libcppunit-dev).")
endif()
