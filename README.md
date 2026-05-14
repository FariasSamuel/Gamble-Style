# 🎲 GambleStyle

Jeu de plateau C++ pour 2 à 4 joueurs — Monopoly × Mario Party.
**GM4 — INSA Rouen Normandie**

> LANDREAU Marius · DECHAMPS Gatien · HIS Adrien · GUILLOT Thomas
> FEUILLOLAY Enora · FARIAS Samuel · LE Tuan Anh · PARAVEMAN Lucas

---

## Structure du projet

```
GambleStyle/
├── include/          ← Headers (.hpp) de toutes les classes
├── src/              ← Implémentations (.cpp)
├── ressource_codeur/ ← Tests unitaires CppUnit
├── docs/             ← Documentation Doxygen (générée)
├── assets/           ← Ressources graphiques SFML
├── bin/              ← Exécutables (générés)
├── obj/              ← Fichiers objets (générés)
├── Doxyfile          ← Configuration Doxygen
├── makefile          ← Compilation Make
└── CMakeLists.txt    ← Compilation CMake
```

---

## Compilation

```bash
# Avec Make
make          # Compile le jeu principal → bin/gamblestyle
make tests    # Compile et lance les tests CppUnit → bin/testEval
make docs     # Génère la documentation → docs/html/
make clean    # Supprime les .o
make remove   # Supprime tout (objets + exécutables)

# Avec CMake
mkdir build && cd build
cmake ..
make
```

**Dépendances :** g++ (C++11), CppUnit (tests), Doxygen (doc), SFML (mini-jeux graphiques)

---

## Classes principales

| Classe | Rôle |
|--------|------|
| `Joueur` | Capital, propriétés, déplacements, condition financière |
| `Plateau` | Boucle de jeu, transferts d'argent, ordre des joueurs |
| `Case` | Abstraite — CaseDepart, CaseAleatoire, CasePropriete, CasePrison, CaseEvenement |
| `Minijeu` | Interface mini-jeux (Tron, Pong, Puissance 4…) |
| `Carte` | Abstraite — 7 sous-types (Gain, Amende, Anniversaire…) |

---

## Tests

```bash
make tests
# ou directement :
./bin/testEval
```

Les résultats sont exportés dans `cppAllTestsResults.xml`.

---

## Documentation

```bash
make docs
# Ouvrir docs/html/index.html dans un navigateur
```
