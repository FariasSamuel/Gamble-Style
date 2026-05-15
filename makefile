# ============================================================
#  GambleStyle — Makefile alternatif (CMake reste recommandé)
#
#  Structure produite :
#    obj/   — fichiers objets intermédiaires (.o)
#    bin/   — exécutables finaux
#
#  Cibles :
#    make          → compile tout (gamble_run, Tron, DiceBattle)
#    make run      → compile + lance gamble_run
#    make clean    → supprime obj/ et les exécutables
#
#  Dépendances : g++ ≥ 7, pkg-config, libsfml-dev ≥ 2.5
# ============================================================

CXX      = g++
STD      = -std=c++17
WARN     = -Wall -Wextra
IFLAGS   = -Iinclude
SFMLCF   = $(shell pkg-config --cflags sfml-graphics sfml-window sfml-system)
SFMLLF   = $(shell pkg-config --libs   sfml-graphics sfml-window sfml-system)

CXXFLAGS = $(STD) $(WARN) $(IFLAGS) $(SFMLCF)

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

# ── Sources de la bibliothèque métier (sans main ni SFML ni mini-jeux) ────────
EXCLUDE  = src/main.cpp src/PlateauSFML.cpp src/Tron.cpp src/DiceBattle.cpp \
           src/CartePosition.cpp src/CartePrison.cpp

ALL_SRCS  = $(wildcard $(SRCDIR)/*.cpp)
GAME_SRCS = $(filter-out $(EXCLUDE), $(ALL_SRCS))
GAME_OBJS = $(GAME_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# ── Objets spécifiques à chaque exécutable ────────────────────────────────────
OBJ_MAIN      = $(OBJDIR)/main.o
OBJ_SFML      = $(OBJDIR)/PlateauSFML.o
OBJ_TRON      = $(OBJDIR)/Tron.o
OBJ_DICE      = $(OBJDIR)/DiceBattle.o

.PHONY: all run clean

all: $(BINDIR)/gamble_run $(BINDIR)/Tron $(BINDIR)/DiceBattle

# ── gamble_run ────────────────────────────────────────────────────────────────
$(BINDIR)/gamble_run: $(GAME_OBJS) $(OBJ_SFML) $(OBJ_MAIN) | $(BINDIR)
	$(CXX) $^ -o $@ $(SFMLLF)
	@echo ">>> bin/gamble_run pret"

# ── Tron ──────────────────────────────────────────────────────────────────────
$(BINDIR)/Tron: $(OBJ_TRON) | $(BINDIR)
	$(CXX) $^ -o $@ $(SFMLLF)
	@echo ">>> bin/Tron pret"

# ── DiceBattle ────────────────────────────────────────────────────────────────
$(BINDIR)/DiceBattle: $(OBJ_DICE) | $(BINDIR)
	$(CXX) $^ -o $@ $(SFMLLF)
	@echo ">>> bin/DiceBattle pret"

# ── Règle générique : .cpp → .o dans obj/ ─────────────────────────────────────
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ── Création des répertoires si absents ───────────────────────────────────────
$(OBJDIR) $(BINDIR):
	@mkdir -p $@

# ── Nettoyage ─────────────────────────────────────────────────────────────────
clean:
	rm -rf $(OBJDIR) $(BINDIR)/gamble_run $(BINDIR)/Tron $(BINDIR)/DiceBattle
	@echo "Nettoyage termine."

run: $(BINDIR)/gamble_run
	./$(BINDIR)/gamble_run
