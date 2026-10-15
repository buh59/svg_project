#include "../include/types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
extern shape_t *create_group(scene_t *scene);
extern void add_shape_to_group(shape_t *group, shape_t *child);
extern void detach_shape(shape_t *shape, scene_t *scene);
extern shape_t *create_group_with(scene_t *scene, int *ids, int count);
extern void remove_from_group(scene_t *scene, int target_id);

bool is_ancestor(shape_t *potential_ancestor, shape_t *shape) {
  shape_t *curr = shape->parent;
  while (curr != NULL) {
    if (curr == potential_ancestor)
      return true;
    curr = curr->parent;
  }
  return false;
}

void print_recursive(shape_t *node, int depth) {
  if (node == NULL)
    return;

  for (int i = 0; i < depth; i++)
    printf("  ");

  if (node->type == RECTANGLE) {
    printf("[ID: %d] RECTANGLE - Pos(%d,%d) Dim(%dx%d) Stroke:%s Fill:%s\n",
           node->id, node->data.rect.x, node->data.rect.y,
           node->data.rect.width, node->data.rect.height,
           node->style.color_stroke, node->style.color_fill);
  } else if (node->type == CIRCLE) {
    printf("[ID: %d] CIRCLE - Center(%d,%d) Radius:%d Stroke:%s Fill:%s\n",
           node->id, node->data.circle.cx, node->data.circle.cy,
           node->data.circle.r, node->style.color_stroke,
           node->style.color_fill);
  } else if (node->type == ELLIPSE) {
    printf("[ID: %d] ELLIPSE - Center(%d,%d) Radii(%d,%d) Stroke:%s Fill:%s\n",
           node->id, node->data.ellipse.cx, node->data.ellipse.cy,
           node->data.ellipse.rx, node->data.ellipse.ry,
           node->style.color_stroke, node->style.color_fill);
  } else if (node->type == LINE) {
    printf("[ID: %d] LINE - P1(%d,%d) P2(%d,%d) Stroke:%s Fill:%s\n", node->id,
           node->data.line.p1.x, node->data.line.p1.y, node->data.line.p2.x,
           node->data.line.p2.y, node->style.color_stroke,
           node->style.color_fill);
  } else if (node->type == POLYLINE) {
    printf("[ID: %d] POLYLINE - %zu points Stroke:%s Fill:%s\n", node->id,
           node->data.polyline.count, node->style.color_stroke,
           node->style.color_fill);
  } else if (node->type == POLYGON) {
    printf("[ID: %d] POLYGON - %zu points Stroke:%s Fill:%s\n", node->id,
           node->data.polygon.count, node->style.color_stroke,
           node->style.color_fill);
  } else if (node->type == GROUP) {
    printf("[ID: %d] GROUPE\n", node->id);
    print_recursive(node->data.group.head, depth + 1);
  }

  print_recursive(node->next, depth);
}

void print_scene(scene_t *scene) {
  printf("--- ARBORESCENCE DE LA SCENE ---\n");
  if (scene->head == NULL)
    printf("La scene est vide.\n");
  else
    print_recursive(scene->head, 0);
  printf("--------------------------------\n");
}

int main(void) {
  char input[256];
  scene_t *scene = create_scene(800, 800);
  if (scene == NULL) {
    printf("Erreur critique: impossible d'initialiser la scene.\n");
    return 1;
  }
  printf("=== EDITEUR SVG (Version Protegee) ===\n");

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
      printf("  create rect X Y W H [STROKE] [FILL]\n");
      printf("  create circle CX CY R [STROKE] [FILL]\n");
      printf("  create ellipse CX CY RX RY [STROKE] [FILL]\n");
      printf("  create line X1 Y1 X2 Y2 [STROKE] [FILL]\n");
      printf("  create polyline [STROKE] [FILL]\n");
      printf("  create polygon [STROKE] [FILL]\n");
      printf("  add_point ID X Y\n");
      printf("  create group\n");
      printf("  add_to_group ID_SHAPE ID_GROUP\n");
      printf("  remove_from_group ID\n");
      printf("  delete ID\n");
      printf("  move ID DX DY\n");
      printf("  rotate ID ANGLE\n");
      printf("  flipx ID\n");
      printf("  flipy ID\n");
      printf("  color ID STROKE FILL\n");
      printf("  list\n");
      printf("  display\n");
      printf("  save FILENAME\n");
      printf("  load FILENAME\n");
      printf("  quit\n");
    } else if (strcmp(input, "list") == 0) {
      print_scene(scene);
    } else if (strcmp(input, "display") == 0) {
      export_to_svg(scene, "temp_render.svg");
    } else if (strncmp(input, "save", 4) == 0) {
      char filename[100];
      if (sscanf(input, "save %99s", filename) == 1) {
        save_project(scene, filename);
      } else {
        printf("> Erreur : Veuillez preciser un nom (ex: save projet.svg)\n");
      }
    } else if (strncmp(input, "load", 4) == 0) {
      char filename[100];
      if (sscanf(input, "load %99s", filename) == 1) {
        scene_t *loaded = load_project(filename);
        if (loaded != NULL) {
          free_scene(scene);
          scene = loaded;
          printf("> Scene chargee avec succes.\n");
        }
      } else {
        printf("> Erreur : Veuillez preciser un nom (ex: load projet.svg)\n");
      }
    } else if (strncmp(input, "create rect", 11) == 0) {
      int x = 0, y = 0, w = 50, h = 50;
      char stroke[32] = "black", fill[32] = "none";
      if (sscanf(input, "create rect %d %d %d %d %31s %31s", &x, &y, &w, &h,
                 stroke, fill) >= 4) {
        shape_t *rect = create_rectangle(scene, x, y, w, h, stroke, fill);
        if (rect != NULL) {
          add_shape_to_scene(scene, rect);
          printf("> Rectangle cree avec l'ID %d.\n", rect->id);
        }
      } else {
        printf(
            "> Erreur syntaxe. Usage: create rect x y w h [stroke] [fill]\n");
      }
    } else if (strncmp(input, "create circle", 13) == 0) {
      int cx = 0, cy = 0, r = 25;
      char stroke[32] = "black", fill[32] = "none";
      if (sscanf(input, "create circle %d %d %d %31s %31s", &cx, &cy, &r,
                 stroke, fill) >= 3) {
        shape_t *circ = create_circle(scene, cx, cy, r, stroke, fill);
        if (circ != NULL) {
          add_shape_to_scene(scene, circ);
          printf("> Cercle cree avec l'ID %d.\n", circ->id);
        }
      } else {
        printf(
            "> Erreur syntaxe. Usage: create circle cx cy r [stroke] [fill]\n");
      }
    } else if (strncmp(input, "create ellipse", 14) == 0) {
      int cx = 0, cy = 0, rx = 30, ry = 20;
      char stroke[32] = "black", fill[32] = "none";
      if (sscanf(input, "create ellipse %d %d %d %d %31s %31s", &cx, &cy, &rx,
                 &ry, stroke, fill) >= 4) {
        shape_t *ell = create_ellipse(scene, cx, cy, rx, ry, stroke, fill);
        if (ell != NULL) {
          add_shape_to_scene(scene, ell);
          printf("> Ellipse creee avec l'ID %d.\n", ell->id);
        }
      } else {
        printf("> Erreur syntaxe. Usage: create ellipse cx cy rx ry [stroke] "
               "[fill]\n");
      }
    } else if (strncmp(input, "create line", 11) == 0) {
      int x1 = 0, y1 = 0, x2 = 50, y2 = 50;
      char stroke[32] = "black", fill[32] = "none";
      if (sscanf(input, "create line %d %d %d %d %31s %31s", &x1, &y1, &x2, &y2,
                 stroke, fill) >= 4) {
        shape_t *line = create_line(scene, x1, y1, x2, y2, stroke, fill);
        if (line != NULL) {
          add_shape_to_scene(scene, line);
          printf("> Ligne creee avec l'ID %d.\n", line->id);
        }
      } else {
        printf("> Erreur syntaxe. Usage: create line x1 y1 x2 y2 [stroke] "
               "[fill]\n");
      }
    } else if (strncmp(input, "create square", 13) == 0) {
      int x = 0, y = 0, size = 50;
      char stroke[32] = "black", fill[32] = "none";
      if (sscanf(input, "create square %d %d %d %31s %31s", &x, &y, &size,
                 stroke, fill) >= 3) {
        shape_t *sq = create_rectangle(scene, x, y, size, size, stroke, fill);
        if (sq != NULL) {
          add_shape_to_scene(scene, sq);
          printf("> Carre cree avec l'ID %d.\n", sq->id);
        }
      } else {
        printf("> Erreur syntaxe. Usage: create square x y size [stroke] "
               "[fill]\n");
      }
    } else if (strncmp(input, "create polyline", 15) == 0) {
      char stroke[32] = "black", fill[32] = "none";
      sscanf(input, "create polyline %31s %31s", stroke, fill);
      shape_t *poly = create_polyline(scene, stroke, fill);
      if (poly != NULL) {
        add_shape_to_scene(scene, poly);
        printf("> Polyline creee avec l'ID %d. Utilisez 'add_point ID x y'\n",
               poly->id);
      }
    } else if (strncmp(input, "create polygon", 14) == 0) {
      char stroke[32] = "black", fill[32] = "none";
      sscanf(input, "create polygon %31s %31s", stroke, fill);
      shape_t *poly = create_polygon(scene, stroke, fill);
      if (poly != NULL) {
        add_shape_to_scene(scene, poly);
        printf("> Polygon cree avec l'ID %d. Utilisez 'add_point ID x y'\n",
               poly->id);
      }
    } else if (strncmp(input, "add_point", 9) == 0) {
      int id, x, y;
      if (sscanf(input, "add_point %d %d %d", &id, &x, &y) == 3) {
        shape_t *s = find_shape_by_id(scene->head, id);
        if (s && (s->type == POLYLINE || s->type == POLYGON)) {
          bool success = (s->type == POLYLINE) ? add_point_to_polyline(s, x, y)
                                               : add_point_to_polygon(s, x, y);
          if (success)
            printf("> Point (%d,%d) ajoute.\n", x, y);
        } else {
          printf("> ID %d introuvable ou mauvais type.\n", id);
        }
      } else {
        printf("> Erreur syntaxe. Usage: add_point ID x y\n");
      }
    } else if (strncmp(input, "create group", 12) == 0) {
      shape_t *group = create_group(scene);
      if (group != NULL) {
        add_shape_to_scene(scene, group);
        printf("> Groupe cree avec l'ID %d.\n", group->id);
      }
    } else if (strncmp(input, "add_to_group", 12) == 0) {
      int id_shape, id_group;
      if (sscanf(input, "add_to_group %d %d", &id_shape, &id_group) == 2) {
        shape_t *shape_to_move = find_shape_by_id(scene->head, id_shape);
        shape_t *target_group = find_shape_by_id(scene->head, id_group);

        if (shape_to_move && target_group && target_group->type == GROUP) {
          if (shape_to_move == target_group) {
            printf("> Erreur: un groupe ne peut pas se contenir lui-meme.\n");
          } else if (is_ancestor(shape_to_move, target_group)) {
            printf("> Erreur critique: Boucle infinie ! Le groupe cible est "
                   "deja un enfant de cette forme.\n");
          } else {
            detach_shape(shape_to_move, scene);
            add_shape_to_group(target_group, shape_to_move);
            printf("> Forme %d ajoutee au groupe %d.\n", id_shape, id_group);
          }
        } else {
          printf("> ID introuvable ou la cible n'est pas un groupe.\n");
        }
      } else {
        printf("> Erreur syntaxe. Usage: add_to_group ID_SHAPE ID_GROUP\n");
      }
    } else if (strncmp(input, "remove_from_group", 17) == 0) {
      int id;
      if (sscanf(input, "remove_from_group %d", &id) == 1)
        remove_from_group(scene, id);
      else
        printf("> Erreur syntaxe. Usage: remove_from_group ID\n");
    } else if (strncmp(input, "move", 4) == 0) {
      int id, dx, dy;
      if (sscanf(input, "move %d %d %d", &id, &dx, &dy) == 3) {
        shape_t *s = find_shape_by_id(scene->head, id);
        if (s) {
          s->transform.move_x += dx;
          s->transform.move_y += dy;
          printf("> Forme %d deplacee.\n", id);
        }
      }
    } else if (strncmp(input, "rotate", 6) == 0) {
      int id, angle;
      if (sscanf(input, "rotate %d %d", &id, &angle) == 2) {
        shape_t *s = find_shape_by_id(scene->head, id);
        if (s) {
          s->transform.angle = (s->transform.angle + angle) % 360;
          printf("> Forme %d pivotee.\n", id);
        }
      }
    } else if (strncmp(input, "flipx", 5) == 0) {
      int id;
      if (sscanf(input, "flipx %d", &id) == 1) {
        shape_t *s = find_shape_by_id(scene->head, id);
        if (s)
          s->transform.invert_x = !s->transform.invert_x;
      }
    } else if (strncmp(input, "flipy", 5) == 0) {
      int id;
      if (sscanf(input, "flipy %d", &id) == 1) {
        shape_t *s = find_shape_by_id(scene->head, id);
        if (s)
          s->transform.invert_y = !s->transform.invert_y;
      }
    } else if (strncmp(input, "color", 5) == 0) {
      int id;
      char stroke[32], fill[32];
      if (sscanf(input, "color %d %31s %31s", &id, stroke, fill) == 3) {
        shape_t *s = find_shape_by_id(scene->head, id);
        if (s) {
          strncpy(s->style.color_stroke, stroke, 31);
          s->style.color_stroke[31] = '\0';
          strncpy(s->style.color_fill, fill, 31);
          s->style.color_fill[31] = '\0';
          printf("> Couleurs mises a jour.\n");
        }
      }
    } else if (strncmp(input, "delete", 6) == 0) {
      int id;
      if (sscanf(input, "delete %d", &id) == 1)
        delete_shape(scene, id);
    } else if (strncmp(input, "viewport", 8) == 0) {
      int w, h;
      if (sscanf(input, "viewport %d %d", &w, &h) == 2) {
        scene->view_width = w;
        scene->view_height = h;
      }
    } else if (strcmp(input, "") != 0) {
      printf("Commande inconnue. Tapez 'help'.\n");
    }
  }

  free_scene(scene);
  printf("Fermeture propre de l'editeur.\n");
  return 0;
}
