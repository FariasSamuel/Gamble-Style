cmake_minimum_required(VERSION 3.10)
project(GambleStyle CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# ── Bibliothèque du jeu (sans main) ──────────────────────────────────────────
file(GLOB_RECURSE GAME_SRCS src/*.cpp)
list(REMOVE_ITEM GAME_SRCS ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_library(gamblestyle_lib ${GAME_SRCS})

# ── Exécutable principal ─────────────────────────────────────────────────────
add_executable(gamblestyle src/main.cpp)
target_link_libraries(gamblestyle gamblestyle_lib)
set_target_properties(gamblestyle PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)

# ── Tests CppUnit ─────────────────────────────────────────────────────────────
find_library(CPPUNIT_LIB cppunit)

if(CPPUNIT_LIB)
    add_executable(testEval ressource_codeur/MainTestRunner.cpp)
    target_link_libraries(testEval gamblestyle_lib ${CPPUNIT_LIB})
    set_target_properties(testEval PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)
    enable_testing()
    add_test(NAME UnitTests COMMAND testEval)
    message(STATUS "CppUnit trouvé — cible 'testEval' disponible")
else()
    message(WARNING "CppUnit non trouvé — les tests ne seront pas compilés")
endif()
