#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"
#include "queue.h"

int main(int argc, char *argv[]) {
  if (argc < 7 || argc > 8) {
    fprintf(stderr, "Error");
    exit(EXIT_FAILURE);
  }
  srand(time(NULL));
  uint nb_rows = atoi(argv[1]);
  uint nb_cols = atoi(argv[2]);
  uint wrapping = atoi(argv[3]);
  uint nb_empty = atoi(argv[4]);
  uint nb_extra = atoi(argv[5]);
  uint shuffle = atoi(argv[6]);
  game g = game_random(nb_rows, nb_cols, wrapping, nb_empty, nb_extra);

  if (shuffle == 1) {
    game_shuffle_orientation(g);
  }
  if (argc == 8) {
    game_save(g, argv[7]);
  }
  game_print(g);
  game_delete(g);
  return EXIT_SUCCESS;
}
