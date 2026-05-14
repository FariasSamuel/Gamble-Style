##############################################################
# Makefile — GambleStyle
# Projet C++ GM4 — INSA Rouen Normandie
##############################################################

CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I include/
LDFLAGS  = -lsfml-graphics -lsfml-window -lsfml-system

# Répertoires
SRC_DIR  = src
OBJ_DIR  = obj
BIN_DIR  = bin
TEST_DIR = ressource_codeur

# Sources et objets du jeu principal
SRCS    = $(wildcard $(SRC_DIR)/*.cpp)
OBJS    = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Exécutable principal
TARGET  = $(BIN_DIR)/gamblestyle

# Sources et objets des tests (CppUnit)
TEST_SRCS   = $(wildcard $(TEST_DIR)/Test*.cpp)
TEST_RUNNER = $(TEST_DIR)/MainTestRunner.cpp
TEST_OBJS   = $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/test_%.o, $(TEST_SRCS))
TEST_TARGET = $(BIN_DIR)/testEval

LDTEST   = -lcppunit

##############################################################
# Cibles principales
##############################################################

.PHONY: all clean remove tests docs

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

##############################################################
# Tests unitaires (CppUnit)
##############################################################

tests: $(TEST_TARGET)
	./$(TEST_TARGET)

# On compile le MainTestRunner qui inclut tous les Test*.cpp
$(TEST_TARGET): $(filter-out $(OBJ_DIR)/main.o, $(OBJS)) \
                $(OBJ_DIR)/test_MainTestRunner.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDTEST)

$(OBJ_DIR)/test_MainTestRunner.o: $(TEST_RUNNER) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

##############################################################
# Documentation Doxygen
##############################################################

docs:
	doxygen Doxyfile

##############################################################
# Nettoyage
##############################################################

clean:
	rm -f $(OBJ_DIR)/*.o

remove: clean
	rm -f $(TARGET) $(TEST_TARGET)

##############################################################
# Création des répertoires si absents
##############################################################

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)
