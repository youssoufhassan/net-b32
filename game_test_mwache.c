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

bool test_game_default() {
  game g = game_default();
  if (g == NULL) {
    return 1;
  }
  shape expected_shapes[] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,      TEE,      TEE, TEE,
                             TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, TEE,
                             CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};

  direction expected_orientations[] = {WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST,
                                       EAST, EAST,  NORTH, WEST,  WEST,  EAST,  SOUTH, SOUTH, NORTH,
                                       WEST, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH};

  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      int pos = i * DEFAULT_SIZE + j;
      if (game_get_piece_shape(g, i, j) != expected_shapes[pos] &&
          game_get_piece_orientation(g, i, j) != expected_orientations[pos]) {
        game_delete(g);
        return 1;
      }
    }
  }
  game_delete(g);
  return 0;
}

int test_game_default_solution() {
  game g = game_default_solution();
  if (g == NULL) return EXIT_FAILURE;
  shape expected_shapes[DEFAULT_SIZE][DEFAULT_SIZE] = {{CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT},
                                                       {TEE, TEE, TEE, TEE, TEE},
                                                       {ENDPOINT, ENDPOINT, TEE, ENDPOINT, SEGMENT},
                                                       {ENDPOINT, TEE, TEE, CORNER, SEGMENT},
                                                       {ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT}};

  direction expected_orientations[DEFAULT_SIZE][DEFAULT_SIZE] = {{WEST, WEST, EAST, EAST, SOUTH},
                                                                 {EAST, SOUTH, SOUTH, NORTH, WEST},
                                                                 {NORTH, NORTH, EAST, WEST, NORTH},
                                                                 {EAST, SOUTH, NORTH, EAST, NORTH},
                                                                 {EAST, NORTH, WEST, NORTH, NORTH}};

  for (int row = 0; row < g->rows; row++) {
    for (int col = 0; col < g->cols; col++) {
      shape actual_shape = game_get_piece_shape(g, row, col);
      direction actual_orientation = game_get_piece_orientation(g, row, col);

      if (actual_shape != expected_shapes[row][col] && actual_orientation != expected_orientations[row][col]) {
        game_delete(g);
        return EXIT_FAILURE;
      }
    }
  }

  game_delete(g);
  return EXIT_SUCCESS;
}

int test_game_get_ajacent_square(void) {
  uint pi_next, pj_next;
  bool res;

  // Configuration de la grille 4x5 (4 lignes, 5 colonnes)
  uint nb_rows = 4, nb_cols = 5;
  shape shapes[20] = {CORNER,  SEGMENT, TEE, CORNER,  SEGMENT, TEE,     TEE, TEE, TEE,    TEE,
                      SEGMENT, SEGMENT, TEE, SEGMENT, TEE,     SEGMENT, TEE, TEE, CORNER, SEGMENT};

  direction orientations[20] = {NORTH, WEST,  EAST, EAST, SOUTH, WEST, NORTH, NORTH, SOUTH, EAST,
                                NORTH, NORTH, WEST, WEST, EAST,  EAST, NORTH, SOUTH, EAST,  EAST};

  game g_wrap = game_new_ext(nb_rows, nb_cols, shapes, orientations, true);
  game g_no_wrap = game_new_ext(nb_rows, nb_cols, shapes, orientations, false);

  if (g_wrap == NULL || g_no_wrap == NULL) {
    printf("Erreur lors de la création des jeux\n");
    return 1;  // Retourne 1 en cas d'erreur de création du jeu
  }

  // ----- Test avec WRAPPING activé -----
  printf("Test avec WRAPPING activé...\n");

  uint i = 0, j = 0;  // Coin supérieur gauche
  res = game_get_ajacent_square(g_wrap, i, j, EAST, &pi_next, &pj_next);
  if (!(res && pi_next == 0 && pj_next == 1)) {
    game_delete(g_wrap);
    game_delete(g_no_wrap);
    return 1;  // Test échoué pour EAST
  }

  res = game_get_ajacent_square(g_wrap, i, j, SOUTH, &pi_next, &pj_next);
  if (!(res && pi_next == 1 && pj_next == 0)) {
    game_delete(g_wrap);
    game_delete(g_no_wrap);
    return 1;  // Test échoué pour SOUTH
  }

  res = game_get_ajacent_square(g_wrap, i, j, WEST, &pi_next, &pj_next);
  if (!(res && pi_next == 0 && pj_next == nb_cols - 1)) {
    game_delete(g_wrap);
    game_delete(g_no_wrap);
    return 1;  // Test échoué pour WEST
  }

  res = game_get_ajacent_square(g_wrap, i, j, NORTH, &pi_next, &pj_next);
  if (!(res && pi_next == nb_rows - 1 && pj_next == 0)) {
    game_delete(g_wrap);
    game_delete(g_no_wrap);
    return 1;  // Test échoué pour NORTH
  }

  // Coin inférieur droit
  i = nb_rows - 1, j = nb_cols - 1;  // Dernière ligne et dernière colonne
  res = game_get_ajacent_square(g_wrap, i, j, EAST, &pi_next, &pj_next);
  if (!(res && pi_next == nb_rows - 1 && pj_next == 0)) {
    game_delete(g_wrap);
    game_delete(g_no_wrap);
    return 1;  // Test échoué pour EAST dans le coin inférieur droit
  }

  res = game_get_ajacent_square(g_wrap, i, j, SOUTH, &pi_next, &pj_next);
  if (!(res && pi_next == 0 && pj_next == nb_cols - 1)) {
    game_delete(g_wrap);
    game_delete(g_no_wrap);
    return 1;  // Test échoué pour SOUTH dans le coin inférieur droit
  }

  // ----- Test avec WRAPPING désactivé -----
  printf("Test avec WRAPPING désactivé...\n");

  i = 0, j = 0;  // Coin supérieur gauche
  res = game_get_ajacent_square(g_no_wrap, i, j, WEST, &pi_next, &pj_next);
  if (res) {
    game_delete(g_wrap);
    game_delete(g_no_wrap);
    return 1;  // Test échoué pour WEST (hors limites avec wrapping désactivé)
  }

  res = game_get_ajacent_square(g_no_wrap, i, j, NORTH, &pi_next, &pj_next);
  if (res) {
    game_delete(g_wrap);
    game_delete(g_no_wrap);
    return 1;  // Test échoué pour NORTH (hors limites avec wrapping désactivé)
  }

  i = nb_rows - 1, j = nb_cols - 1;  // Coin inférieur droit
  res = game_get_ajacent_square(g_no_wrap, i, j, EAST, &pi_next, &pj_next);
  if (res) {
    game_delete(g_wrap);
    game_delete(g_no_wrap);
    return 1;  // Test échoué pour EAST (hors limites avec wrapping désactivé)
  }

  res = game_get_ajacent_square(g_no_wrap, i, j, SOUTH, &pi_next, &pj_next);
  if (res) {
    game_delete(g_wrap);
    game_delete(g_no_wrap);
    return 1;  // Test échoué pour SOUTH (hors limites avec wrapping désactivé)
  }

  // Déplacement valide
  i = 1, j = 1;  // Position centrale
  res = game_get_ajacent_square(g_no_wrap, i, j, EAST, &pi_next, &pj_next);
  if (!(res && pi_next == 1 && pj_next == 2)) {
    game_delete(g_wrap);
    game_delete(g_no_wrap);
    return 1;  // Test échoué pour EAST au centre
  }

  res = game_get_ajacent_square(g_no_wrap, i, j, SOUTH, &pi_next, &pj_next);
  if (!(res && pi_next == 2 && pj_next == 1)) {
    game_delete(g_wrap);
    game_delete(g_no_wrap);
    return 1;  // Test échoué pour SOUTH au centre
  }

  // Nettoyage
  game_delete(g_wrap);
  game_delete(g_no_wrap);

  printf("Tous les tests ont réussi !\n");
  return 0;  // Retourne 0 si tous les tests passent
}

int test_game_has_half_edge() {
  uint nb_rows = 3, nb_cols = 3;
  shape shapes[9] = {CROSS, EMPTY, SEGMENT, TEE, CROSS, CORNER, EMPTY, SEGMENT, TEE};
  direction orientations[9] = {NORTH, NORTH, NORTH, EAST, SOUTH, WEST, EAST, NORTH, SOUTH};

  game g = game_new_ext(nb_rows, nb_cols, shapes, orientations, false);
  if (g == NULL) {
    return 1;
  }

  for (int i = 0; i < nb_rows; i++) {
    for (int j = 0; j < nb_cols; j++) {
      bool has_any_edge = game_has_half_edge(g, i, j, NORTH) || game_has_half_edge(g, i, j, EAST) ||
                          game_has_half_edge(g, i, j, SOUTH) || game_has_half_edge(g, i, j, WEST);

      shape s = game_get_piece_shape(g, i, j);

      if (s == CROSS) {
        if (!game_has_half_edge(g, i, j, NORTH) || !game_has_half_edge(g, i, j, EAST) ||
            !game_has_half_edge(g, i, j, SOUTH) || !game_has_half_edge(g, i, j, WEST)) {
          game_delete(g);
          return 1;
          continue;
        }

        if ((s == EMPTY && has_any_edge) || (s != EMPTY && !has_any_edge)) {
          game_delete(g);
          return 1;
        }
      }
    }
  }
  game_delete(g);
  return 0;
}

int test_game_check_edge() {
  game g = game_default_solution();
  if (!g) return 1;

  direction directions[] = {NORTH, SOUTH, EAST, WEST};

  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      for (int d = 0; d < 4; d++) {
        edge_status status = game_check_edge(g, i, j, directions[d]);

        if ((i == 0 && directions[d] == NORTH) || (i == DEFAULT_SIZE - 1 && directions[d] == SOUTH) ||
            (j == 0 && directions[d] == WEST) || (j == DEFAULT_SIZE - 1 && directions[d] == EAST)) {
          if (status != NOEDGE) {
            game_delete(g);
            return 1;
          }
        }

        else {
          if (status != MATCH && status != MISMATCH && status != NOEDGE) {
            game_delete(g);
            return 1;
          }
        }
      }
    }
  }

  game_delete(g);
  return 0;
}
int test_game_is_well_paired() {
  game g1 = game_new_empty();
  assert(g1 != NULL);
  if (!game_is_well_paired(g1)) {
    game_delete(g1);
    fprintf(stderr, "test échoué pour empty\n");
    return EXIT_FAILURE;
  }
  game_delete(g1);

  game g2 = game_default();
  assert(g2 != NULL);
  if (game_is_well_paired(g2)) {
    game_delete(g2);
    fprintf(stderr, "test échoué pour default\n");
    return EXIT_FAILURE;
  }
  game_delete(g2);

  game g = game_default_solution();
  assert(g != NULL);
  if (!game_is_well_paired(g)) {
    game_delete(g);
    fprintf(stderr, "test échoué pour default solution\n");
    return EXIT_FAILURE;
  }

  for (int i = 0; i < g->rows; i++) {
    for (int j = 0; j < g->cols; j++) {
      shape actual_schape = game_get_piece_shape(g, i, j);
      game_set_piece_shape(g, i, j, EMPTY);
      if (game_is_well_paired(g)) {
        fprintf(stderr, "ERREUR avec EMPTY\n");
        game_delete(g);
        return EXIT_FAILURE;
      }
      if (actual_schape == SEGMENT) {
        game_set_piece_shape(g, i, j, TEE);
        if (game_is_well_paired(g)) {
          fprintf(stderr, "ERREUR avec TEE\n");
          game_delete(g);
          return EXIT_FAILURE;
        }
      }
      if (actual_schape == TEE) {
        game_set_piece_shape(g, i, j, SEGMENT);
        if (game_is_well_paired(g)) {
          fprintf(stderr, "ERREUR avec SEGMENT\n");
          game_delete(g);
          return EXIT_FAILURE;
        }
      }
      if (actual_schape == ENDPOINT) {
        game_set_piece_shape(g, i, j, TEE);
        if (game_is_well_paired(g)) {
          fprintf(stderr, "ERREUR avec TEE pour ENDPOINT\n");
          game_print(g);
          game_delete(g);
          return EXIT_FAILURE;
        }
      }
      if (actual_schape == CORNER) {
        game_set_piece_shape(g, i, j, SEGMENT);
        if (game_is_well_paired(g)) {
          fprintf(stderr, "ERREUR avec SEGMENT pour CORNER\n");
          game_delete(g);
          return EXIT_FAILURE;
        }
      }
      game_set_piece_shape(g, i, j, actual_schape);
    }
  }
  game_delete(g);

  // ----- Ajout : Test avec WRAPPING activé -----
  printf("Test avec wrapping activé...\n");
  uint nb_rows = 4;
  uint nb_cols = 5;
  shape shapes[20] = {CORNER,  SEGMENT, TEE, CORNER,  SEGMENT, TEE,     TEE, TEE, TEE,    TEE,
                      SEGMENT, SEGMENT, TEE, SEGMENT, TEE,     SEGMENT, TEE, TEE, CORNER, SEGMENT};
  direction orientations[20] = {NORTH, WEST,  EAST, EAST, SOUTH, WEST, NORTH, NORTH, SOUTH, EAST,
                                NORTH, NORTH, WEST, WEST, EAST,  EAST, NORTH, SOUTH, EAST,  EAST};

  // Création du jeu avec wrapping activé
  game g_wrap = game_new_ext(nb_rows, nb_cols, shapes, orientations, true);
  assert(g_wrap != NULL);

  // Vérification initiale : La grille doit être bien appariée
  if (game_is_well_paired(g_wrap)) {
    fprintf(stderr,
            "Test échoué : La grille initiale avec wrapping ne devrait pas "
            "être bien appariée.\n");
    game_delete(g_wrap);
    return EXIT_FAILURE;
  }

  // Parcours des pièces pour tester avec wrapping
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      shape actual_shape = game_get_piece_shape(g_wrap, i, j);

      // Tester avec EMPTY
      game_set_piece_shape(g_wrap, i, j, EMPTY);
      if (game_is_well_paired(g_wrap)) {
        fprintf(stderr, "ERREUR avec EMPTY (wrapping) à (%u, %u)\n", i, j);
        game_delete(g_wrap);
        return EXIT_FAILURE;
      }

      // Tester avec formes incompatibles
      if (actual_shape == SEGMENT) {
        game_set_piece_shape(g_wrap, i, j, TEE);
        if (game_is_well_paired(g_wrap)) {
          fprintf(stderr, "ERREUR avec TEE (wrapping) à (%u, %u)\n", i, j);
          game_delete(g_wrap);
          return EXIT_FAILURE;
        }
      }
      if (actual_shape == TEE) {
        game_set_piece_shape(g_wrap, i, j, SEGMENT);
        if (game_is_well_paired(g_wrap)) {
          fprintf(stderr, "ERREUR avec SEGMENT (wrapping) à (%u, %u)\n", i, j);
          game_delete(g_wrap);
          return EXIT_FAILURE;
        }
      }
      if (actual_shape == CORNER) {
        game_set_piece_shape(g_wrap, i, j, SEGMENT);
        if (game_is_well_paired(g_wrap)) {
          fprintf(stderr, "ERREUR avec SEGMENT pour CORNER (wrapping) à (%u, %u)\n", i, j);
          game_delete(g_wrap);
          return EXIT_FAILURE;
        }
      }

      game_set_piece_shape(g_wrap, i, j, actual_shape);
    }
  }

  game_delete(g_wrap);

  printf("Tous les tests ont réussi, y compris avec wrapping !\n");
  return EXIT_SUCCESS;
}
int test_game_is_connected() {
  // Tests initiaux sans wrapping (ne pas modifier cette partie)
  game g1 = game_new_empty();
  assert(g1 != NULL);
  if (!game_is_connected(g1)) {
    game_delete(g1);
    fprintf(stderr, "test échoué pour empty\n");
    return EXIT_FAILURE;
  }
  game_delete(g1);

  game g3 = game_default();
  assert(g3 != NULL);
  if (game_is_connected(g3)) {
    game_delete(g3);
    fprintf(stderr, "test échoué pour default bizarrement\n");
    return EXIT_FAILURE;
  }
  game_delete(g3);

  game g = game_default_solution();
  assert(g != NULL);
  if (!game_is_connected(g)) {
    game_delete(g);
    fprintf(stderr, "test échoué pour default solution\n");
    return EXIT_FAILURE;
  }

  game_set_piece_shape(g, 1, 2, CORNER);
  if (game_is_connected(g)) {
    game_delete(g);
    fprintf(stderr, "test échoué pour default1\n");
    return EXIT_FAILURE;
  }

  game_set_piece_shape(g, 1, 1, CORNER);
  if (game_is_connected(g)) {
    game_delete(g);
    fprintf(stderr, "test échoué pour default2\n");
    return EXIT_FAILURE;
  }

  game_set_piece_orientation(g, 1, 1, SOUTH);
  game_set_piece_orientation(g, 1, 2, EAST);
  if (game_is_connected(g)) {
    game_delete(g);
    fprintf(stderr, "test échoué pour default3\n");
    return EXIT_FAILURE;
  }
  game_delete(g);

  // ----- Ajout : Test avec WRAPPING activé -----
  printf("Test avec wrapping activé...\n");
  shape expected_shapes[] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,      TEE,      TEE, TEE,
                             TEE,    ENDPOINT, ENDPOINT, TEE,    ENDPOINT, SEGMENT,  ENDPOINT, TEE, TEE,
                             CORNER, SEGMENT,  ENDPOINT, TEE,    ENDPOINT, ENDPOINT, ENDPOINT};
  direction expected_orientations[] = {EAST,  WEST,  EAST,  SOUTH, SOUTH, EAST,  SOUTH, SOUTH, NORTH,
                                       WEST,  NORTH, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH, NORTH,
                                       SOUTH, SOUTH, EAST,  NORTH, WEST,  NORTH, NORTH};
  game g_ad = game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, expected_shapes, expected_orientations,
                           true);  // default solution with wrapping activated
  game_set_piece_orientation(g_ad, 4, 4, EAST);
  if (game_is_connected(g_ad)) {
    fprintf(stderr, "Test1 échoué : La grille modifiée ne devrait pas être connectée.\n");
    game_delete(g_ad);
    return EXIT_FAILURE;
  }
  game_set_piece_orientation(g_ad, 4, 4, NORTH);
  if (!game_is_connected(g_ad)) {
    fprintf(stderr, "Test2 échoué : La grille modifiée devrait  être connectée.\n");
    game_print(g_ad);
    game_delete(g_ad);
    return EXIT_FAILURE;
  }
  game_delete(g_ad);
  game g_ad1 = game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, expected_shapes, expected_orientations,
                            true);  // default solution with wrapping activated
  game_set_piece_shape(g_ad1, 0, 1, CORNER);
  game_set_piece_orientation(g_ad1, 0, 1, WEST);
  game_set_piece_shape(g_ad1, 4, 1, CROSS);
  if (!game_is_connected(g_ad1)) {
    fprintf(stderr,
            "Test3 échoué : La grille modifiée devrait  être connectée car "
            "wrapping activé.\n");
    game_print(g_ad1);
    game_delete(g_ad1);
    return EXIT_FAILURE;
  }
  game_delete(g_ad1);

  shape shapess[4] = {ENDPOINT, ENDPOINT, CORNER, CORNER};
  direction orientationss[4] = {SOUTH, SOUTH, WEST, NORTH};
  game gn = game_new_ext(2, 2, shapess, orientationss, true);
  if (!game_is_connected(gn)) {
    fprintf(stderr,
            "Test6 échoué : La grille modifiée devrait  être connectée car "
            "wrapping activé.\n");
    game_print(gn);
    game_delete(gn);
    return EXIT_FAILURE;
  }
  game_delete(gn);
  printf("Tests réussis : La grille  est connectée avec wrapping.\n");
  return EXIT_SUCCESS;
}

int test_game_new_ext() {
  uint nb_rows = 4;
  uint nb_cols = 5;
  shape shapes[20] = {CORNER,  SEGMENT, TEE, CORNER,  SEGMENT, TEE,     TEE, TEE, TEE,    TEE,
                      SEGMENT, SEGMENT, TEE, SEGMENT, TEE,     SEGMENT, TEE, TEE, CORNER, SEGMENT};

  direction orientations[20] = {NORTH, WEST,  EAST, EAST, SOUTH, WEST, NORTH, NORTH, SOUTH, EAST,
                                NORTH, NORTH, WEST, WEST, EAST,  EAST, NORTH, SOUTH, EAST,  EAST};

  game g = game_new_ext(nb_rows, nb_cols, shapes, orientations, true);
  if (g == NULL) {
    fprintf(stderr, "Erreur : g est null !");
    return 1;
  }
  for (int i = 0; i < g->rows; i++) {
    for (int j = 0; j < g->cols; j++) {
      int index = i * nb_cols + j;
      if (game_get_piece_shape(g, i, j) != shapes[index] ||
          game_get_piece_orientation(g, i, j) != orientations[index]) {
        fprintf(stderr, "Erreur : different shape or orientation  avce true!");
        game_delete(g);
        return 1;
      }
    }
  }
  game_delete(g);
  game g1 = game_new_ext(nb_rows, nb_cols, shapes, orientations, false);
  if (g1 == NULL) {
    fprintf(stderr, "Erreur : g est null !");
    return EXIT_FAILURE;
  }
  for (int i = 0; i < g1->rows; i++) {
    for (int j = 0; j < g1->cols; j++) {
      int index = i * nb_cols + j;

      if (game_get_piece_shape(g1, i, j) != shapes[index] ||
          game_get_piece_orientation(g1, i, j) != orientations[index]) {
        fprintf(stderr, "Erreur : different shape or orientation avec false !");
        game_delete(g1);
        return 1;
      }
    }
  }
  game_delete(g1);

  return 0;
}

int test_game_save() {
  game g = game_default();
  if (g == NULL) {
    fprintf(stderr, "Erreur test_game_save : g has not been initialised properly.\n");
    return EXIT_FAILURE;
  }
  char filename[] = "test_default";
  game_save(g, filename);
  game g_load = game_load("test_default");
  if (!game_equal(g, g_load, false)) {
    fprintf(stderr, "Error test_game_save : the games are not equal after the loading of the saved file.\n");
    game_delete(g);
    game_delete(g_load);
    return 1;
  }
  game_play_move(g, 0, 0, 1);
  if (game_equal(g, g_load, false)) {
    fprintf(stderr,
            "Error test_game_save : the games are equal after the loading of the saved file but they are supposed to "
            "be different.\n");
    game_delete(g);
    game_delete(g_load);
    return 1;
  }
  game_delete(g);
  game_delete(g_load);
  return 0;
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
  if (strcmp("game_default", argv[1]) == 0) {
    ok = test_game_default();
  }
  if (strcmp("game_default_solution", argv[1]) == 0) {
    ok = test_game_default_solution();
  }
  if (strcmp("game_get_ajacent_square", argv[1]) == 0) {
    ok = test_game_get_ajacent_square();
  }
  if (strcmp("game_has_half_edge", argv[1]) == 0) {
    ok = test_game_has_half_edge();
  }
  if (strcmp("game_check_edge", argv[1]) == 0) {
    ok = test_game_check_edge();
  }
  if (strcmp("game_is_well_paired", argv[1]) == 0) {
    ok = test_game_is_well_paired();
  }
  if (strcmp("game_is_connected", argv[1]) == 0) {
    ok = test_game_is_connected();
  }
  if (strcmp("game_new_ext", argv[1]) == 0) {
    ok = test_game_new_ext();
  }
  if (strcmp("game_save", argv[1]) == 0) {
    ok = test_game_save();
  }
  if (ok == 0) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}
