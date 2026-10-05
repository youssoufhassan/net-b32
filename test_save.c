#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"
#include "queue.h"

int main(int argc, char *argv[]) {
  game g = game_default_solution();
  game_save(g, "jeu_prec");
  game k = game_load("default.txt");
  game_print(k);
  game_delete(g);
  game_delete(k);
}