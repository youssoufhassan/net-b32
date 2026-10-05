#include "game_aux.h"

#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue.h"
void game_print(cgame g) {
  if (g == NULL || g->tab_shape == NULL || g->tab_dir == NULL) {
    fprintf(stderr, "error");
    exit(EXIT_FAILURE);
  }
  printf("    ");
  for (int x = 0; x < g->cols; x++) {
    printf(" %d", x);
  }
  printf("\n");
  printf("     ");
  for (int x = 0; x < g->cols; x++) {
    printf("--");
  }
  printf("\n");
  for (int i = 0; i < g->rows; i++) {
    printf("  %d |", i);
    for (int j = 0; j < g->cols; j++) {
      if (g->tab_shape[i] == NULL) {
        fprintf(stderr, "Error game_print: tab_shape[i] in g is empty");
        exit(EXIT_FAILURE);
      }
      if (g->tab_dir[i] == NULL) {
        fprintf(stderr, "Error game_print: tab_dir[i] in g is empty");
        exit(EXIT_FAILURE);
      }
      if (g->tab_shape[i][j] == EMPTY) {
        printf("  ");
      }
      if (g->tab_shape[i][j] == CROSS) {
        printf("+ ");
      }
      if (g->tab_shape[i][j] == ENDPOINT) {
        if (g->tab_dir[i][j] == NORTH) {
          printf("^ ");
        }
        if (g->tab_dir[i][j] == EAST) {
          printf("> ");
        }
        if (g->tab_dir[i][j] == SOUTH) {
          printf("v ");
        }
        if (g->tab_dir[i][j] == WEST) {
          printf("< ");
        }
      }
      if (g->tab_shape[i][j] == SEGMENT) {
        if (g->tab_dir[i][j] == NORTH) {
          printf("| ");
        }
        if (g->tab_dir[i][j] == EAST) {
          printf("- ");
        }
        if (g->tab_dir[i][j] == SOUTH) {
          printf("| ");
        }
        if (g->tab_dir[i][j] == WEST) {
          printf("- ");
        }
      }
      if (g->tab_shape[i][j] == CORNER) {
        if (g->tab_dir[i][j] == NORTH) {
          printf("└ ");
        }
        if (g->tab_dir[i][j] == EAST) {
          printf("┌ ");
        }
        if (g->tab_dir[i][j] == SOUTH) {
          printf("┐ ");
        }
        if (g->tab_dir[i][j] == WEST) {
          printf("┘ ");
        }
      }
      if (g->tab_shape[i][j] == TEE) {
        if (g->tab_dir[i][j] == NORTH) {
          printf("┴ ");
        }
        if (g->tab_dir[i][j] == EAST) {
          printf("├ ");
        }
        if (g->tab_dir[i][j] == SOUTH) {
          printf("┬ ");
        }
        if (g->tab_dir[i][j] == WEST) {
          printf("┤ ");
        }
      }
    }
    printf("| \n");
  }
  printf("     ");
  for (int x = 0; x < g->cols; x++) {
    printf("--");
  }
  printf("\n");
}

game game_default(void) {
  shape expected_shapes[] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,      TEE,      TEE, TEE,
                             TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, TEE,
                             CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};

  direction expected_orientations[] = {WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST,
                                       EAST, EAST,  NORTH, WEST,  WEST,  EAST,  SOUTH, SOUTH, NORTH,
                                       WEST, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH};

  game g = game_new(expected_shapes, expected_orientations);
  if (g == NULL) {
    fprintf(stderr, "Error game_default: game g is empty\n");
    exit(EXIT_FAILURE);
  }
  return g;
}

game game_default_solution(void) {
  shape expected_shapes[] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,      TEE,      TEE, TEE,
                             TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, TEE,
                             CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};
  direction expected_orientations[] = {EAST,  WEST,  EAST,  SOUTH, SOUTH, EAST,  SOUTH, SOUTH, NORTH,
                                       WEST,  NORTH, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH, NORTH,
                                       SOUTH, SOUTH, EAST,  NORTH, WEST,  NORTH, NORTH};
  game g = game_new(expected_shapes, expected_orientations);
  if (g == NULL) {
    fprintf(stderr, "Error game_default_solution: game g is empty after the call of game_new.\n");
    exit(EXIT_FAILURE);
  }
  return g;
}

bool game_get_ajacent_square(cgame g, uint i, uint j, direction d, uint* pi_next, uint* pj_next) {
  if (g == NULL || pi_next == NULL || pj_next == NULL) {
    fprintf(stderr, "Error game_get_adacent_square: g, pi_next, or pj_next is empty");
    exit(EXIT_FAILURE);
  }
  if (i >= g->rows || j >= g->cols) {
    fprintf(stderr, "Error game_get_adacent_square: i or j out of bounds");
    exit(EXIT_FAILURE);
  }
  *pi_next = i;
  *pj_next = j;

  switch (d) {
    case NORTH:
      if (!g->wrapping) {
        if (i > 0) {
          *pi_next = i - 1;
          if (*pi_next < 0) {
            return false;
          }
          return true;
        }
      }
      if (g->wrapping) {
        uint next_square_n = i - 1;
        if (next_square_n == -1) {
          next_square_n = g->rows - 1;
        }
        *pi_next = next_square_n;
        return true;
      }
      break;
    case SOUTH:
      if (!g->wrapping) {
        if (i < g->rows - 1) {
          *pi_next = i + 1;
          return true;
        }
      }
      if (g->wrapping) {
        uint next_square_s = i + 1;
        if (next_square_s == g->rows) {
          next_square_s = 0;
        }
        *pi_next = next_square_s;
        return true;
      }
      break;
    case EAST:
      if (!g->wrapping) {
        if (j < g->cols - 1) {
          *pj_next = j + 1;
          return true;
        }
      }
      if (g->wrapping) {
        uint next_square_e = j + 1;
        if (next_square_e == g->cols) {
          next_square_e = 0;
        }
        *pj_next = next_square_e;
        return true;
      }
      break;
    case WEST:
      if (!g->wrapping) {
        if (j > 0) {
          *pj_next = j - 1;
          return true;
        }
      }
      if (g->wrapping) {
        uint next_square_w = j - 1;
        if (next_square_w == -1) {
          next_square_w = g->cols - 1;
        }
        *pj_next = next_square_w;
        return true;
      }
      break;
    default:
      break;
  }

  return false;
}

bool game_has_half_edge(cgame g, uint i, uint j, direction d) {
  if (g == NULL) {
    fprintf(stderr, "Error: game is NULL.\n");
    exit(EXIT_FAILURE);
  }

  if (d < NORTH || d >= NB_DIRS) {
    fprintf(stderr, "Error game_has_half_edge: invalid direction value.\n");
    exit(EXIT_FAILURE);
  }

  if (g->tab_shape == NULL || g->tab_dir == NULL) {
    fprintf(stderr, "Error: tab_shape or tab_dir is NULL.\n");
    exit(EXIT_FAILURE);
  }

  if (i >= g->rows || j >= g->cols || i < 0 || j < 0) {
    fprintf(stderr, "Error: indices i or j are out of bounds (rows: %u, cols: %u).\n", g->rows, g->cols);
    exit(EXIT_FAILURE);
  }

  // Vérification si la case est vide (pas de moitié de bord)
  if (g->tab_shape[i][j] == EMPTY) {
    return false;
  }

  // Vérification du type de forme et de la direction
  switch (g->tab_shape[i][j]) {
    case ENDPOINT:
      // L'endpoint n'a qu'une seule direction possible
      return (g->tab_dir[i][j] == d);

    case SEGMENT:
      // Segment : autorise seulement les directions opposées
      if (g->tab_dir[i][j] == NORTH || g->tab_dir[i][j] == SOUTH) {
        return (d == NORTH || d == SOUTH);
      } else if (g->tab_dir[i][j] == EAST || g->tab_dir[i][j] == WEST) {
        return (d == EAST || d == WEST);
      }
      break;

    case CORNER:
      // Corners : vérifie si la direction correspond à une arête du coin
      if (g->tab_dir[i][j] == NORTH) {
        return (d == NORTH || d == EAST);
      } else if (g->tab_dir[i][j] == EAST) {
        return (d == EAST || d == SOUTH);
      } else if (g->tab_dir[i][j] == SOUTH) {
        return (d == SOUTH || d == WEST);
      } else if (g->tab_dir[i][j] == WEST) {
        return (d == WEST || d == NORTH);
      }
      break;

    case TEE:
      // TEE : vérifie si la direction est bien dans l'une des 3 directions
      if (g->tab_dir[i][j] == NORTH) {
        return (d == NORTH || d == EAST || d == WEST);
      } else if (g->tab_dir[i][j] == EAST) {
        return (d == EAST || d == NORTH || d == SOUTH);
      } else if (g->tab_dir[i][j] == SOUTH) {
        return (d == SOUTH || d == EAST || d == WEST);
      } else if (g->tab_dir[i][j] == WEST) {
        return (d == WEST || d == NORTH || d == SOUTH);
      }
      break;

    case CROSS:
      return true;

    default:
      break;
  }

  return false;
}

edge_status game_check_edge(cgame g, uint i, uint j, direction d) {
  if (g == NULL) {
    fprintf(stderr, "Error game_check_edge: g is empty");
    exit(EXIT_FAILURE);
  }
  if (i < 0 || i >= g->rows) {
    fprintf(stderr, "Error game_check_edge: i is inferior to 0 or superior or equal to the number of rows.\n");
    exit(EXIT_FAILURE);
  }
  if (j < 0 || j >= g->cols) {
    fprintf(stderr, "Error game_check_edge: j is inferior to 0 or superior or equal to number of cols.\n");
    exit(EXIT_FAILURE);
  }
  if (d >= NB_DIRS || d < 0) {
    fprintf(stderr, "Error game_check_edge: d is inferior to 0 or superior or equal to NB_DIRS.\n");
    exit(EXIT_FAILURE);
  }
  bool pos1 = game_has_half_edge(g, i, j, d);
  if (!pos1) {
    return NOEDGE;
  }
  bool pos2 = false;
  unsigned int pi_next;
  unsigned int pj_next;
  bool res = game_get_ajacent_square(g, i, j, d, &pi_next, &pj_next);
  if (res) {
    if (d == NORTH) {
      pos2 = game_has_half_edge(g, pi_next, pj_next, SOUTH);
    }
    if (d == EAST) {
      pos2 = game_has_half_edge(g, pi_next, pj_next, WEST);
    }
    if (d == SOUTH) {
      pos2 = game_has_half_edge(g, pi_next, pj_next, NORTH);
    }
    if (d == WEST) {
      pos2 = game_has_half_edge(g, pi_next, pj_next, EAST);
    }
  } else {
    return MISMATCH;
  }
  if (pos1 == true && pos2 == true) {
    return MATCH;
  } else {
    return MISMATCH;
  }
}

bool game_is_wrapping_enabled(cgame g) { return g->wrapping; }

bool game_is_well_paired(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Error game_is_well_paired: game is NULL.\n");
    return false;
  }

  for (int i = 0; i < g->rows; i++) {
    for (int j = 0; j < g->cols; j++) {
      if (g->tab_shape[i][j] == EMPTY) {
        continue;
      }

      for (int dir = 0; dir < NB_DIRS; dir++) {
        uint next_i, next_j;
        bool has_adjacent = game_get_ajacent_square(g, i, j, dir, &next_i, &next_j);
        edge_status edge1 = game_check_edge(g, i, j, dir);
        if (edge1 == MISMATCH) {
          return false;
        }

        if (g->wrapping || has_adjacent) {
          edge_status edge = game_check_edge(g, i, j, dir);
          if (edge == MISMATCH) {
            return false;
          }
        }
      }
    }
  }

  return true;
}

bool game_is_connected(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Error game_is_connected: g is NULL.\n");
    return false;
  }

  // Allocation de la matrice de connexions
  bool** connected = malloc(sizeof(bool*) * g->rows);
  if (connected == NULL) {
    fprintf(stderr, "Error game_is_connected: Memory allocation failed.\n");
    return false;
  }

  for (uint i = 0; i < g->rows; i++) {
    connected[i] = malloc(sizeof(bool) * g->cols);
    if (connected[i] == NULL) {
      fprintf(stderr, "Error game_is_connected: Memory allocation failed.\n");
      for (uint j = 0; j < i; j++) {
        free(connected[j]);
      }
      free(connected);
      return false;
    }
  }

  // Initialisation de la matrice de connexions à false
  for (uint i = 0; i < g->rows; i++) {
    for (uint j = 0; j < g->cols; j++) {
      connected[i][j] = false;
    }
  }

  // Trouver la première case non vide pour commencer l'exploration
  bool found_start = false;
  for (uint i = 0; i < g->rows && !found_start; i++) {
    for (uint j = 0; j < g->cols && !found_start; j++) {
      if (game_get_piece_shape(g, i, j) != EMPTY) {
        connected[i][j] = true;
        found_start = true;
      }
    }
  }

  // Si aucune case non vide n'a été trouvée, alors le jeu est connecté (aucune pièce à connecter)
  if (!found_start) {
    for (uint i = 0; i < g->rows; i++) {
      free(connected[i]);
    }
    free(connected);
    return true;
  }

  // Propagation des connexions
  bool progress = true;
  while (progress) {
    progress = false;
    for (uint i = 0; i < g->rows; i++) {
      for (uint j = 0; j < g->cols; j++) {
        if (!connected[i][j] || game_get_piece_shape(g, i, j) == EMPTY) {
          continue;
        }

        // Vérification des directions adjacentes
        for (int d = 0; d < NB_DIRS; d++) {
          uint ni = i, nj = j;

          // On utilise la fonction pour obtenir la case voisine en tenant compte du wrapping
          if (game_get_ajacent_square(g, i, j, (direction)d, &ni, &nj)) {
            // Si wrapping est activé, on ajuste les indices pour la grille circulaire
            if (game_is_wrapping_enabled(g)) {
              if (ni >= g->rows) ni = 0;
              if (nj >= g->cols) nj = 0;
              if (ni < 0) ni = g->rows - 1;
              if (nj < 0) nj = g->cols - 1;
            }

            // On vérifie si la connexion est valide
            if (game_check_edge(g, i, j, (direction)d) == MATCH && !connected[ni][nj]) {
              connected[ni][nj] = true;
              progress = true;
            }
          }
        }
      }
    }
  }
  for (uint i = 0; i < g->rows; i++) {
    for (uint j = 0; j < g->cols; j++) {
      if (connected[i][j]) {
        for (int d = 0; d < NB_DIRS; d++) {
          uint ni = i, nj = j;

          if (game_get_ajacent_square(g, i, j, (direction)d, &ni, &nj)) {
            if (game_is_wrapping_enabled(g)) {
              if (ni >= g->rows) ni = 0;
              if (nj >= g->cols) nj = 0;
              if (ni < 0) ni = g->rows - 1;
              if (nj < 0) nj = g->cols - 1;
            }

            if (game_check_edge(g, i, j, (direction)d) == MATCH && !connected[ni][nj]) {
              connected[ni][nj] = true;
              progress = true;
            }
          }
        }
      }
    }
  }

  bool is_connected = true;
  for (uint i = 0; i < g->rows && is_connected; i++) {
    for (uint j = 0; j < g->cols && is_connected; j++) {
      if (game_get_piece_shape(g, i, j) != EMPTY && !connected[i][j]) {
        is_connected = false;
      }
    }
  }

  for (uint i = 0; i < g->rows; i++) {
    free(connected[i]);
  }
  free(connected);

  return is_connected;
}
