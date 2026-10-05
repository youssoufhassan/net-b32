#include "game.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue.h"

game game_new_empty(void) {
  game g = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false);
  assert(g != NULL);
  return g;
}

game game_new(shape *shapes, direction *orientations) {
  game g = game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, shapes, orientations, false);
  assert(g != NULL);
  return g;
}

game game_copy(cgame g) {
  assert(g != NULL);
  game g_copy = game_new_empty_ext(g->rows, g->cols, g->wrapping);
  assert(g_copy != NULL);

  for (uint i = 0; i < g->rows; i++) {
    for (uint j = 0; j < g->cols; j++) {
      game_set_piece_shape(g_copy, i, j, game_get_piece_shape(g, i, j));
      game_set_piece_orientation(g_copy, i, j, game_get_piece_orientation(g, i, j));
    }
  }

  return g_copy;
}

bool game_equal(cgame g1, cgame g2, bool ignore_orientation) {
  if (g1->rows != g2->rows || g1->cols != g2->cols) {
    return false;
  }
  if (g1->wrapping != g2->wrapping) {
    return false;
  }

  for (int i = 0; i < g1->rows; i++) {
    for (int j = 0; j < g1->cols; j++) {
      if (game_get_piece_shape(g1, i, j) != game_get_piece_shape(g2, i, j)) {
        return false;
      }
    }
  }

  if (ignore_orientation) {
    return true;
  }

  for (int i = 0; i < g1->rows; i++) {
    for (int j = 0; j < g1->cols; j++) {
      if (game_get_piece_orientation(g1, i, j) != game_get_piece_orientation(g2, i, j)) {
        return false;
      }
    }
  }

  return true;
}
void game_delete(game g) {
  if (g != NULL) {
    for (int i = 0; i < g->rows; i++) {
      free(g->tab_shape[i]);
      free(g->tab_dir[i]);
    }

    free(g->tab_shape);
    free(g->tab_dir);

    while (!queue_is_empty(g->pointers)) {
      uint *i = (uint *)queue_pop_head(g->pointers);
      uint *j = (uint *)queue_pop_head(g->pointers);
      int *nb_quarter_turns = (int *)queue_pop_head(g->pointers);
      free(i);
      free(j);
      free(nb_quarter_turns);
    }

    queue_free(g->pointers);
    queue_free(g->undo_moves);
    queue_free(g->redo_moves);

    free(g);
  }
}
void game_set_piece_shape(game g, uint i, uint j, shape s) {
  if (g == NULL) {
    fprintf(stderr, "Error game_set_piece_shape: game g is empty");
    exit(EXIT_FAILURE);
  }
  if (i >= g->rows || i < 0) {
    fprintf(stderr, "Error game_set_piece_shape: i is superior to the DEFAULT_SIZE or inferior to 0");
    exit(EXIT_FAILURE);
  }
  if (j >= g->cols || j < 0) {
    fprintf(stderr, "Error game_set_piece_shape: j is superior to the DEFAULT_SIZE or inferior to 0");
    exit(EXIT_FAILURE);
  }
  if (g->tab_shape == NULL) {
    fprintf(stderr, "Error game_get_piece_shape: tab_shape in game is empty");
    exit(EXIT_FAILURE);
  }
  if (g->tab_shape[i] == NULL) {
    fprintf(stderr, "Error game_get_piece_shape: tab_shape[i] in game is empty");
    exit(EXIT_FAILURE);
  }
  if (s >= NB_SHAPES || s < 0) {
    fprintf(stderr, "Error game_get_piece_shape: s is inferior to 0 or superior or equal to NB_SHAPES");
    exit(EXIT_FAILURE);
  }
  g->tab_shape[i][j] = s;
}

void game_set_piece_orientation(game g, uint i, uint j, direction o) {
  if (g == NULL) {
    fprintf(stderr, "Error game_set_piece_orientation: game g is empty");
    exit(EXIT_FAILURE);
  }
  if (i >= g->rows || i < 0) {
    fprintf(stderr, "Error game_set_piece_orientation: i is superior to the DEFAULT_SIZE or inferior to 0");
    exit(EXIT_FAILURE);
  }
  if (j >= g->cols || j < 0) {
    fprintf(stderr, "Error game_set_piece_orientation: j is superior to the DEFAULT_SIZE or inferior to 0");
    exit(EXIT_FAILURE);
  }
  if (g->tab_dir == NULL) {
    fprintf(stderr, "Error game_get_piece_orientation: tab_dir in game is empty");
    exit(EXIT_FAILURE);
  }
  if (g->tab_dir[i] == NULL) {
    fprintf(stderr, "Error game_get_piece_orientation: tab_dir[i] in game is empty");
    exit(EXIT_FAILURE);
  }
  if (o >= NB_DIRS || o < 0) {
    fprintf(stderr, "Error game_get_piece_orientation: o is inferior to 0 or superior or equal to NB_DIRS");
    exit(EXIT_FAILURE);
  }
  g->tab_dir[i][j] = o;
}

shape game_get_piece_shape(cgame g, uint i, uint j) {
  if (g == NULL) {
    fprintf(stderr, "Error game_get_piece_shape: game g is empty");
    exit(EXIT_FAILURE);
  }
  if (i >= g->rows || i < 0) {
    fprintf(stderr, "Error game_get_piece_shape: i is superior to the DEFAULT_SIZE or inferior to 0");
    exit(EXIT_FAILURE);
  }
  if (j >= g->cols || j < 0) {
    fprintf(stderr, "Error game_get_piece_shape: j is superior to the DEFAULT_SIZE or inferior to 0");
    exit(EXIT_FAILURE);
  }
  if (g->tab_shape == NULL) {
    fprintf(stderr, "Error game_get_piece_shape: tab_shape in game is empty");
    exit(EXIT_FAILURE);
  }
  if (g->tab_shape[i] == NULL) {
    fprintf(stderr, "Error game_get_piece_shape: tab_shape[i] in game is empty");
    exit(EXIT_FAILURE);
  }
  shape s = g->tab_shape[i][j];
  return s;
}

direction game_get_piece_orientation(cgame g, uint i, uint j) {
  if (g == NULL) {
    fprintf(stderr, "Error game_get_piece_orientation: game g is NULL.\n");
    exit(EXIT_FAILURE);
  }
  if (i >= g->rows) {
    fprintf(stderr, "Error game_get_piece_orientation: index i (%u).\n", i);
    exit(EXIT_FAILURE);
  }
  if (j >= g->cols) {
    fprintf(stderr, "Error game_get_piece_orientation: index j (%u) out of bounds.\n", j);
    exit(EXIT_FAILURE);
  }
  if (g->tab_dir == NULL) {
    fprintf(stderr, "Error game_get_piece_orientation: g -> tab_dir undefined, access to unallocated memory.\n");
    exit(EXIT_FAILURE);
  }
  if (g->tab_dir[i] == NULL) {
    fprintf(stderr, "Error game_get_piece_orientation: g -> tab_dir[i] undefined, access to unallocated memory.\n");
    exit(EXIT_FAILURE);
  }
  return g->tab_dir[i][j];
}

void game_play_move(game g, uint i, uint j, int nb_quarter_turns) {
  if (g == NULL) {
    fprintf(stderr, "Error game_play_move: game g is empty");
    exit(EXIT_FAILURE);
  }
  if (i >= g->rows || i < 0) {
    fprintf(stderr, "Error game_play_move: i is superior to the number of rows or inferior to 0.\n");
    exit(EXIT_FAILURE);
  }
  if (j >= g->cols || j < 0) {
    fprintf(stderr, "Error game_play_move: j is superior to the number of cols or inferior to 0.\n");
    exit(EXIT_FAILURE);
  }
  if (g->tab_dir == NULL) {
    fprintf(stderr, "Error game_play_move: tab_dir in game is empty");
    exit(EXIT_FAILURE);
  }
  if (g->tab_dir[i] == NULL) {
    fprintf(stderr, "Error game_play_move: tab_dir[i] in game is empty");
    exit(EXIT_FAILURE);
  }
  if (g->tab_shape == NULL) {
    fprintf(stderr, "Error game_play_move: tab_shape in game is empty");
    exit(EXIT_FAILURE);
  }
  if (g->tab_shape[i] == NULL) {
    fprintf(stderr, "Error game_play_move: tab_shape[i] in game is empty");
    exit(EXIT_FAILURE);
  }
  g->tab_dir[i][j] = g->tab_dir[i][j] + nb_quarter_turns;
  if (g->tab_dir[i][j] >= NB_DIRS) {
    while (g->tab_dir[i][j] >= NB_DIRS) {
      g->tab_dir[i][j] = g->tab_dir[i][j] - NB_DIRS;
    }
  }
  if (g->tab_dir[i][j] < 0) {
    while (g->tab_dir[i][j] < 0) {
      g->tab_dir[i][j] = g->tab_dir[i][j] + NB_DIRS;
    }
  }
  uint *ptr_i = malloc(sizeof(uint));
  uint *ptr_j = malloc(sizeof(uint));
  int *ptr_nb_quarter_turns = malloc(sizeof(int));
  if (ptr_i == NULL || ptr_j == NULL || ptr_nb_quarter_turns == NULL) {
    fprintf(stderr, "Error game_play_move: memory allocation problem.\n");
    exit(EXIT_FAILURE);
  }
  *ptr_i = i;
  *ptr_j = j;
  *ptr_nb_quarter_turns = nb_quarter_turns;
  queue_push_head(g->pointers, ptr_i);
  queue_push_head(g->pointers, ptr_j);
  queue_push_head(g->pointers, ptr_nb_quarter_turns);
  queue_push_head(g->undo_moves, ptr_i);
  queue_push_head(g->undo_moves, ptr_j);
  queue_push_head(g->undo_moves, ptr_nb_quarter_turns);
  queue_clear(g->redo_moves);
}

bool game_won(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Error game_won: game g is NULL\n");
    return false;
  }
  return game_is_well_paired(g) && game_is_connected(g);
}

void game_reset_orientation(game g) {
  if (g == NULL) {
    fprintf(stderr, "Error game_reset_orientation: game g is empty");
    exit(EXIT_FAILURE);
  }
  if (g->tab_dir == NULL) {
    fprintf(stderr, "Error game_reset_orientation: tab_dir in game is empty");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < g->rows; i++) {
    for (int j = 0; j < g->cols; j++) {
      if (g->tab_dir[i] == NULL) {
        fprintf(stderr, "Error game_reset_orientation: tab_dir[i] in game is empty");
        exit(EXIT_FAILURE);
      }
      g->tab_dir[i][j] = NORTH;
    }
  }
}

void game_shuffle_orientation(game g) {
  if (g == NULL) {
    fprintf(stderr, "Error game_shuffle_orientation: game g is empty");
    exit(EXIT_FAILURE);
  }
  if (g->tab_dir == NULL) {
    fprintf(stderr, "Error game_shuffle_orientation: tab_dir in game is empty");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < g->rows; i++) {
    for (int j = 0; j < g->cols; j++) {
      if (g->tab_dir[i] == NULL) {
        fprintf(stderr, "Error game_shuffle_orientation: tab_dir[i] in game is empty");
        exit(EXIT_FAILURE);
      }
      g->tab_dir[i][j] = rand() % 4;
    }
  }
}
