#include "../include/types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Déclarations externes
extern scene_t *create_scene(int width, int height);
extern void free_scene(scene_t *scene);
extern shape_t *create_rectangle(scene_t *scene, int x, int y, int width,
                                 int height, char *color_stroke,
                                 char *color_fill);
extern void add_shape_to_scene(scene_t *scene, shape_t *shape);
extern void export_to_svg(scene_t *scene, const char *filename);
extern void save_project(scene_t *scene, const char *filename);
extern scene_t *load_project(const char *filename);
extern shape_t *find_shape_by_id(shape_t *current, int target_id);
extern void delete_shape(scene_t *scene, int target_id);
extern shape_t *create_circle(scene_t *scene, int cx, int cy, int r,
                              char *color_stroke, char *color_fill);
extern shape_t *create_line(scene_t *scene, int x1, int y1, int x2, int y2,
                            char *color_stroke, char *color_fill);
extern shape_t *create_ellipse(scene_t *scene, int cx, int cy, int rx, int ry,
                               char *color_stroke, char *color_fill);
extern shape_t *create_polyline(scene_t *scene, char *color_stroke,
                                char *color_fill);
extern bool add_point_to_polyline(shape_t *shape, int x, int y);
extern shape_t *create_polygon(scene_t *scene, char *color_stroke,
                               char *color_fill);
extern bool add_point_to_polygon(shape_t *shape, int x, int y);

void print_scene(scene_t *scene) {
  printf("--- ETAT DE LA SCENE ---\n");
  if (scene->head == NULL) {
    printf("La scene est vide.\n");
    return;
  }
  shape_t *curr = scene->head;
  while (curr != NULL) {
    if (curr->type == RECTANGLE) {
      printf("[ID: %d] RECTANGLE - Pos(%d,%d) Dim(%dx%d) Stroke:%s Fill:%s\n",
             curr->id, curr->data.rect.x, curr->data.rect.y,
             curr->data.rect.width, curr->data.rect.height,
             curr->style.color_stroke, curr->style.color_fill);
    } else if (curr->type == CIRCLE) {
      printf("[ID: %d] CIRCLE - Center(%d,%d) Radius:%d Stroke:%s Fill:%s\n",
             curr->id, curr->data.circle.cx, curr->data.circle.cy,
             curr->data.circle.r, curr->style.color_stroke,
             curr->style.color_fill);
    } else if (curr->type == ELLIPSE) {
      printf(
          "[ID: %d] ELLIPSE - Center(%d,%d) Radii(%d,%d) Stroke:%s Fill:%s\n",
          curr->id, curr->data.ellipse.cx, curr->data.ellipse.cy,
          curr->data.ellipse.rx, curr->data.ellipse.ry,
          curr->style.color_stroke, curr->style.color_fill);
    } else if (curr->type == LINE) {
      printf("[ID: %d] LINE - P1(%d,%d) P2(%d,%d) Stroke:%s Fill:%s\n",
             curr->id, curr->data.line.p1.x, curr->data.line.p1.y,
             curr->data.line.p2.x, curr->data.line.p2.y,
             curr->style.color_stroke, curr->style.color_fill);
    } else if (curr->type == POLYLINE) {
      printf("[ID: %d] POLYLINE - %zu points Stroke:%s Fill:%s\n", curr->id,
             curr->data.polyline.count, curr->style.color_stroke,
             curr->style.color_fill);
    } else if (curr->type == POLYGON) {
      printf("[ID: %d] POLYGON - %zu points Stroke:%s Fill:%s\n", curr->id,
             curr->data.polygon.count, curr->style.color_stroke,
             curr->style.color_fill);
    }
    curr = curr->next;
  }
  printf("------------------------\n");
}

int main(void) {
  char input[256];
  scene_t *scene = create_scene(800, 800);
  if (scene == NULL) {
    printf("Erreur critique: impossible d'initialiser la scene.\n");
    return 1;
  }
  printf("=== EDITEUR SVG (Vertical Slice) ===\n");

  while (true) {
    printf("svg> ");
    if (fgets(input, sizeof(input), stdin) == NULL)
      break;
    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "quit") == 0) {
      break;
    } else if (strcmp(input, "help") == 0) {
      printf("Commandes disponibles :\n");
      printf("  viewport W H\n");
      printf("  create rect X Y W H STROKE FILL\n");
      printf("  delete ID\n");
      printf("  move ID DX DY\n");
      printf("  rotate ID ANGLE\n");
      printf("  flipx ID\n");
      printf("  flipy ID\n");
      printf("  color ID STROKE FILL\n");
      printf("  list\n");
      printf("  display\n");
      printf("  save\n");
      printf("  load\n");
      printf("  quit\n");
    } else if (strcmp(input, "list") == 0) {
      print_scene(scene);
    } else if (strcmp(input, "display") == 0) {
      export_to_svg(scene, "temp_render.svg");
    } else if (strcmp(input, "save") == 0) {
      save_project(scene, "save.mysvg");
    } else if (strcmp(input, "load") == 0) {
      scene_t *loaded = load_project("save.mysvg");
      if (loaded != NULL) {
        free_scene(scene);
        scene = loaded;
        printf("> Scene chargee avec succes.\n");
      }
    } else if (strncmp(input, "create line", 11) == 0) {
      int x1, y1, x2, y2;
      char stroke[10], fill[10];
      if (sscanf(input, "create line %d %d %d %d %9s %9s", &x1, &y1, &x2, &y2,
                 stroke, fill) == 6) {
        shape_t *line = create_line(scene, x1, y1, x2, y2, stroke, fill);
        if (line != NULL) {
          add_shape_to_scene(scene, line);
          printf("> Ligne creee avec l'ID %d.\n", line->id);
        }
      } else {
        printf(
            "> Erreur syntaxe. Usage: create line x1 y1 x2 y2 stroke fill\n");
      }
    } else if (strncmp(input, "create polyline", 15) == 0) {
      char stroke[10], fill[10];
      if (sscanf(input, "create polyline %9s %9s", stroke, fill) == 2) {
        shape_t *poly = create_polyline(scene, stroke, fill);
        if (poly != NULL) {
          add_shape_to_scene(scene, poly);
          printf("> Polyline creee avec l'ID %d. Utilisez 'add_point ID x y' "
                 "pour ajouter des points.\n",
                 poly->id);
        }
      } else {
        printf("> Erreur syntaxe. Usage: create polyline stroke fill\n");
      }
    } else if (strncmp(input, "create polygon", 14) == 0) {
      char stroke[10], fill[10];
      if (sscanf(input, "create polygon %9s %9s", stroke, fill) == 2) {
        shape_t *poly = create_polygon(scene, stroke, fill);
        if (poly != NULL) {
          add_shape_to_scene(scene, poly);
          printf("> Polygon cree avec l'ID %d. Utilisez 'add_point ID x y' "
                 "pour ajouter des points.\n",
                 poly->id);
        }
      } else {
        printf("> Erreur syntaxe. Usage: create polygon stroke fill\n");
      }
    } else if (strncmp(input, "add_point", 9) == 0) {
      int id, x, y;
      if (sscanf(input, "add_point %d %d %d", &id, &x, &y) == 3) {
        shape_t *s = find_shape_by_id(scene->head, id);
        if (s && (s->type == POLYLINE || s->type == POLYGON)) {
          bool success = false;
          if (s->type == POLYLINE) {
            success = add_point_to_polyline(s, x, y);
          } else {
            success = add_point_to_polygon(s, x, y);
          }
          if (success) {
            printf("> Point (%d,%d) ajoute (Total: %zu points).\n", x, y,
                   s->data.polyline.count);
          } else {
            printf("> Erreur lors de l'ajout du point.\n");
          }
        } else {
          printf("> ID %d introuvable ou n'est pas une polyline/polygon.\n",
                 id);
        }
      } else {
        printf("> Erreur syntaxe. Usage: add_point ID x y\n");
      }
    } else if (strncmp(input, "create ellipse", 14) == 0) {
      int cx, cy, rx, ry;
      char stroke[10], fill[10];
      if (sscanf(input, "create ellipse %d %d %d %d %9s %9s", &cx, &cy, &rx,
                 &ry, stroke, fill) == 6) {
        shape_t *ell = create_ellipse(scene, cx, cy, rx, ry, stroke, fill);
        if (ell != NULL) {
          add_shape_to_scene(scene, ell);
          printf("> Ellipse creee avec l'ID %d.\n", ell->id);
        }
      } else {
        printf("> Erreur syntaxe. Usage: create ellipse cx cy rx ry stroke "
               "fill\n");
      }
    } else if (strncmp(input, "create rect", 11) == 0) {
      int x, y, w, h;
      char stroke[10], fill[10];
      if (sscanf(input, "create rect %d %d %d %d %9s %9s", &x, &y, &w, &h,
                 stroke, fill) == 6) {
        shape_t *rect = create_rectangle(scene, x, y, w, h, stroke, fill);
        if (rect != NULL) {
          add_shape_to_scene(scene, rect);
          printf("> Rectangle cree avec l'ID %d.\n", rect->id);
        }
      } else {
        printf("> Erreur syntaxe. Usage: create rect x y w h stroke fill\n");
      }
    } else if (strncmp(input, "create square", 13) == 0) {
      int x, y, size;
      char stroke[10], fill[10];
      // On parse 5 arguments au lieu de 6
      if (sscanf(input, "create square %d %d %d %9s %9s", &x, &y, &size, stroke,
                 fill) == 5) {
        // On appelle create_rectangle avec width = height = size !
        shape_t *sq = create_rectangle(scene, x, y, size, size, stroke, fill);
        if (sq != NULL) {
          add_shape_to_scene(scene, sq);
          printf("> Carre cree avec l'ID %d.\n", sq->id);
        }
      } else {
        printf("> Erreur syntaxe. Usage: create square x y size stroke fill\n");
      }
    } else if (strncmp(input, "create circle", 13) == 0) {
      int cx, cy, r;
      char stroke[10], fill[10];
      if (sscanf(input, "create circle %d %d %d %9s %9s", &cx, &cy, &r, stroke,
                 fill) == 5) {
        shape_t *circ = create_circle(scene, cx, cy, r, stroke, fill);
        if (circ != NULL) {
          add_shape_to_scene(scene, circ);
          printf("> Cercle cree avec l'ID %d.\n", circ->id);
        }
      } else {
        printf("> Erreur syntaxe. Usage: create circle cx cy r stroke fill\n");
      }
    } else if (strncmp(input, "move", 4) == 0) {
      int id, dx, dy;
      if (sscanf(input, "move %d %d %d", &id, &dx, &dy) == 3) {
        shape_t *s = find_shape_by_id(scene->head, id);
        if (s) {
          s->transform.move_x += dx;
          s->transform.move_y += dy;
          printf("> Forme %d deplacee (Total move: %d, %d)\n", id,
                 s->transform.move_x, s->transform.move_y);
        } else {
          printf("> ID %d introuvable.\n", id);
        }
      } else {
        printf("> Erreur syntaxe move.\n");
      }
    } else if (strncmp(input, "rotate", 6) == 0) {
      int id, angle;
      if (sscanf(input, "rotate %d %d", &id, &angle) == 2) {
        shape_t *s = find_shape_by_id(scene->head, id);
        if (s) {
          s->transform.angle = (s->transform.angle + angle) % 360;
          printf("> Forme %d pivotee (Angle total: %d)\n", id,
                 s->transform.angle);
        } else {
          printf("> ID %d introuvable.\n", id);
        }
      } else {
        printf("> Erreur syntaxe rotate.\n");
      }
    } else if (strncmp(input, "flipx", 5) == 0) {
      int id;
      if (sscanf(input, "flipx %d", &id) == 1) {
        shape_t *s = find_shape_by_id(scene->head, id);
        if (s) {
          s->transform.invert_x = !s->transform.invert_x;
          printf("> Forme %d inversee sur l'axe X (etat: %s)\n", id,
                 s->transform.invert_x ? "OUI" : "NON");
        } else {
          printf("> ID %d introuvable.\n", id);
        }
      } else {
        printf("> Erreur syntaxe flipx.\n");
      }
    } else if (strncmp(input, "flipy", 5) == 0) {
      int id;
      if (sscanf(input, "flipy %d", &id) == 1) {
        shape_t *s = find_shape_by_id(scene->head, id);
        if (s) {
          s->transform.invert_y = !s->transform.invert_y;
          printf("> Forme %d inversee sur l'axe Y (etat: %s)\n", id,
                 s->transform.invert_y ? "OUI" : "NON");
        } else {
          printf("> ID %d introuvable.\n", id);
        }
      } else {
        printf("> Erreur syntaxe flipy.\n");
      }
    } else if (strncmp(input, "color", 5) == 0) {
      int id;
      char stroke[10], fill[10];
      if (sscanf(input, "color %d %9s %9s", &id, stroke, fill) == 3) {
        shape_t *s = find_shape_by_id(scene->head, id);
        if (s) {
          strncpy(s->style.color_stroke, stroke, 9);
          s->style.color_stroke[9] = '\0';
          strncpy(s->style.color_fill, fill, 9);
          s->style.color_fill[9] = '\0';
          printf("> Couleurs de la forme %d mises a jour.\n", id);
        } else {
          printf("> ID %d introuvable.\n", id);
        }
      } else {
        printf("> Erreur syntaxe color.\n");
      }
    } else if (strncmp(input, "delete", 6) == 0) {
      int id;
      if (sscanf(input, "delete %d", &id) == 1) {
        delete_shape(scene, id);
      } else {
        printf("> Erreur syntaxe. Usage: delete <id>\n");
      }
    } else if (strncmp(input, "viewport", 8) == 0) {
      int w, h;
      if (sscanf(input, "viewport %d %d", &w, &h) == 2) {
        scene->view_width = w;
        scene->view_height = h;
        printf("> Viewport redimensionne en %dx%d.\n", w, h);
      } else {
        printf("> Erreur syntaxe. Usage: viewport <largeur> <hauteur>\n");
      }
    } else if (strcmp(input, "") == 0) {
      // Entrée vide, on ne fait rien
    } else {
      printf("Commande inconnue. Tapez 'help'.\n");
    }
  }

  free_scene(scene);
  printf("Fermeture propre de l'editeur.\n");
  return 0;
}
