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
    int min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    bool visible = false;

    if (curr->type == RECTANGLE) {
      min_x = curr->data.rect.x + curr->transform.move_x;
      max_x = min_x + curr->data.rect.width;
      min_y = curr->data.rect.y + curr->transform.move_y;
      max_y = min_y + curr->data.rect.height;
      visible = true;
    } else if (curr->type == CIRCLE) {
      min_x =
          curr->data.circle.cx + curr->transform.move_x - curr->data.circle.r;
      max_x = min_x + (curr->data.circle.r * 2);
      min_y =
          curr->data.circle.cy + curr->transform.move_y - curr->data.circle.r;
      max_y = min_y + (curr->data.circle.r * 2);
      visible = true;
    } else if (curr->type == POLYLINE) {
      if (curr->data.polyline.head != NULL) {
        point_node_t *pt = curr->data.polyline.head;
        // Initialiser avec le premier point
        min_x = pt->p.x + curr->transform.move_x;
        max_x = min_x;
        min_y = pt->p.y + curr->transform.move_y;
        max_y = min_y;

        // Parcourir tous les points pour trouver les min/max
        while (pt != NULL) {
          int px = pt->p.x + curr->transform.move_x;
          int py = pt->p.y + curr->transform.move_y;
          if (px < min_x)
            min_x = px;
          if (px > max_x)
            max_x = px;
          if (py < min_y)
            min_y = py;
          if (py > max_y)
            max_y = py;
          pt = pt->next;
        }
        visible = true;
      }
    } else if (curr->type == POLYGON && curr->data.polygon.head != NULL) {
      point_node_t *pt = curr->data.polygon.head;
      min_x = pt->p.x + curr->transform.move_x;
      max_x = min_x;
      min_y = pt->p.y + curr->transform.move_y;
      max_y = min_y;

      while (pt != NULL) {
        int px = pt->p.x + curr->transform.move_x;
        int py = pt->p.y + curr->transform.move_y;
        if (px < min_x)
          min_x = px;
        if (px > max_x)
          max_x = px;
        if (py < min_y)
          min_y = py;
        if (py > max_y)
          max_y = py;
        pt = pt->next;
      }
      visible = true;
    }
    // Si la forme est totalement en dehors du viewport, on passe à la suivante
    if (visible && (max_x < 0 || min_x > scene->view_width || max_y < 0 ||
                    min_y > scene->view_height)) {
      curr = curr->next;
      continue; // On saute le dessin de cette forme
    }
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
    } else if (curr->type == POLYGON && curr->data.polygon.head != NULL) {
      int scale_x = curr->transform.invert_x ? -1 : 1;
      int scale_y = curr->transform.invert_y ? -1 : 1;

      // Calculer le centroïde pour la rotation
      int sum_x = 0, sum_y = 0;
      size_t count = curr->data.polygon.count;
      point_node_t *pt = curr->data.polygon.head;
      while (pt != NULL) {
        sum_x += pt->p.x;
        sum_y += pt->p.y;
        pt = pt->next;
      }
      int center_x = sum_x / count;
      int center_y = sum_y / count;

      int center_x_final = center_x + curr->transform.move_x;
      int center_y_final = center_y + curr->transform.move_y;

      fprintf(f, "  <polygon points=\"");
      pt = curr->data.polygon.head;
      while (pt != NULL) {
        fprintf(f, "%d,%d ", pt->p.x, pt->p.y);
        pt = pt->next;
      }
      fprintf(f,
              "\" fill=\"%s\" stroke=\"%s\" stroke-width=\"5\" "
              "transform=\"translate(%d %d) rotate(%d) scale(%d %d) "
              "translate(%d %d)\" />\n",
              curr->style.color_fill, curr->style.color_stroke, center_x_final,
              center_y_final, curr->transform.angle, scale_x, scale_y,
              -center_x, -center_y);
    }
    // Les autres if (curr->type == CIRCLE) viendront ici plus tard !
    else if (curr->type == ELLIPSE) {
      int scale_x = curr->transform.invert_x ? -1 : 1;
      int scale_y = curr->transform.invert_y ? -1 : 1;

      // Centre de l'ellipse dans ses coordonnées locales
      int center_x = curr->data.ellipse.cx;
      int center_y = curr->data.ellipse.cy;

      // Centre final après translation
      int center_x_final = center_x + curr->transform.move_x;
      int center_y_final = center_y + curr->transform.move_y;

      fprintf(f,
              "  <ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" "
              "fill=\"%s\" stroke=\"%s\" stroke-width=\"5\" "
              "transform=\"translate(%d %d) rotate(%d) scale(%d %d) "
              "translate(%d %d)\" />\n",
              curr->data.ellipse.cx, curr->data.ellipse.cy,
              curr->data.ellipse.rx, curr->data.ellipse.ry,
              curr->style.color_fill, curr->style.color_stroke, center_x_final,
              center_y_final,        // 4. Replace au centre final
              curr->transform.angle, // 3. Tourne
              scale_x, scale_y,      // 2. Flip
              -center_x, -center_y   // 1. Amène le centre à 0,0
      );

    } else if (curr->type == LINE) {
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
    } else if (curr->type == POLYLINE && curr->data.polyline.head != NULL) {
      int scale_x = curr->transform.invert_x ? -1 : 1;
      int scale_y = curr->transform.invert_y ? -1 : 1;

      // Calculer le centroïde (centre de masse) pour la rotation
      int sum_x = 0, sum_y = 0;
      size_t count = curr->data.polyline.count;
      point_node_t *pt = curr->data.polyline.head;
      while (pt != NULL) {
        sum_x += pt->p.x;
        sum_y += pt->p.y;
        pt = pt->next;
      }
      int center_x = sum_x / count;
      int center_y = sum_y / count;

      int center_x_final = center_x + curr->transform.move_x;
      int center_y_final = center_y + curr->transform.move_y;

      // Écrire les points
      fprintf(f, "  <polyline points=\"");
      pt = curr->data.polyline.head;
      while (pt != NULL) {
        fprintf(f, "%d,%d ", pt->p.x, pt->p.y);
        pt = pt->next;
      }

      // Appliquer les transformations (comme pour le rectangle)
      fprintf(f,
              "\" fill=\"%s\" stroke=\"%s\" stroke-width=\"5\" "
              "transform=\"translate(%d %d) rotate(%d) scale(%d %d) "
              "translate(%d %d)\" />\n",
              curr->style.color_fill, curr->style.color_stroke, center_x_final,
              center_y_final, curr->transform.angle, scale_x, scale_y,
              -center_x, -center_y);
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
