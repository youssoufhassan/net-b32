#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_struct.h"
#include "queue.h"

game game_new_ext(uint nb_rows, uint nb_cols, shape *shapes, direction *orientations, bool wrapping) {
  if (nb_rows == 0 || nb_cols == 0) {
    fprintf(stderr, "Erreur : dimensions du jeu invalides.\n");
    return NULL;
  }

  game nouv = malloc(sizeof(struct game_s));
  if (nouv == NULL) {
    fprintf(stderr, "Erreur : mémoire insuffisante pour le jeu.\n");
    return NULL;
  }

  nouv->rows = nb_rows;
  nouv->cols = nb_cols;
  nouv->wrapping = wrapping;

  nouv->tab_shape = malloc(nb_rows * sizeof(shape *));
  nouv->tab_dir = malloc(nb_rows * sizeof(direction *));
  if (nouv->tab_shape == NULL || nouv->tab_dir == NULL) {
    fprintf(stderr, "Erreur : mémoire insuffisante pour les tableaux.\n");
    free(nouv);
    return NULL;
  }

  for (uint i = 0; i < nb_rows; i++) {
    nouv->tab_shape[i] = malloc(nb_cols * sizeof(shape));
    nouv->tab_dir[i] = malloc(nb_cols * sizeof(direction));
    if (nouv->tab_shape[i] == NULL || nouv->tab_dir[i] == NULL) {
      fprintf(stderr, "Erreur : mémoire insuffisante pour les lignes.\n");
      game_delete(nouv);
      return NULL;
    }
  }

  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      nouv->tab_shape[i][j] = shapes ? shapes[i * nb_cols + j] : EMPTY;
      nouv->tab_dir[i][j] = orientations ? orientations[i * nb_cols + j] : NORTH;
    }
  }

  nouv->undo_moves = queue_new();
  nouv->redo_moves = queue_new();
  nouv->pointers = queue_new();
  if (nouv->undo_moves == NULL || nouv->redo_moves == NULL || nouv->pointers == NULL) {
    fprintf(stderr, "Erreur : mémoire insuffisante pour les files d'attente.\n");
    game_delete(nouv);
    return NULL;
  }

  return nouv;
}
game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping) {
  if (nb_rows == 0 || nb_cols == 0) {
    fprintf(stderr, "Erreur : dimensions du jeu invalides.\n");
    return NULL;
  }

  uint taille = nb_rows * nb_cols;
  shape *shapes = malloc(sizeof(shape) * taille);
  direction *orientations = malloc(sizeof(direction) * taille);

  if (shapes == NULL || orientations == NULL) {
    fprintf(stderr, "Erreur : mémoire insuffisante pour l'initialisation.\n");
    free(shapes);
    free(orientations);
    return NULL;
  }
  for (uint i = 0; i < taille; i++) {
    shapes[i] = EMPTY;
    orientations[i] = NORTH;
  }

  game g = game_new_ext(nb_rows, nb_cols, shapes, orientations, wrapping);

  free(shapes);
  free(orientations);

  return g;
}

uint game_nb_rows(cgame g) {
  assert(g != NULL);
  return g->rows;
}

uint game_nb_cols(cgame g) {
  assert(g != NULL);
  return g->cols;
}

bool game_is_wrapping(cgame g) {
  assert(g != NULL);
  return g->wrapping;
}
void game_undo(game g) {
  if (!queue_is_empty(g->undo_moves)) {
    int *nb_quarters_turns = (int *)queue_pop_head(g->undo_moves);
    uint *j = (uint *)queue_pop_head(g->undo_moves);
    uint *i = (uint *)queue_pop_head(g->undo_moves);
    queue_push_head(g->redo_moves, i);
    queue_push_head(g->redo_moves, j);
    queue_push_head(g->redo_moves, nb_quarters_turns);
    {
      if (g == NULL) {
        fprintf(stderr, "Error game_play_move: game g is empty");
        exit(EXIT_FAILURE);
      }
      if (*i >= g->rows || *i < 0) {
        fprintf(stderr, "Error game_play_move: i is superior to the number of rows or inferior to 0.\n");
        exit(EXIT_FAILURE);
      }
      if (*j >= g->cols || *j < 0) {
        fprintf(stderr, "Error game_play_move: j is superior to the number of cols or inferior to 0.\n");
        exit(EXIT_FAILURE);
      }
      if (g->tab_dir == NULL) {
        fprintf(stderr, "Error game_play_move: tab_dir in game is empty");
        exit(EXIT_FAILURE);
      }
      if (g->tab_dir[*i] == NULL) {
        fprintf(stderr, "Error game_play_move: tab_dir[i] in game is empty");
        exit(EXIT_FAILURE);
      }
      if (g->tab_shape == NULL) {
        fprintf(stderr, "Error game_play_move: tab_shape in game is empty");
        exit(EXIT_FAILURE);
      }
      if (g->tab_shape[*i] == NULL) {
        fprintf(stderr, "Error game_play_move: tab_shape[i] in game is empty");
        exit(EXIT_FAILURE);
      }
      int nb_turns = *nb_quarters_turns * -1;
      int tmp = g->tab_dir[*i][*j];
      tmp = tmp + nb_turns;
      if (tmp >= NB_DIRS) {
        while (tmp >= NB_DIRS) {
          tmp = tmp - NB_DIRS;
        }
        g->tab_dir[*i][*j] = tmp;
      }
      if (tmp < 0) {
        while (tmp < 0) {
          tmp = tmp + NB_DIRS;
        }
        g->tab_dir[*i][*j] = tmp;
      }
      g->tab_dir[*i][*j] = tmp;
    }
  }
}

void game_redo(game g) {
  if (!queue_is_empty(g->redo_moves)) {
    int *nb_quarters_turns = queue_pop_head(g->redo_moves);
    uint *j = queue_pop_head(g->redo_moves);
    uint *i = queue_pop_head(g->redo_moves);
    {
      if (g == NULL) {
        fprintf(stderr, "Error game_play_move: game g is empty");
        exit(EXIT_FAILURE);
      }
      if (*i >= g->rows || *i < 0) {
        fprintf(stderr, "Error game_play_move: i is superior to the number of rows or inferior to 0.\n");
        exit(EXIT_FAILURE);
      }
      if (*j >= g->cols || *j < 0) {
        fprintf(stderr, "Error game_play_move: j is superior to the number of cols or inferior to 0.\n");
        exit(EXIT_FAILURE);
      }
      if (g->tab_dir == NULL) {
        fprintf(stderr, "Error game_play_move: tab_dir in game is empty");
        exit(EXIT_FAILURE);
      }
      if (g->tab_dir[*i] == NULL) {
        fprintf(stderr, "Error game_play_move: tab_dir[i] in game is empty");
        exit(EXIT_FAILURE);
      }
      if (g->tab_shape == NULL) {
        fprintf(stderr, "Error game_play_move: tab_shape in game is empty");
        exit(EXIT_FAILURE);
      }
      if (g->tab_shape[*i] == NULL) {
        fprintf(stderr, "Error game_play_move: tab_shape[i] in game is empty");
        exit(EXIT_FAILURE);
      }
      int nb_turns = *nb_quarters_turns;
      int tmp = g->tab_dir[*i][*j];
      tmp = tmp + nb_turns;
      if (tmp >= NB_DIRS) {
        while (tmp >= NB_DIRS) {
          tmp = tmp - NB_DIRS;
        }
        g->tab_dir[*i][*j] = tmp;
      }
      if (tmp < 0) {
        while (tmp < 0) {
          tmp = tmp + NB_DIRS;
        }
        g->tab_dir[*i][*j] = tmp;
      }
      g->tab_dir[*i][*j] = tmp;
    }
  }
}
