#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"
#include "queue.h"

int main(int argc, char *argv[]) {
  game g = game_new_empty();
  if (argc == 2) {
    g = game_load(argv[1]);
    printf("The game has been loaded.\n");
  } else {
    g = game_default();
  }

  if (g == NULL) {
    fprintf(stderr, "Erreur dans la définition du jeu par défaut.\n");
    return EXIT_FAILURE;
  }

  while (!game_won(g)) {
    game_print(g);
    char c;
    uint i, j;
    char filename[] = "default";
    int ret = scanf(" %c", &c);
    if (ret == 1 && c == 'h') {
      printf("Press and enter 'Q' to quit.\nPress 'R' to shuffle.\n");
      printf("Press and enter 'C <i> <j>' to rotate the piece clockwise in <i> <j>.\n");
      printf("Press and enter 'A <i> <j>' to rotate the piece anti-clockwise in <i> <j>.\n");
      printf("Press and enter 'Z' to undo the last move you did.\n");
      printf("Press and enter 'Y' to redo the last move you have undone.\n");
      printf(
          "Press and enter 'S' followed by the name of the file you want in order to save the game in it's current "
          "state. (ex: s '<filename>')\n");
    }

    if (ret == 1 && (c == 'r' || c == 'R')) {
      game_shuffle_orientation(g);
    }

    if (ret == 1 && (c == 'q' || c == 'Q')) {
      printf("shame\n");
      game_delete(g);
      return EXIT_SUCCESS;
    }
    if (ret == 1 && (c == 'z' || c == 'Z')) {
      game_undo(g);
    }

    if (ret == 1 && (c == 'y' || c == 'Y')) {
      game_redo(g);
    }
    if (ret == 1 && (c == 's' || c == 'S')) {
      int ret2 = scanf(" %s", filename);
      if (ret2 == 1 && (c == 's' || c == 'S')) {
        game_save(g, filename);
        printf("The game has been saved properly !\n");
      } else {
        printf(
            "ERROR: We could not find the name of the file in which you want to save the game, type anything after s "
            "to create a saving file (check example in help pressing h)\n");
      }
    }

    if (((ret == 1) && (c == 'c' || c == 'C')) || ((ret == 1) && (c == 'a' || c == 'A'))) {
      int ret1 = scanf("%d %d", &i, &j);
      if (ret1 == 2 && (c == 'c' || c == 'C')) {
        game_play_move(g, i, j, 1);
      }
      if (ret1 == 2 && (c == 'a' || c == 'A')) {
        game_play_move(g, i, j, -1);
      }
    }
  }
  printf("congratulation\n");
  game_delete(g);
  return EXIT_SUCCESS;
}
