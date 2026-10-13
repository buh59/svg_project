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

      // 1. Calcul du centre de la forme dans ses coordonnées locales (avant
      // move)
      int center_x_local = curr->data.rect.x + (curr->data.rect.width / 2);
      int center_y_local = curr->data.rect.y + (curr->data.rect.height / 2);

      // 2. Calcul du centre final (après le déplacement)
      int center_x_final = center_x_local + curr->transform.move_x;
      int center_y_final = center_y_local + curr->transform.move_y;

      // En SVG, les transformations s'appliquent de la DROITE vers la GAUCHE.
      // L'ordre est donc : translate(local) -> scale -> rotate ->
      // translate(final)
      fprintf(f,
              "  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "
              "fill=\"%s\" stroke=\"%s\" stroke-width=\"5\" "
              "transform=\"translate(%d %d) rotate(%d) scale(%d %d) "
              "translate(%d %d)\" />\n",
              curr->data.rect.x, curr->data.rect.y, curr->data.rect.width,
              curr->data.rect.height, curr->style.color_fill,
              curr->style.color_stroke, center_x_final,
              center_y_final,                  // 4. On replace au centre final
              curr->transform.angle,           // 3. On tourne
              scale_x, scale_y,                // 2. On flip (autour de 0,0)
              -center_x_local, -center_y_local // 1. On amène le centre à 0,0
      );
    }
    // Les autres if (curr->type == CIRCLE) viendront ici plus tard !
    else if (curr->type == LINE) {
      int scale_x = curr->transform.invert_x ? -1 : 1;
      int scale_y = curr->transform.invert_y ? -1 : 1;

      // Calcul du point médian (centre de la ligne)
      int mid_x = (curr->data.line.p1.x + curr->data.line.p2.x) / 2;
      int mid_y = (curr->data.line.p1.y + curr->data.line.p2.y) / 2;

      int mid_x_final = mid_x + curr->transform.move_x;
      int mid_y_final = mid_y + curr->transform.move_y;

      fprintf(f,
              "  <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" "
              "stroke=\"%s\" stroke-width=\"5\" "
              "transform=\"translate(%d %d) rotate(%d) scale(%d %d) "
              "translate(%d %d)\" />\n",
              curr->data.line.p1.x, curr->data.line.p1.y, curr->data.line.p2.x,
              curr->data.line.p2.y, curr->style.color_stroke, mid_x_final,
              mid_y_final,           // 4. Replace au centre final
              curr->transform.angle, // 3. Tourne
              scale_x, scale_y,      // 2. Flip
              -mid_x, -mid_y         // 1. Amène le centre à 0,0
      );
    } else if (curr->type == CIRCLE) {
      int scale_x = curr->transform.invert_x ? -1 : 1;
      int scale_y = curr->transform.invert_y ? -1 : 1;

      fprintf(f,
              "  <circle cx=\"%d\" cy=\"%d\" r=\"%d\" fill=\"%s\" "
              "stroke=\"%s\" stroke-width=\"5\" transform=\"translate(%d %d) "
              "rotate(%d) scale(%d %d)\" />\n",
              curr->data.circle.cx, curr->data.circle.cy, curr->data.circle.r,
              curr->style.color_fill, curr->style.color_stroke,
              curr->transform.move_x, curr->transform.move_y,
              curr->transform.angle, scale_x, scale_y);
    }

    curr = curr->next;
  }

  fprintf(f, "</svg>\n");
  fclose(f);

  printf("> Rendu SVG genere dans %s.\n", filename);

  char cmd[100];
  sprintf(cmd, "xdg-open %s &", filename);
  system(cmd);
}
