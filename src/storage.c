#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Déclarations externes nécessaires
extern shape_t *create_rectangle(scene_t *scene, int x, int y, int width,
                                 int height, char *color_stroke,
                                 char *color_fill);
extern void add_shape_to_scene(scene_t *scene, shape_t *shape);
extern scene_t *create_scene(int width, int height);

void save_project(scene_t *scene, const char *filename) {
  if (scene == NULL)
    return;

  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("Erreur : impossible d'ouvrir %s pour la sauvegarde.\n", filename);
    return;
  }

  // 1. Sauvegarde de la scène
  fprintf(f, "SCENE %d %d %d\n", scene->view_width, scene->view_height,
          scene->next_id);

  // 2. Sauvegarde des formes
  shape_t *curr = scene->head;
  while (curr != NULL) {
    if (curr->type == RECTANGLE) {
      // Si une couleur est vide, on écrit "none" pour que sscanf puisse la lire
      char stroke[20] = "none";
      char fill[20] = "none";
      if (curr->style.color_stroke[0] != '\0')
        strcpy(stroke, curr->style.color_stroke);
      if (curr->style.color_fill[0] != '\0')
        strcpy(fill, curr->style.color_fill);

      fprintf(f, "RECT %d %d %d %d %s %s %d %d %d %d %d\n", curr->data.rect.x,
              curr->data.rect.y, curr->data.rect.width, curr->data.rect.height,
              stroke, fill, curr->transform.move_x, curr->transform.move_y,
              curr->transform.angle, curr->transform.invert_x,
              curr->transform.invert_y);
    }
    curr = curr->next;
  }

  fclose(f);
  printf("> Projet sauvegarde avec succes dans '%s'.\n", filename);
}

// Le chargement renvoie un NOUVEAU pointeur scene_t
scene_t *load_project(const char *filename) {
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    printf("Erreur : impossible de lire %s.\n", filename);
    return NULL;
  }

  char line[256];
  scene_t *new_scene = NULL;

  while (fgets(line, sizeof(line), f)) {
    if (strncmp(line, "SCENE", 5) == 0) {
      int w, h, next_id;
      if (sscanf(line, "SCENE %d %d %d", &w, &h, &next_id) == 3) {
        new_scene = create_scene(w, h);
        if (new_scene)
          new_scene->next_id = next_id; // On restaure l'ID exact !
      }
    } else if (strncmp(line, "RECT", 4) == 0 && new_scene != NULL) {
      int x, y, w, h, mx, my, angle, ix, iy;
      char stroke[20], fill[20];

      if (sscanf(line, "RECT %d %d %d %d %19s %19s %d %d %d %d %d", &x, &y, &w,
                 &h, stroke, fill, &mx, &my, &angle, &ix, &iy) == 11) {

        // Restauration des couleurs vides
        if (strcmp(stroke, "none") == 0)
          stroke[0] = '\0';
        if (strcmp(fill, "none") == 0)
          fill[0] = '\0';

        shape_t *rect = create_rectangle(new_scene, x, y, w, h, stroke, fill);
        if (rect != NULL) {
          rect->transform.move_x = mx;
          rect->transform.move_y = my;
          rect->transform.angle = angle;
          rect->transform.invert_x = ix;
          rect->transform.invert_y = iy;
          add_shape_to_scene(new_scene, rect);
        }
      }
    }
  }

  fclose(f);
  printf("> Projet charge avec succes depuis '%s'.\n", filename);
  return new_scene;
}
