#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"
bool test_game_new() {
  shape shapes[DEFAULT_SIZE * DEFAULT_SIZE] = {CORNER, SEGMENT, TEE,     CORNER, SEGMENT, TEE,     TEE,     TEE, TEE,
                                               TEE,    SEGMENT, SEGMENT, TEE,    SEGMENT, TEE,     SEGMENT, TEE, TEE,
                                               CORNER, SEGMENT, SEGMENT, TEE,    SEGMENT, SEGMENT, SEGMENT};

  direction orientations[DEFAULT_SIZE * DEFAULT_SIZE] = {NORTH, WEST,  EAST,  EAST,  SOUTH, WEST,  NORTH, NORTH, SOUTH,
                                                         EAST,  NORTH, NORTH, WEST,  WEST,  EAST,  EAST,  NORTH, SOUTH,
                                                         EAST,  EAST,  EAST,  SOUTH, WEST,  NORTH, NORTH};

  game g1 = game_new(shapes, orientations);
  if (g1 == NULL) {
    return 1;
  }

  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      uint index = i * DEFAULT_SIZE + j;
      if (game_get_piece_shape(g1, i, j) != shapes[index] ||
          game_get_piece_orientation(g1, i, j) != orientations[index]) {
        game_delete(g1);
        return 1;
      }
    }
  }
  game_delete(g1);

  game g2 = game_new(NULL, NULL);
  if (g2 == NULL) {
    return 1;
  }

  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_piece_shape(g2, i, j) != EMPTY || game_get_piece_orientation(g2, i, j) != NORTH) {
        game_delete(g2);
        return 1;
      }
    }
  }
  game_delete(g2);

  return 0;
}

bool test_game_new_empty() {
  game g = game_new_empty();
  if (g == NULL) {
    return 1;
  }

  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_piece_shape(g, i, j) != EMPTY || game_get_piece_orientation(g, i, j) != NORTH) {
        game_delete(g);
        return 1;
      }
    }
  }

  game_delete(g);

  return 0;
}
int test_game_equal(void) {
  game g = game_new_empty();
  game g1 = game_new_empty();
  if (!game_equal(g1, g, false)) {
    game_delete(g1);
    game_delete(g);
    return 1;
  }
  game_set_piece_orientation(g1, 0, 0, EAST);
  if (game_equal(g1, g, false)) {
    game_delete(g1);
    game_delete(g);
    return 1;
  }
  if (!game_equal(g1, g, true)) {
    game_delete(g1);
    game_delete(g);
    return 1;
  }
  game_delete(g);
  game_delete(g1);

  return 0;
}
int test_game_copy() {
  // Création d'un jeu personnalisé avec une grille rectangulaire et wrapping
  // activé
  uint nb_rows = 5, nb_cols = 7;
  shape shapes[35] = {CROSS,   SEGMENT, TEE,     CORNER,  SEGMENT, TEE,    SEGMENT, TEE,     TEE,
                      SEGMENT, CROSS,   SEGMENT, SEGMENT, CORNER,  EMPTY,  TEE,     TEE,     CROSS,
                      SEGMENT, EMPTY,   CORNER,  CROSS,   SEGMENT, TEE,    EMPTY,   SEGMENT, SEGMENT,
                      CROSS,   SEGMENT, EMPTY,   EMPTY,   SEGMENT, CORNER, TEE,     SEGMENT};
  direction orientations[35] = {NORTH, WEST,  EAST,  SOUTH, NORTH, EAST,  WEST,  NORTH, SOUTH, WEST,  NORTH, EAST,
                                SOUTH, EAST,  NORTH, SOUTH, EAST,  WEST,  NORTH, SOUTH, WEST,  NORTH, EAST,  SOUTH,
                                EAST,  NORTH, WEST,  EAST,  WEST,  SOUTH, EAST,  WEST,  NORTH, EAST,  NORTH};

  game g = game_new_ext(nb_rows, nb_cols, shapes, orientations, true);
  assert(g != NULL);

  game g1 = game_copy(g);
  if (g1 == NULL) {
    game_delete(g);
    fprintf(stderr, "ERREUR !!!! Impossible de copier le jeu.\n");
    return EXIT_FAILURE;
  }

  if (g->cols != g1->cols || g->rows != g1->rows || g->wrapping != g1->wrapping) {
    fprintf(stderr,
            "Résultat inattendu : Les dimensions ou les propriétés wrapping ne "
            "correspondent pas.\n");
    game_delete(g);
    game_delete(g1);
    return EXIT_FAILURE;
  }

  // Vérification de l'égalité des jeux
  if (!game_equal(g, g1, false)) {
    fprintf(stderr, "Résultat inattendu : Les jeux ne sont pas égaux après la copie.\n");
    game_delete(g);
    game_delete(g1);
    return EXIT_FAILURE;
  }

  // Modification du jeu copié
  game_play_move(g1, 0, 0, 1);
  if (game_equal(g, g1, false)) {
    fprintf(stderr, "Résultat inattendu : Les jeux sont égaux après modification de g1.\n");
    game_delete(g);
    game_delete(g1);
    return EXIT_FAILURE;
  }

  // Vérification de la copie de g1
  game g2 = game_copy(g1);
  if (g2 == NULL) {
    fprintf(stderr, "ERREUR !!!! Impossible de copier g1.\n");
    game_delete(g);
    game_delete(g1);
    return EXIT_FAILURE;
  }

  // Modification de g2 et vérification
  game_play_move(g2, 0, 0, -1);  // Revenir à l'état précédent pour g2
  if (!game_equal(g, g2, false)) {
    fprintf(stderr, "Résultat inattendu : g2 ne correspond pas à g après modification.\n");
    game_delete(g);
    game_delete(g1);
    game_delete(g2);
    return EXIT_FAILURE;
  }

  game_delete(g);
  game_delete(g1);
  game_delete(g2);

  printf("Test réussi pour game_copy avec les nouvelles propriétés.\n");
  return EXIT_SUCCESS;
}
int test_game_delete(void) {
  game g = game_default();
  if (g == NULL) {
    return 1;
  }
  game_delete(g);
  return g == NULL;
}

int test_game_set_piece_shape(void) {
  game g = game_new_empty();
  shape def = game_get_piece_shape(g, 0, 0);
  if (def != EMPTY) {
    game_delete(g);
    return 1;
  }
  game_set_piece_shape(g, 0, 0, SEGMENT);
  shape nouv = game_get_piece_shape(g, 0, 0);
  if (nouv != SEGMENT) {
    game_delete(g);
    return 1;
  }
  game_delete(g);
  return 0;
}
int test_game_set_piece_orientation(void) {
  game g = game_new_empty();
  if (g == NULL) {
    game_delete(g);
    return 1;
  }
  direction d = game_get_piece_orientation(g, 0, 0);
  if (d != NORTH) {
    game_delete(g);
    return 1;
  }
  game_set_piece_orientation(g, 0, 0, WEST);
  direction nouv_d = game_get_piece_orientation(g, 0, 0);
  if (nouv_d != WEST) {
    game_delete(g);
    return 1;
  }
  game_delete(g);
  return 0;
}
int test_dummy() {
  printf("test_dummy exécuté avec succès.\n");
  return EXIT_SUCCESS;
}
int test_game_nb_rows() {
  game g = game_new_empty_ext(4, 3, false);
  if (g == NULL) {
    return EXIT_FAILURE;
  }

  uint rows = game_nb_rows(g);
  if (rows != 4) {
    fprintf(stderr, "Expected 4 rows, got %d\n", rows);
    game_delete(g);
    return EXIT_FAILURE;
  }

  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_nb_cols() {
  game g = game_new_empty_ext(4, 3, false);
  if (g == NULL) {
    return EXIT_FAILURE;
  }

  uint cols = game_nb_cols(g);
  if (cols != 3) {
    fprintf(stderr, "Expected 3 columns, got %d\n", cols);
    game_delete(g);
    return EXIT_FAILURE;
  }

  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_is_wrapping() {
  game g = game_new_empty_ext(4, 3, true);
  if (g == NULL) {
    return EXIT_FAILURE;
  }

  if (!game_is_wrapping(g)) {
    fprintf(stderr, "Expected wrapping to be true\n");
    game_delete(g);
    return EXIT_FAILURE;
  }

  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_new_empty_ext() {
  game g = game_new_empty_ext(8, 7, true);
  if (g == NULL) {
    return EXIT_FAILURE;
  }
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      if (game_get_piece_shape(g, i, j) != EMPTY || game_get_piece_orientation(g, i, j) != NORTH) {
        game_delete(g);
        return EXIT_FAILURE;
      }
    }
  }
  game_delete(g);
  return EXIT_SUCCESS;
}
int test_hyoussouf(const char *testname) {
  if (strcmp(testname, "dummy") == 0) {
    return test_dummy();
  } else {
    return EXIT_FAILURE;
  }
}
int test_game_undo() {
  game g = game_default();
  assert(g != NULL);
  for (int i = 0; i < g->cols; i++) {
    for (int j = 0; j < g->rows; j++) {
      for (int k = 0; k < NB_DIRS; k++) {
        direction dir_prev = game_get_piece_orientation(g, i, j);
        game_play_move(g, i, j, k);
        game_undo(g);
        direction actual_dir = game_get_piece_orientation(g, i, j);
        if (dir_prev != actual_dir) {
          fprintf(stderr, "Error dir_prev %u different elt in redo_stack %u", dir_prev, actual_dir);
          game_delete(g);
          return EXIT_FAILURE;
        }
      }
    }
  }
  direction actual_dir1 = game_get_piece_orientation(g, 0, 1);
  game_play_move(g, 0, 1, 1);  // Effectue une rotation horaire
  direction actual_dir2 = game_get_piece_orientation(g, 1, 0);
  game_play_move(g, 1, 0, 2);
  direction actual_dir3 = game_get_piece_orientation(g, 2, 1);
  game_play_move(g, 2, 1, 3);
  direction actual_dir4 = game_get_piece_orientation(g, 2, 2);
  game_play_move(g, 2, 2, 2);
  direction actual_dir5 = game_get_piece_orientation(g, 0, 3);
  game_play_move(g, 0, 3, 1);
  direction actual_dir6 = game_get_piece_orientation(g, 1, 2);
  game_play_move(g, 1, 2, -1);

  game_undo(g);
  game_undo(g);
  game_undo(g);
  game_undo(g);
  game_undo(g);
  game_undo(g);

  direction dir_prev6 = game_get_piece_orientation(g, 1, 2);
  if (actual_dir6 != dir_prev6) {
    fprintf(stderr, "Error: dir_prev6 %u != actual_dir6 %u  ", dir_prev6, actual_dir6);
    game_delete(g);
    return EXIT_FAILURE;
  }
  direction dir_prev5 = game_get_piece_orientation(g, 0, 3);
  if (actual_dir5 != dir_prev5) {
    fprintf(stderr, "Error: dir_prev5 %u != actual_dir5 %u  ", dir_prev5, actual_dir5);
    game_delete(g);
    return EXIT_FAILURE;
  }
  direction dir_prev4 = game_get_piece_orientation(g, 2, 2);
  if (actual_dir4 != dir_prev4) {
    fprintf(stderr, "Error: dir_prev4 %u != actual_dir4 %u  ", dir_prev4, actual_dir4);
    game_delete(g);
    return EXIT_FAILURE;
  }
  direction dir_prev3 = game_get_piece_orientation(g, 2, 1);
  if (actual_dir3 != dir_prev3) {
    fprintf(stderr, "Error: dir_prev3 %u != actual_dir3 %u  ", dir_prev3, actual_dir3);
    game_delete(g);
    return EXIT_FAILURE;
  }
  direction dir_prev2 = game_get_piece_orientation(g, 1, 0);
  if (actual_dir2 != dir_prev2) {
    fprintf(stderr, "Error: dir_prev2 %u != actual_dir2 %u  ", dir_prev2, actual_dir2);
    game_delete(g);
    return EXIT_FAILURE;
  }
  direction dir_prev1 = game_get_piece_orientation(g, 0, 1);
  if (actual_dir1 != dir_prev1) {
    fprintf(stderr, "Error: dir_prev1 %u != actual_dir1 %u  ", dir_prev1, actual_dir1);
    game_delete(g);
    return EXIT_FAILURE;
  }

  game_delete(g);
  return EXIT_SUCCESS;
}
int test_game_load() {
  game g = game_default_solution();
  if (g == NULL) {
    fprintf(stderr, "Failed to create default game.\n");
    return EXIT_FAILURE;
  }

  game_save(g, "jeu_prec.txt");
  game k = game_load("jeu_prec.txt");
  if (k == NULL) {
    fprintf(stderr, "Failed to load game.\n");
    game_delete(g);
    return EXIT_FAILURE;
  }

  bool gagne = game_won(k) && game_equal(g, k, false);

  game_delete(g);
  game_delete(k);

  return gagne ? EXIT_SUCCESS : EXIT_FAILURE;
}
int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Erreur : aucun argument passé.\n");
    return EXIT_FAILURE;
  }
  int ok = 1;
  if (strcmp("dummy", argv[1]) == 0) {
    ok = test_dummy();
  }
  if (strcmp("game_new_empty", argv[1]) == 0) {
    ok = test_game_new_empty();
  }
  if (strcmp("game_new_empty_ext", argv[1]) == 0) {
    ok = test_game_new_empty_ext();
  }
  if (strcmp("game_new", argv[1]) == 0) {
    ok = test_game_new();
  }
  if (strcmp("game_copy", argv[1]) == 0) {
    ok = test_game_copy();
  }
  if (strcmp("game_equal", argv[1]) == 0) {
    ok = test_game_equal();
  }
  if (strcmp("game_delete", argv[1]) == 0) {
    ok = test_game_delete();
  }
  if (strcmp("game_set_piece_shape", argv[1]) == 0) {
    ok = test_game_set_piece_shape();
  }
  if (strcmp("game_set_piece_orientation", argv[1]) == 0) {
    ok = test_game_set_piece_orientation();
  }
  if (strcmp("game_nb_rows", argv[1]) == 0) {
    ok = test_game_nb_rows();
  }

  if (strcmp("game_nb_cols", argv[1]) == 0) {
    ok = test_game_nb_cols();
  }
  if (strcmp("game_undo", argv[1]) == 0) {
    ok = test_game_undo();
  }
  if (strcmp("game_is_wrapping", argv[1]) == 0) {
    ok = test_game_is_wrapping();
  }
  if (strcmp("game_load", argv[1]) == 0) {
    ok = test_game_load();
  }
  if (ok == 0) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}