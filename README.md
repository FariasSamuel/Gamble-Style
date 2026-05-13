Gamble-Style
===============

Projet école — squelette initial créé à partir du cahier des spécifications fourni dans `ressource_codeur`.

Structure importante:
- `include/` : en-têtes publiques
- `src/` : sources du projet
- `ressource_codeur/` : cahier des specs et tests fournis
- `docs/` : documentation générée

Quickstart (CMake):

```bash
mkdir build && cd build
cmake ..
cmake --build .
./gamble_run
```

Pour exécuter les tests (si CppUnit installé):

```bash
cmake -S . -B build -DCPPUNIT_ROOT=/path/to/cppunit
cmake --build build --target run_tests
ctest --test-dir build
```
