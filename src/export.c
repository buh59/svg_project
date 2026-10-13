#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>

void export_to_svg(scene_t *scene, const char *filename) {
  if (scene == NULL)
    return;

  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("Erreur : impossible de creer le fichier %s\n", filename);
    return;
  }

  // En-tête standard SVG
  fprintf(f,
          "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" "
          "height=\"%d\" viewBox=\"0 0 %d %d\">\n",
          scene->view_width, scene->view_height, scene->view_width,
          scene->view_height);

  // Dessine un fond blanc de la taille exacte de la scène pour mieux voir les
  // limites
  fprintf(
      f,
      "  <rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"white\" />\n",
      scene->view_width, scene->view_height);
  // Parcours de ta liste chaînée
  shape_t *curr = scene->head;
  while (curr != NULL) {
    if (curr->type == RECTANGLE) {
      int scale_x = curr->transform.invert_x ? -1 : 1;
      int scale_y = curr->transform.invert_y ? -1 : 1;

      // Centre de la forme AVANT translation
      int center_x = curr->data.rect.x + (curr->data.rect.width / 2);
      int center_y = curr->data.rect.y + (curr->data.rect.height / 2);

      // Centre de la forme APRÈS translation
      int center_x_final = center_x + curr->transform.move_x;
      int center_y_final = center_y + curr->transform.move_y;

      fprintf(f,
              "  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "
              "fill=\"%s\" stroke=\"%s\" stroke-width=\"5\" "
              "transform=\"translate(%d %d) rotate(%d) translate(%d %d) "
              "scale(%d %d)\" />\n",
              curr->data.rect.x, curr->data.rect.y, curr->data.rect.width,
              curr->data.rect.height, curr->style.color_fill,
              curr->style.color_stroke, center_x_final,
              center_y_final,        // 1. Amener le centre à sa position finale
              curr->transform.angle, // 2. Tourner
              -center_x, -center_y,  // 3. Revenir à l'origine relative
              scale_x, scale_y);
    }
    // Les autres if (curr->type == CIRCLE) viendront ici plus tard !

    curr = curr->next;
  }

  fprintf(f, "</svg>\n");
  fclose(f);

  printf("> Rendu SVG genere dans %s.\n", filename);

  char cmd[100];
  sprintf(cmd, "xdg-open %s &", filename);
  system(cmd);
}
