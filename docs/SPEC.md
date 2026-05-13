Spécifications (extraites de `ressource_codeur`)
=============================================

Conventions de nommage:
- Classes: `Majuscule` (ex: `Plateau`)
- Attributs: minuscules avec underscore ou tirets selon le projet
- Méthodes: minuscules, underscore entre mots (ex: `donner_capital`)

Fonctionnalités mentionnées:
- Gestion de joueurs (`Joueur`), capital, cases courantes
- Plateaux et cases: `CasePropriete`, `CaseDepart`, `CasePrison`, `CaseEvenement`, `CaseAleatoire`
- Cartes: `CarteAnniversaire`, `CarteDeplacement`, `CarteSortiePrison`, etc.
- Tests unitaires CppUnit fournis en `ressource_codeur/files`

Décisions pour le squelette:
- Utiliser CMake pour la compilation
- Ajouter une cible `game` statique qui compile tout `src/*.cpp`
- Fournir une `main.cpp` minimale qui inclut `Joueur.hpp` et démarre un programme vide
