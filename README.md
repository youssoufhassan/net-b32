# Net B32 — Jeu de logique en C

**Net B32** est un jeu de logique développé entièrement en **C** dans le cadre de la formation en informatique.

Le joueur doit orienter les différentes pièces du plateau afin de construire un réseau dans lequel **toutes les pièces sont reliées entre elles**.

Le projet propose plusieurs modes d'interaction, notamment une **version textuelle**, une **interface graphique SDL2** et une **version web**.

---

## Présentation du jeu

Le plateau est composé de différentes pièces représentant des segments de réseau :

* extrémités ;
* segments ;
* coins ;
* intersections ;
* embranchements.

Chaque pièce peut être tournée afin de modifier son orientation.

L'objectif est de trouver une configuration dans laquelle **l'ensemble du réseau est connecté**.

```text
        ┌───┐
        │   │
    ────┘   └────
            │
            │
            └────
```

Le joueur peut notamment :

* faire tourner les pièces ;
* annuler ses dernières actions ;
* refaire des actions annulées ;
* générer des configurations ;
* résoudre automatiquement des puzzles ;
* sauvegarder des parties ;
* charger des parties.

---

## Fonctionnalités

### Jeu

* Création et manipulation du plateau ;
* Rotation des pièces ;
* Vérification de la connectivité du réseau ;
* Gestion des actions du joueur ;
* Annulation des actions ;
* Rétablissement des actions annulées ;
* Génération de puzzles ;
* Résolution automatique.

### Gestion des parties

* Sauvegarde d'une partie ;
* Chargement d'une partie ;
* Gestion de différents états du jeu.

### Interfaces

Le projet contient plusieurs interfaces :

* **Interface texte** ;
* **Interface graphique SDL2** ;
* **Version web**.

---

## Architecture

Le projet sépare la logique du jeu des différentes interfaces.

```text
                         ┌─────────────────┐
                         │   Moteur du jeu │
                         │      en C       │
                         └────────┬────────┘
                                  │
                ┌─────────────────┼─────────────────┐
                │                 │                 │
                ▼                 ▼                 ▼
        ┌──────────────┐  ┌──────────────┐  ┌──────────────┐
        │ Interface    │  │ Interface    │  │    Version   │
        │    texte     │  │    SDL2      │  │     Web      │
        └──────────────┘  └──────────────┘  └──────────────┘
```

Cette organisation permet de réutiliser la logique principale du jeu avec plusieurs interfaces.

---

## Structure du projet

```text
net-b32/
│
├── game.c
├── game.h
├── game_aux.c
├── game_aux.h
├── game_ext.c
├── game_ext.h
├── game_random.c
├── game_solve.c
├── game_struct.h
├── game_tools.c
├── game_tools.h
├── game_sdl.c
├── game_sdl.h
├── game_text.c
│
├── queue.c
├── queue.h
│
├── main.c
│
├── test_queue.c
├── test_save.c
├── game_test_adejesus.c
├── game_test_hyoussouf.c
├── game_test_mwache.c
│
├── CMakeLists.txt
├── sdl2.cmake
├── .clang-format
├── .gitignore
│
├── res/
│   ├── arial.ttf
│   ├── background.png
│   ├── corner_*.png
│   ├── cross.png
│   ├── endpoint_*.png
│   ├── segment_*.png
│   └── tee_*.png
│
└── web/
    ├── game.html
    ├── demo.js
    ├── style.css
    ├── wrapper.c
    ├── Makefile
    │
    ├── images/
    │
    └── src/
        ├── game.c
        ├── game.h
        ├── game_aux.c
        ├── game_aux.h
        ├── game_ext.c
        ├── game_ext.h
        ├── game_private.c
        ├── game_private.h
        ├── game_struct.h
        ├── game_tools.c
        ├── game_tools.h
        ├── queue.c
        └── queue.h
```

---

## Organisation du moteur

Le moteur du jeu est réparti en plusieurs modules.

| Module          | Rôle                            |
| --------------- | ------------------------------- |
| `game.c`        | Gestion principale du jeu       |
| `game_aux.c`    | Fonctions auxiliaires           |
| `game_ext.c`    | Fonctions complémentaires       |
| `game_struct.h` | Structures de données           |
| `game_tools.c`  | Fonctions utilitaires           |
| `game_random.c` | Génération de configurations    |
| `game_solve.c`  | Résolution des puzzles          |
| `game_text.c`   | Interface texte                 |
| `game_sdl.c`    | Interface graphique SDL2        |
| `queue.c`       | Implémentation d'une file       |
| `main.c`        | Point d'entrée de l'application |

---

## Résolution automatique

Le projet possède un module dédié à la résolution des puzzles :

```text
game_solve.c
```

Le solveur permet de rechercher une configuration permettant de connecter l'ensemble du réseau.

Cette partie du projet met en pratique des notions d'**algorithmique et de recherche de solution**.

---

## Génération aléatoire

Le fichier :

```text
game_random.c
```

est dédié à la génération de configurations aléatoires du jeu.

Cela permet notamment de produire différentes instances du puzzle.

---

## Gestion des actions

Le jeu permet au joueur de modifier progressivement le plateau.

Les actions peuvent être annulées puis rétablies.

Une structure de **file (`queue`)** est également implémentée dans :

```text
queue.c
queue.h
```

afin de gérer certaines opérations nécessaires au fonctionnement du jeu.

---

## Interface graphique

Le projet propose une interface graphique basée sur **SDL2**.

Les fichiers principaux sont :

```text
game_sdl.c
game_sdl.h
```

Les ressources graphiques utilisées par cette interface sont stockées dans :

```text
res/
```

On y retrouve notamment les différentes représentations des pièces du réseau :

```text
corner
cross
endpoint
segment
tee
```

ainsi que les ressources graphiques générales du jeu.

---

## Version web

Le projet contient également une version web dans :

```text
web/
```

Cette version comprend notamment :

```text
web/
├── game.html
├── demo.js
├── style.css
├── wrapper.c
├── Makefile
├── images/
└── src/
```

La présence d'un `wrapper.c` permet d'adapter le code du moteur du jeu pour son utilisation dans l'environnement web.

---

## Tests

Le projet contient plusieurs fichiers de tests :

```text
test_queue.c
test_save.c

game_test_adejesus.c
game_test_hyoussouf.c
game_test_mwache.c
```

Les tests couvrent notamment :

* la gestion de la file ;
* la sauvegarde ;
* différents comportements du moteur du jeu.

---

## Compilation

Le projet utilise **CMake** pour sa configuration et sa compilation.

Le fichier principal est :

```text
CMakeLists.txt
```

Un fichier spécifique est également présent pour l'intégration de SDL2 :

```text
sdl2.cmake
```

### Prérequis

* Compilateur C ;
* CMake ;
* SDL2 pour l'interface graphique ;
* environnement compatible avec les outils de compilation du projet.

---

## Compilation avec CMake

Créer un répertoire de compilation :

```bash
mkdir build
cd build
```

Configurer le projet :

```bash
cmake ..
```

Compiler :

```bash
cmake --build .
```

Selon la configuration de l'environnement, l'installation de SDL2 peut être nécessaire pour compiler la version graphique.

---

## Concepts techniques

Ce projet met en pratique plusieurs notions de programmation en C :

### Programmation en C

* structures ;
* pointeurs ;
* gestion de mémoire ;
* organisation modulaire ;
* fichiers `.c` et `.h` ;
* compilation multi-fichiers.

### Algorithmique

* manipulation de graphes / réseaux ;
* recherche de solution ;
* génération de configurations ;
* vérification de connectivité.

### Structures de données

* files ;
* structures représentant le plateau ;
* gestion des états du jeu.

### Tests

* tests unitaires ;
* tests des structures de données ;
* tests de sauvegarde ;
* validation du comportement du moteur.

### Interfaces

* interface en ligne de commande ;
* interface graphique SDL2 ;
* adaptation du moteur pour une utilisation web.

---

## Technologies

| Technologie                 | Utilisation                  |
| --------------------------- | ---------------------------- |
| **C**                       | Langage principal            |
| **CMake**                   | Compilation et configuration |
| **SDL2**                    | Interface graphique          |
| **Make**                    | Build de la version web      |
| **HTML / CSS / JavaScript** | Interface web                |
| **Git**                     | Gestion de versions          |

---

## Compétences développées

Ce projet m'a permis de travailler notamment sur :

* programmation C ;
* conception modulaire ;
* structures de données ;
* algorithmique ;
* recherche de solutions ;
* gestion de la mémoire ;
* tests unitaires ;
* compilation avec CMake ;
* programmation graphique avec SDL2 ;
* adaptation d'un programme C pour le web ;
* travail en équipe avec Git.

---

## Équipe

Projet réalisé en équipe :

* **Hassan Youssouf**
* **Anthony De Jesus**
* **Matéo Wache**

---

## Contexte

Projet réalisé dans le cadre de la formation **L3 Informatique — Université de Bordeaux**.

---

## Statut

**Projet universitaire — jeu de logique en C**

Le projet comprend un moteur de jeu, plusieurs interfaces, un système de résolution, une génération de configurations ainsi que des tests automatisés.
