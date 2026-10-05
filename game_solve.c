#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

int main(int argc, char *argv[]) {
  if (argc < 3 || argc > 4) {
    fprintf(stderr, "Error: Invalid number of arguments. Usage: %s [-s|-c] <input_file> [output_file]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Charger le jeu depuis le fichier d'entrée
  game g = game_load(argv[2]);
  if (g == NULL) {
    fprintf(stderr, "Error: Could not load game from file '%s'.\n", argv[2]);
    exit(EXIT_FAILURE);
  }

  // Affichage du jeu
  game_print(g);

  // Si l'option -s (résoudre le jeu)
  if (strcmp(argv[1], "-s") == 0) {
    if (game_solve(g)) {
      // Sauvegarder le jeu résolu dans un fichier
      char *output_filename = (argc == 4) ? argv[3] : "game.solv";
      game_save(g, output_filename);  // Pas besoin de tester la valeur de retour
    } else {
      fprintf(stderr, "Error: Could not solve the game.\n");
      game_delete(g);
      return EXIT_FAILURE;
    }
  }

  // Si l'option -c (compter les solutions)
  if (strcmp(argv[1], "-c") == 0) {
    uint nb_sol = game_nb_solutions(g);

    // Définir le nom du fichier de sortie pour le nombre de solutions
    char *filename = (argc == 4) ? argv[3] : "nb_sol.sol";

    // Ouvrir le fichier pour écrire le nombre de solutions
    FILE *output = fopen(filename, "w");
    if (output == NULL) {
      fprintf(stderr, "Error: Could not open output file '%s' for writing.\n", filename);
      game_delete(g);
      return EXIT_FAILURE;
    }
    fprintf(output, "%u\n", nb_sol);
    fclose(output);  // Fermeture du fichier après l'écriture
  }

  // Libérer les ressources associées au jeu
  game_delete(g);
  return EXIT_SUCCESS;
}
