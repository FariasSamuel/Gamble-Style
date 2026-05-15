# GambleStyle

Jeu de plateau inspiré du Monopoly, avec des mini-jeux intégrés (Tron, DiceBattle),
des cartes à effets variés et une interface graphique SFML.

---

## Prérequis

| Outil | Version minimale |
|---|---|
| g++ | 7 (C++17) |
| CMake | 3.10 |
| SFML | 2.5 |
| libcppunit-dev | *(optionnel — tests uniquement)* |

Sous Debian/Ubuntu :
```bash
sudo apt install libsfml-dev cmake g++ libcppunit-dev
```

---

## Compilation

### Via CMake (recommandé)

```bash
cmake -B build
cmake --build build
```

Les exécutables sont produits dans `bin/` :
- `bin/gamble_run` — jeu principal
- `bin/Tron`       — mini-jeu Tron
- `bin/DiceBattle` — mini-jeu Bataille de dés

### Via Makefile

```bash
make          # compile tout dans bin/ (objets intermédiaires dans obj/)
make run      # compile + lance gamble_run
make clean    # supprime obj/ et les exécutables
```

---

## Lancement

```bash
./bin/gamble_run
```

Le jeu doit être lancé depuis la racine du projet afin que les chemins relatifs
(`assets/`, `board.txt`, `./bin/Tron`, `./bin/DiceBattle`) soient résolus correctement.

---

## Règles du jeu

### Plateau

Le plateau est configuré dans `board.txt` (grille 4×4, 12 cases par défaut).
Les joueurs se déplacent dans le sens horaire après avoir lancé deux dés.

### Types de cases

| Type | Effet |
|---|---|
| **Départ** | +200 € à chaque passage |
| **Propriété** | Achat possible ; si occupée par un adversaire → duel Tron |
| **Événement** | Bataille de dés (DiceBattle) entre tous les joueurs |
| **Aléatoire** | Pioche d'une carte dans le deck |
| **Prison** | Séjour de 1 à 3 tours ; sortie par double ou carte |
| **Aller en Prison** | Téléporte immédiatement le joueur en prison |

### Mini-jeux

- **Tron** (cases Propriété) : duel 1v1. Le joueur dont le serpent survit remporte la mise.
- **DiceBattle** (cases Événement) : tous les joueurs lancent des dés simultanément ;
  le plus grand score remporte la mise de chaque adversaire.

### Cartes aléatoires

| Carte | Effet |
|---|---|
| Anniversaire | Chaque adversaire vous verse X € |
| Argent | Gain ou perte aléatoire de X € (50/50) |
| Déplacement | Avance de 2 à 6 cases |
| Double Gain | Conservée en main ; double votre gain si vous gagnez le prochain duel |
| Sortie de Prison | Conservée en main ; utilisable avant un lancer pour sortir de prison |

### Fin de partie

La partie se termine lorsqu'un seul joueur reste solvable (capital > 0 ou propriétés),
ou à l'expiration du temps configuré. Le vainqueur est le joueur au score le plus élevé
(capital + valeur des propriétés).

---

## Contrôles

### Écran de configuration

| Action | Contrôle |
|---|---|
| Changer le nombre de joueurs | Boutons `-` et `+` |
| Saisir un nom | Clic sur le champ, puis clavier |
| Changer la durée | Clic sur le champ Durée |
| Démarrer | Bouton **Jouer** |

### Jeu principal

| Action | Contrôle |
|---|---|
| Lancer les dés | Bouton **Lancer le dé** (panneau droite) |
| Acheter une propriété | Bouton **Oui** dans le prompt |
| Choisir une mise | Clic sur 50 / 100 / 150 / 200 € puis **Confirmer** |
| Utiliser une carte | Bouton dédié dans le prompt affiché |
| Continuer après une carte | Bouton **Continuer** ou `Espace` / `Entrée` |
| Quitter | `Échap` |

### Mini-jeu Tron

| Joueur | Contrôles |
|---|---|
| Joueur 1 (vert) | `Z` haut, `Q` gauche, `S` bas, `D` droite |
| Joueur 2 (rouge) | Flèches directionnelles |
| Menus | `↑` / `↓` naviguer — `Espace` / `Entrée` valider |

---

## Structure du projet

```
Gamble-Style/
├── assets/          — images (plateau, cases, dés, joueurs, cartes)
├── bin/             — exécutables compilés (gamble_run, Tron, DiceBattle)
├── build/           — répertoire CMake (généré automatiquement)
├── docs/            — documentation supplémentaire
├── include/         — fichiers d'en-tête (.hpp)
├── obj/             — objets intermédiaires Makefile (.o)
├── src/             — sources C++ (logique + SFML + mini-jeux)
├── tests/           — tests unitaires CppUnit
├── board.txt        — configuration du plateau
├── CMakeLists.txt   — script CMake principal
├── makefile         — alternative Makefile
├── RAPPORT.md       — rapport complet avec diagrammes UML
└── README.md        — ce fichier
```

---

## Tests unitaires

Nécessite `libcppunit-dev` installé.

```bash
cmake -B build -DWITH_CPPUNIT=ON
cmake --build build
cd build && ctest --output-on-failure
```

Suites de tests disponibles dans `tests/` :
`TestJoueur`, `TestPlateau`, `TestCasePropriete`, `TestCaseAleatoire`,
`TestCasePrison`, `TestCaseDepart`, `TestCaseEvenement`, `TestCarte`
