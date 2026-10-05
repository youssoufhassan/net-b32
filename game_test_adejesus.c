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

int test_dummy() { return EXIT_SUCCESS; }

int test_game_get_piece_shape(void) {
  game g = game_new_empty();

  game_set_piece_shape(g, 0, 0, TEE);
  game_set_piece_shape(g, 1, 1, CORNER);
  game_set_piece_shape(g, 2, 2, ENDPOINT);
  game_set_piece_shape(g, 3, 3, SEGMENT);
  game_set_piece_shape(g, 1, 2, CROSS);
  if (game_get_piece_shape(g, 0, 0) != TEE) {
    game_delete(g);
    return 1;
  }
  if (game_get_piece_shape(g, 1, 1) != CORNER) {
    game_delete(g);
    return 1;
  }
  if (game_get_piece_shape(g, 2, 2) != ENDPOINT) {
    game_delete(g);
    return 1;
  }
  if (game_get_piece_shape(g, 3, 3) != SEGMENT) {
    game_delete(g);
    return 1;
  }

  if (game_get_piece_shape(g, 4, 4) != EMPTY) {
    game_delete(g);
    return 1;
  }

  if (game_get_piece_shape(g, 1, 2) != CROSS) {
    game_delete(g);
    return 1;
  }

  game_delete(g);

  return 0;
}

int test_game_get_piece_orientation() {
  game g = game_new_empty();
  if (g == NULL) {
    fprintf(stderr, "ERREUR!!!!");
    return EXIT_FAILURE;
  }
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      game_set_piece_orientation(g, i, j, NORTH);
      direction d = game_get_piece_orientation(g, i, j);
      if (d != NORTH) {
        game_set_piece_shape(g, 1, 2, CORNER);
        if ((game_is_connected(g))) {
          game_delete(g);
          fprintf(stderr, "test echoué pour default1");
          return EXIT_FAILURE;
        }
        game_set_piece_shape(g, 1, 1, CORNER);
        if ((game_is_connected(g))) {
          game_delete(g);
          fprintf(stderr, "test echoué pour default2");
          return EXIT_FAILURE;
        }
        game_set_piece_orientation(g, 1, 1, SOUTH);
        game_set_piece_orientation(g, 1, 2, EAST);
        if ((game_is_connected(g))) {
          game_delete(g);
          fprintf(stderr, "test echoué pour default3");
          return EXIT_FAILURE;
        }
        game_delete(g);
        fprintf(stderr, "ERREUR!!!!1");
        return EXIT_FAILURE;
      }
      game_set_piece_orientation(g, i, j, SOUTH);
      direction d1 = game_get_piece_orientation(g, i, j);
      if (d1 != SOUTH) {
        game_delete(g);
        fprintf(stderr, "ERREUR!!!!2");
        return EXIT_FAILURE;
      }
      game_set_piece_orientation(g, i, j, EAST);
      direction d2 = game_get_piece_orientation(g, i, j);
      if (d2 != EAST) {
        game_delete(g);
        fprintf(stderr, "ERREUR!!!!3");
        return EXIT_FAILURE;
      }
      game_set_piece_orientation(g, i, j, WEST);
      direction d3 = game_get_piece_orientation(g, i, j);
      if (d3 != WEST) {
        game_delete(g);
        fprintf(stderr, "ERREUR!!!!4");
        return EXIT_FAILURE;
      }
    }
  }
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_play_move(void) {
  game g = game_default_solution();
  if (g == NULL) {
    fprintf(stderr, "ERRREUR!!\n");
    return EXIT_FAILURE;
  }
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      direction d = game_get_piece_orientation(g, i, j);
      for (int k = 3; k < 5; k++) {
        int nouv_k = ((k % 4) + 4) % 4;
        game_play_move(g, i, j, nouv_k);
        direction d1 = game_get_piece_orientation(g, i, j);
        if (d1 != (d + nouv_k) % 4) {
          game_delete(g);
          fprintf(stderr, "ERRREUR!!\n");
          return EXIT_FAILURE;
        }
        d = d1;
      }
    }
  }
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_won() {
  game gi = game_default_solution();
  game g1 = game_default();
  game g2 = game_new_empty();
  game g3 = game_default_solution();
  game g4 = game_default_solution();
  game g5 = game_default_solution();
  game g6 = game_default_solution();

  // Vérification de l'allocation des jeux
  if (g1 == NULL || g2 == NULL || g3 == NULL || g4 == NULL || g5 == NULL || g6 == NULL || gi == NULL) {
    fprintf(stderr, "ERREUR: Un ou plusieurs jeux sont vides.\n");
    // Libération de la mémoire avant la sortie
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(gi);  // Libérer gi ici aussi
    return EXIT_FAILURE;
  }

  // Test sur game_default
  if (game_won(g1) == true || !game_equal(gi, g1, true)) {
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(gi);  // Libérer gi ici aussi
    fprintf(stderr, "Résultat inattendu du test effectué sur game_default\n");
    return EXIT_FAILURE;
  }

  // Test sur game_empty
  if (game_won(g2) == false) {
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(gi);  // Libérer gi ici aussi
    fprintf(stderr, "Résultat inattendu du test effectué sur game_empty\n");
    return EXIT_FAILURE;
  }

  // Test sur game_default_solution
  if (game_won(g3) == false) {
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(gi);  // Libérer gi ici aussi
    fprintf(stderr, "Résultat inattendu du test effectué sur game_default_solution\n");
    return EXIT_FAILURE;
  }

  // Modification de g4 et test
  game_play_move(g4, 2, 1, 2);
  if (game_won(g4) == true) {
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(gi);
    fprintf(stderr, "Résultat inattendu du test après modification de game_default_solution\n");
    return EXIT_FAILURE;
  }

  for (int j = 0; j < DEFAULT_SIZE; j++) {
    game_set_piece_shape(g5, 2, j, EMPTY);
  }
  if (game_won(g5) == true) {
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(gi);
    fprintf(stderr, "Résultat inattendu pour une grille partiellement connectée\n");
    return EXIT_FAILURE;
  }

  for (int i = 0; i < DEFAULT_SIZE; i++) {
    game_play_move(g6, 0, i, 1);
    game_play_move(g6, DEFAULT_SIZE - 1, i, 1);
    game_play_move(g6, i, 0, 2);
    game_play_move(g6, i, DEFAULT_SIZE - 1, 2);
  }

  if (game_won(g6) == true) {
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(gi);
    fprintf(stderr, "Résultat inattendu pour une grille formant une boucle sur les bords\n");
    return EXIT_FAILURE;
  }

  game_play_move(g6, 1, 1, 1);
  game_play_move(g6, 1, 3, 2);
  game_play_move(g6, 3, 1, 1);
  game_play_move(g6, 3, 3, 2);
  if (game_won(g6) == true) {
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(gi);
    fprintf(stderr, "Résultat inattendu pour une grille formant une boucle sur les bords après mouvement\n");
    return EXIT_FAILURE;
  }

  if (!(game_won(gi) && game_is_connected(gi) && game_is_well_paired(gi))) {
    fprintf(stderr, "Conditions injustifiées dans game_default_solution\n");
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(gi);
    return EXIT_FAILURE;
  }

  game g = game_default_solution();
  game_set_piece_shape(g, 1, 2, CORNER);
  if ((game_won(g))) {
    fprintf(stderr, "test échoué pour default1\n");
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(gi);
    return EXIT_FAILURE;
  }

  game_set_piece_shape(g, 1, 1, CORNER);
  if ((game_won(g))) {
    fprintf(stderr, "test échoué pour default2\n");
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(gi);
    return EXIT_FAILURE;
  }

  game_set_piece_orientation(g, 1, 1, SOUTH);
  game_set_piece_orientation(g, 1, 2, EAST);
  if ((game_won(g))) {
    fprintf(stderr, "test échoué pour default3\n");
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(gi);
    return EXIT_FAILURE;
  }

  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  game_delete(g5);
  game_delete(g6);
  game_delete(gi);
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_reset_orientation(void) {
  game g = game_default();
  game_reset_orientation(g);
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_piece_orientation(g, i, j) != 0) {
        game_delete(g);
        return EXIT_FAILURE;
      }
    }
  }
  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_shuffle_orientation(void) {
  game g = game_default();
  if (g == NULL) {
    fprintf(stderr, "erreur");
    return EXIT_FAILURE;
  }
  shape ini[DEFAULT_SIZE][DEFAULT_SIZE];
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      ini[i][j] = game_get_piece_shape(g, i, j);
    }
  }
  int tmp = 3;
  bool change = true;
  direction prece[DEFAULT_SIZE][DEFAULT_SIZE];
  for (int t = 0; t < tmp; t++) {
    if (t > 0) {
      for (int i = 0; i < DEFAULT_SIZE; i++) {
        for (int j = 0; j < DEFAULT_SIZE; j++) {
          prece[i][j] = game_get_piece_orientation(g, i, j);
        }
      }
    }
    game_shuffle_orientation(g);
    for (int i = 0; i < DEFAULT_SIZE; i++) {
      for (int j = 0; j < DEFAULT_SIZE; j++) {
        if (game_get_piece_shape(g, i, j) != ini[i][j]) {
          game_delete(g);
          return EXIT_FAILURE;
        }
      }
    }

    if (t > 0) {
      int nb_change = 0;
      for (int i = 0; i < DEFAULT_SIZE; i++) {
        for (int j = 0; j < DEFAULT_SIZE; j++) {
          if (game_get_piece_orientation(g, i, j) != prece[i][j]) {
            nb_change++;
          }
        }
      }
      if (nb_change == 0) {
        change = false;
        break;
      }
    }
  }
  game_delete(g);
  if (!change) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int test_game_print(void) {
  game g = game_default();
  game_print(g);
  game_delete(g);
  return EXIT_SUCCESS;
}
int test_game_redo(void) {
  game g = game_default();
  game g1 = game_copy(g);
  game g2 = game_copy(g);
  game_play_move(g, 3, 2, 1);
  game_play_move(g2, 3, 2, 1);
  game_undo(g);
  game_redo(g);

  if (game_equal(g, g1, false) || !game_equal(g, g2, false)) {
    game_delete(g);
    game_delete(g1);
    game_delete(g2);
    return EXIT_FAILURE;
  }
  game_delete(g);
  game_delete(g1);
  game_delete(g2);
  return EXIT_SUCCESS;
}

int test_game_load(void) {
  game g = game_default();
  game g2 = game_default();
  game_play_move(g2, 3, 2, 1);

  char file[] = "g_default";
  char file2[] = "g2";
  game_save(g, file);
  game_save(g2, file2);

  game g_loaded = game_load(file);
  game g2_loaded = game_load(file2);

  if (!game_equal(g, g_loaded, false) || !game_equal(g2, g2_loaded, false)) {
    game_delete(g);
    game_delete(g2);
    game_delete(g_loaded);
    game_delete(g2_loaded);
    return EXIT_FAILURE;
  }
  game_delete(g);
  game_delete(g2);
  game_delete(g_loaded);
  game_delete(g2_loaded);
  return EXIT_SUCCESS;
}
int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "number of arguments different than expected (2)\n");
    return EXIT_FAILURE;
  }
  bool ok = false;
  if (strcmp("dummy", argv[1]) == 0) {
    ok = test_dummy();
  }
  if (strcmp("game_get_piece_shape", argv[1]) == 0) {
    ok = test_game_get_piece_shape();
  }
  if (strcmp("game_get_piece_orientation", argv[1]) == 0) {
    ok = test_game_get_piece_orientation();
  }
  if (strcmp("game_play_move", argv[1]) == 0) {
    ok = test_game_play_move();
  }
  if (strcmp("game_won", argv[1]) == 0) {
    ok = test_game_won();
  }
  if (strcmp("game_reset_orientation", argv[1]) == 0) {
    ok = test_game_reset_orientation();
  }
  if (strcmp("game_shuffle_orientation", argv[1]) == 0) {
    ok = test_game_shuffle_orientation();
  }
  if (strcmp("game_print", argv[1]) == 0) {
    ok = test_game_print();
  }
  if (strcmp("game_redo", argv[1]) == 0) {
    ok = test_game_redo();
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
