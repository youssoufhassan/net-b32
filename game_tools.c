#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"

direction char_to_direction(char c) {
  switch (c) {
    case 'N':
      return NORTH;
    case 'E':
      return EAST;
    case 'S':
      return SOUTH;
    case 'W':
      return WEST;
    default:
      fprintf(stderr, "caractere inconnu lu exit(game)\n");
      exit(EXIT_FAILURE);
  }
}

shape char_to_shape(char c) {
  switch (c) {
    case 'E':
      return EMPTY;
    case 'N':
      return ENDPOINT;
    case 'S':
      return SEGMENT;
    case 'C':
      return CORNER;
    case 'T':
      return TEE;
    case 'X':
      return CROSS;
    default:
      fprintf(stderr, "caractere inconnu lu exit(game)\n");
      exit(EXIT_FAILURE);
  }
}

game game_load(char *filename) {
  if (filename == NULL) {
    fprintf(stderr, "name of file is empty \n");
    exit(EXIT_FAILURE);
  }
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "the file is not opened \n");
    exit(EXIT_FAILURE);
  }
  int cols;
  int rows;
  int wrapping;
  bool wrap;
  if (!fscanf(f, "%d %d %d", &rows, &cols, &wrapping)) {
    fclose(f);
    exit(EXIT_FAILURE);
  }
  wrap = (wrapping != 0);
  shape *shapes = malloc((cols * rows) * sizeof(shape));
  direction *orientation = malloc((cols * rows) * sizeof(direction));
  if (shapes == NULL || orientation == NULL) {
    fprintf(stderr, "not enough memory\n");
    exit(EXIT_FAILURE);
  }
  char *s = malloc(3 * sizeof(char));
  if (s == NULL) {
    fprintf(stderr, "not enough memory\n");
    exit(EXIT_FAILURE);
  }
  int i = 0;
  while (fscanf(f, "%s", s) != EOF) {
    shapes[i] = char_to_shape(s[0]);
    orientation[i] = char_to_direction(s[1]);
    ++i;
  }
  fclose(f);
  game g = game_new_ext(rows, cols, shapes, orientation, wrap);
  free(s);
  free(shapes);
  free(orientation);
  assert(g != NULL);
  return g;
}

void game_save(cgame g, char *filename) {
  if (g == NULL || filename == NULL) {
    fprintf(stderr, "Error game_save");
    exit(EXIT_FAILURE);
  }
  FILE *f = fopen(filename, "w");
  fprintf(f, "%u %u %d\n", g->rows, g->cols, g->wrapping);
  if (g->tab_shape == NULL || g->tab_dir == NULL) {
    fclose(f);
    fprintf(stderr, "Error game_save");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < g->rows; i++) {
    if (g->tab_shape[i] == NULL || g->tab_dir[i] == NULL) {
      fclose(f);
      fprintf(stderr, "Error game_save");
      exit(EXIT_FAILURE);
    }
    for (int j = 0; j < g->cols; j++) {
      if (g->tab_shape[i][j] == EMPTY) {
        fprintf(f, "E");
      }
      if (g->tab_shape[i][j] == ENDPOINT) {
        fprintf(f, "N");
      }
      if (g->tab_shape[i][j] == SEGMENT) {
        fprintf(f, "S");
      }
      if (g->tab_shape[i][j] == CORNER) {
        fprintf(f, "C");
      }
      if (g->tab_shape[i][j] == TEE) {
        fprintf(f, "T");
      }
      if (g->tab_shape[i][j] == CROSS) {
        fprintf(f, "X");
      }
      if (g->tab_dir[i][j] == NORTH) {
        fprintf(f, "N ");
      }
      if (g->tab_dir[i][j] == EAST) {
        fprintf(f, "E ");
      }
      if (g->tab_dir[i][j] == SOUTH) {
        fprintf(f, "S ");
      }
      if (g->tab_dir[i][j] == WEST) {
        fprintf(f, "W ");
      }
    }
    if (i < g->rows - 1) {
      fprintf(f, "\n");
    }
  }
  fclose(f);
}

// @copyright University of Bordeaux. All rights reserved, 2024.

/* ************************************************************************** */

/** @brief Hard-coding of pieces (shape & orientation) in an integer array.
 * @details The 4 least significant bits encode the presence of an half-edge in
 * the N-E-S-W directions (in that order). Thus, binary coding 1100 represents
 * the piece "└" (a corner in north orientation).
 */
static uint _code[NB_SHAPES][NB_DIRS] = {
    {0b0000, 0b0000, 0b0000, 0b0000},  // EMPTY {" ", " ", " ", " "}
    {0b1000, 0b0100, 0b0010, 0b0001},  // ENDPOINT {"^", ">", "v", "<"},
    {0b1010, 0b0101, 0b1010, 0b0101},  // SEGMENT {"|", "-", "|", "-"},
    {0b1100, 0b0110, 0b0011, 0b1001},  // CORNER {"└", "┌", "┐", "┘"}
    {0b1101, 0b1110, 0b0111, 0b1011},  // TEE {"┴", "├", "┬", "┤"}
    {0b1111, 0b1111, 0b1111, 0b1111}   // CROSS {"+", "+", "+", "+"}
};

/* ************************************************************************** */

/** encode a shape and an orientation into an integer code */
static uint _encode_shape(shape s, direction o) { return _code[s][o]; }

/* ************************************************************************** */

/** decode an integer code into a shape and an orientation */
static bool _decode_shape(uint code, shape *s, direction *o) {
  assert(code >= 0 && code < 16);
  assert(s);
  assert(o);
  for (int i = 0; i < NB_SHAPES; i++)
    for (int j = 0; j < NB_DIRS; j++)
      if (code == _code[i][j]) {
        *s = i;
        *o = j;
        return true;
      }
  return false;
}

/* ************************************************************************** */

/** add an half-edge in the direction d */
static void _add_half_edge(game g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g));
  assert(j < game_nb_cols(g));
  assert(d < NB_DIRS);

  shape s = game_get_piece_shape(g, i, j);
  direction o = game_get_piece_orientation(g, i, j);
  uint code = _encode_shape(s, o);
  uint mask = 0b1000 >> d;     // mask with half-edge in the direction d
  assert((code & mask) == 0);  // check there is no half-edge in the direction d
  uint newcode = code | mask;  // add the half-edge in the direction d
  shape news;
  direction newo;
  bool ok = _decode_shape(newcode, &news, &newo);
  assert(ok);
  game_set_piece_shape(g, i, j, news);
  game_set_piece_orientation(g, i, j, newo);
}

/* ************************************************************************** */

#define OPPOSITE_DIR(d) ((d + 2) % NB_DIRS)

/* ************************************************************************** */

/**
 * @brief Add an edge between two adjacent squares.
 * @details This is done by modifying the pieces of the two adjacent squares.
 * More precisely, we add an half-edge to each adjacent square, so as to build
 * an edge between these two squares.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param d the direction of the adjacent square
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game height
 * @pre @p j < game width
 * @return true if an edge can be added, false otherwise
 */
static bool _add_edge(game g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g));
  assert(j < game_nb_cols(g));
  assert(d < NB_DIRS);

  uint nexti, nextj;
  bool next = game_get_ajacent_square(g, i, j, d, &nexti, &nextj);
  if (!next) return false;

  // check if the two half-edges are free
  bool he = game_has_half_edge(g, i, j, d);
  if (he) return false;
  bool next_he = game_has_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));
  if (next_he) return false;

  _add_half_edge(g, i, j, d);
  _add_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));

  return true;
}

/* ************************************************************************** */

game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_empty, uint nb_extra) {
  // Vérifie les préconditions
  if (nb_rows * nb_cols < 2 || nb_empty > (nb_rows * nb_cols - 2)) {
    fprintf(stderr, "Erreur : Paramètres invalides.\n");
    return NULL;
  }

  // Créer un jeu vide
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  if (g == NULL) {
    fprintf(stderr, "Erreur : Impossible de créer le jeu.\n");
    return NULL;
  }

  uint next_i, next_j;
  uint shape_nn_empty = (nb_rows * nb_cols) - nb_empty;  // Nombre de pièces à ajouter

  // Sélection aléatoire d'un point de départ
  uint initial_i = rand() % nb_rows;
  uint initial_j = rand() % nb_cols;
  uint initial_d = rand() % NB_DIRS;

  // Assure que la case a un voisin valide
  int max_attempts = 100;
  while (max_attempts-- > 0 && !game_get_ajacent_square(g, initial_i, initial_j, initial_d, &next_i, &next_j)) {
    initial_i = rand() % nb_rows;
    initial_j = rand() % nb_cols;
    initial_d = rand() % NB_DIRS;
  }
  if (max_attempts <= 0) {
    fprintf(stderr, "Erreur : Impossible de trouver une position valide.\n");
    game_delete(g);
    return NULL;
  }

  // Ajoute une première connexion entre deux pièces
  _add_edge(g, initial_i, initial_j, initial_d);
  _add_edge(g, next_i, next_j, (initial_d + 2) % NB_DIRS);
  shape_nn_empty -= 2;  // Déjà ajouté 2 pièces

  // Construction du jeu
  while (shape_nn_empty > 0) {
    uint i = rand() % nb_rows;
    uint j = rand() % nb_cols;
    uint d = rand() % NB_DIRS;

    // Trouve une direction valide
    max_attempts = 100;
    while (max_attempts-- > 0 && !game_get_ajacent_square(g, i, j, d, &next_i, &next_j)) {
      d = rand() % NB_DIRS;
    }
    if (max_attempts <= 0) {
      fprintf(stderr, "Erreur : Impossible de trouver une direction valide.\n");
      game_delete(g);
      return NULL;
    }

    // Si le voisin est vide, on l'ajoute
    if (game_get_piece_shape(g, i, j) != EMPTY && game_get_piece_shape(g, next_i, next_j) == EMPTY) {
      _add_edge(g, i, j, d);
      _add_edge(g, next_i, next_j, (d + 2) % NB_DIRS);
      shape_nn_empty--;
    }
  }

  // Ajout d'arêtes supplémentaires pour former des cycles
  while (nb_extra > 0) {
    uint i = rand() % nb_rows;
    uint j = rand() % nb_cols;
    uint d = rand() % NB_DIRS;

    if (game_get_piece_shape(g, i, j) != EMPTY) {
      max_attempts = 100;
      while (max_attempts-- > 0 &&
             (!game_get_ajacent_square(g, i, j, d, &next_i, &next_j) ||
              game_get_piece_shape(g, next_i, next_j) == EMPTY || game_check_edge(g, i, j, d) != NOEDGE)) {
        d = rand() % NB_DIRS;
      }
      if (max_attempts <= 0) {
        fprintf(stderr, "Erreur : Impossible de trouver une arête supplémentaire.\n");
        game_delete(g);
        return NULL;
      }
      _add_edge(g, i, j, d);
      _add_edge(g, next_i, next_j, (d + 2) % NB_DIRS);
      nb_extra--;
    }
  }

  return g;
}

/* ************************************************************************** */
bool game_solve_aux(game g, uint *count_solution, bool find_one_solution, uint pos, uint size_gride) {
  if (pos == size_gride) {
    if (game_won(g)) {
      (*count_solution)++;
      game_print(g);
      if (find_one_solution) return true;
    }
    return false;
  }

  uint nb_rotations = NB_DIRS;
  uint i = pos / g->cols;
  uint j = pos % g->cols;
  if (i >= game_nb_rows(g)) return false;
  shape piece_shape = game_get_piece_shape(g, i, j);
  if (piece_shape == SEGMENT) nb_rotations = 2;
  if (piece_shape == CROSS || piece_shape == EMPTY) nb_rotations = 1;
  direction initial_orientation = game_get_piece_orientation(g, i, j);

  for (uint d = 0; d < nb_rotations; d++) {
    game_set_piece_orientation(g, i, j, (initial_orientation + d) % NB_DIRS);
    if (i == 0 && j == 0) {
      if (game_solve_aux(g, count_solution, find_one_solution, pos + 1, size_gride)) {
        return true;
      }
    }

    if ((i != 0 || j != 0) &&
        (game_check_edge(g, i, j, WEST) != MISMATCH || game_check_edge(g, i, j, NORTH) != MISMATCH)) {
      if (game_solve_aux(g, count_solution, find_one_solution, pos + 1, size_gride)) {
        return true;
      }
    }
  }
  game_set_piece_orientation(g, i, j, initial_orientation);
  return find_one_solution ? (*count_solution > 0) : false;
}

bool game_solve(game g) {
  uint count_solution = 0;
  uint size_gride = game_nb_cols(g) * game_nb_rows(g);
  return game_solve_aux(g, &count_solution, true, 0, size_gride);
}

uint game_nb_solutions(cgame g) {
  game g_copy = game_copy(g);
  uint count_solution = 0;
  uint size_gride = game_nb_cols(g_copy) * game_nb_rows(g_copy);

  game_solve_aux(g_copy, &count_solution, false, 0, size_gride);

  game_delete(g_copy);
  return count_solution;
}
