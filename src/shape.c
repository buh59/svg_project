// src/shape.c
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =========================================================================
// GESTION DE LA SCÈNE
// =========================================================================

scene_t *create_scene(int width, int height) {
  scene_t *scene = malloc(sizeof(struct scene_s));
  if (scene == NULL) {
    return NULL;
  }
  scene->view_width = width;
  scene->view_height = height;

  // Initialisation des pointeurs pour la liste chaînée de la scène
  scene->head = NULL;
  scene->tail = NULL;
  scene->element_count = 0;
  scene->next_id = 1; // On commence les IDs à 1
  return scene;
}

void add_shape_to_scene(scene_t *scene, shape_t *shape) {
  if (scene == NULL || shape == NULL)
    return;

  if (scene->tail == NULL) {
    // La scène est vide
    scene->head = shape;
    scene->tail = shape;
  } else {
    // Ajout à la fin de la liste principale
    scene->tail->next = shape;
    shape->prev = scene->tail;
    scene->tail = shape;
  }
  scene->element_count++;
}

// =========================================================================
// GESTION DES GROUPES ET DES ARBRES
// =========================================================================

void add_shape_to_group(shape_t *group, shape_t *child) {
  if (group == NULL || child == NULL || group->type != GROUP)
    return;

  // On lie l'enfant à son nouveau parent
  child->parent = group;

  if (group->data.group.tail == NULL) {
    // Le groupe est vide
    group->data.group.head = child;
    group->data.group.tail = child;
  } else {
    // Ajout à la fin de la liste des enfants
    group->data.group.tail->next = child;
    child->prev = group->data.group.tail;
    group->data.group.tail = child;
  }
}

// =========================================================================
// DESTRUCTION (LIBÉRATION MÉMOIRE)
// =========================================================================

void free_shape(shape_t *shape) {
  if (shape == NULL)
    return;

  shape_type_t type = shape->type;

  // 1. Si c'est un Groupe, on doit nettoyer récursivement tous les enfants
  if (type == GROUP) {
    shape_t *current_child = shape->data.group.head;
    shape_t *next_child;

    while (current_child != NULL) {
      next_child = current_child->next;
      free_shape(current_child); // Détruire récursivement l'enfant
      current_child = next_child;
    }
    shape->data.group.head = NULL;
    shape->data.group.tail = NULL;
  }

  // 2. Libérer les données dynamiques spécifiques à certaines formes (tableaux
  // de points)
  // 2. Libérer les données dynamiques spécifiques à certaines formes
  if (type == POLYLINE || type == POLYGON || type == PATH) {
    if (type == POLYLINE) {
      point_node_t *current = shape->data.polyline.head;
      point_node_t *next;
      while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
      }
      shape->data.polyline.head = NULL;
      shape->data.polyline.tail = NULL;
    }
    if (type == POLYGON) {
      point_node_t *current = shape->data.polygon.head;
      point_node_t *next;
      while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
      }
      shape->data.polygon.head = NULL;
      shape->data.polygon.tail = NULL;
    }
    // (Même logique pour POLYGON et PATH plus tard)
  }

  // 3. Libérer la structure principale de la forme
  free(shape);
}

void free_scene(scene_t *scene) {
  if (scene == NULL)
    return;

  // On parcourt tous les éléments racines de la scène
  shape_t *current = scene->head;
  shape_t *next_shape;

  while (current != NULL) {
    next_shape = current->next;
    free_shape(
        current); // Détruit récursivement le nœud et ses enfants s'il y en a
    current = next_shape;
  }

  free(scene);
}

// =========================================================================
// CRÉATION DES FORMES
// =========================================================================

static shape_t *create_base_shape(scene_t *scene, shape_type_t type,
                                  char *color_stroke, char *color_fill) {
  shape_t *shape = malloc(sizeof(struct shape_s));
  if (shape == NULL) {
    fprintf(stderr, "Error: Failed to allocate shape.\n");
    return NULL;
  }

  // Initialisation des pointeurs de la liste doublement chaînée
  shape->prev = NULL;
  shape->next = NULL;
  shape->parent = NULL;

  // Initialisation des pointeurs internes si c'est un groupe
  if (type == GROUP) {
    shape->data.group.head = NULL;
    shape->data.group.tail = NULL;
  }

  // Attribution de l'ID via next_id
  shape->id = scene->next_id++;
  shape->type = type;

  // Initialisation des transformations par défaut
  shape->transform.move_x = 0;
  shape->transform.move_y = 0;
  shape->transform.angle = 0;
  shape->transform.invert_x = false;
  shape->transform.invert_y = false;

  // Copie sécurisée des couleurs (prévention du Segfault si NULL)
  if (color_fill != NULL) {
    strncpy(shape->style.color_fill, color_fill, 31);
    shape->style.color_fill[31] = '\0';
  } else {
    shape->style.color_fill[0] = '\0';
  }

  if (color_stroke != NULL) {
    strncpy(shape->style.color_stroke, color_stroke, 31);
    shape->style.color_stroke[31] = '\0';
  } else {
    shape->style.color_stroke[0] = '\0';
  }

  return shape;
}

shape_t *create_rectangle(scene_t *scene, int x, int y, int width, int height,
                          char *color_stroke, char *color_fill) {
  shape_t *shape =
      create_base_shape(scene, RECTANGLE, color_stroke, color_fill);
  if (shape == NULL)
    return NULL;

  shape->data.rect.x = x;
  shape->data.rect.y = y;
  shape->data.rect.width = width;
  shape->data.rect.height = height;

  return shape;
}

shape_t *create_polyline(scene_t *scene, char *color_stroke, char *color_fill) {
  shape_t *shape = create_base_shape(scene, POLYLINE, color_stroke, color_fill);
  if (shape == NULL)
    return NULL;

  // Initialisation de la liste chaînée de points
  shape->data.polyline.head = NULL;
  shape->data.polyline.tail = NULL;
  shape->data.polyline.count = 0;

  return shape;
}

bool add_point_to_polyline(shape_t *shape, int x, int y) {
  if (shape == NULL || shape->type != POLYLINE)
    return false;

  // Créer un nouveau nœud de point
  point_node_t *new_node = malloc(sizeof(point_node_t));
  if (new_node == NULL) {
    fprintf(stderr, "Erreur: échec de l'allocation mémoire pour le point.\n");
    return false;
  }

  new_node->p.x = x;
  new_node->p.y = y;
  new_node->next = NULL;

  // Ajouter à la liste chaînée
  if (shape->data.polyline.tail == NULL) {
    // Liste vide
    shape->data.polyline.head = new_node;
    shape->data.polyline.tail = new_node;
  } else {
    // Ajouter à la fin
    shape->data.polyline.tail->next = new_node;
    shape->data.polyline.tail = new_node;
  }

  shape->data.polyline.count++;
  return true;
}

shape_t *create_polygon(scene_t *scene, char *color_stroke, char *color_fill) {
  shape_t *shape = create_base_shape(scene, POLYGON, color_stroke, color_fill);
  if (shape == NULL)
    return NULL;
  shape->data.polygon.head = NULL;
  shape->data.polygon.tail = NULL;
  shape->data.polygon.count = 0;
  return shape;
}

bool add_point_to_polygon(shape_t *shape, int x, int y) {
  if (shape == NULL || shape->type != POLYGON)
    return false;

  point_node_t *new_node = malloc(sizeof(point_node_t));
  if (new_node == NULL)
    return false;

  new_node->p.x = x;
  new_node->p.y = y;
  new_node->next = NULL;

  if (shape->data.polygon.tail == NULL) {
    shape->data.polygon.head = new_node;
    shape->data.polygon.tail = new_node;
  } else {
    shape->data.polygon.tail->next = new_node;
    shape->data.polygon.tail = new_node;
  }
  shape->data.polygon.count++;
  return true;
}

shape_t *create_ellipse(scene_t *scene, int cx, int cy, int rx, int ry,
                        char *color_stroke, char *color_fill) {
  shape_t *shape = create_base_shape(scene, ELLIPSE, color_stroke, color_fill);
  if (shape == NULL)
    return NULL;

  shape->data.ellipse.cx = cx;
  shape->data.ellipse.cy = cy;
  shape->data.ellipse.rx = rx;
  shape->data.ellipse.ry = ry;

  return shape;
}

shape_t *create_line(scene_t *scene, int x1, int y1, int x2, int y2,
                     char *color_stroke, char *color_fill) {
  shape_t *shape = create_base_shape(scene, LINE, color_stroke, color_fill);
  if (shape == NULL)
    return NULL;

  shape->data.line.p1.x = x1;
  shape->data.line.p1.y = y1;
  shape->data.line.p2.x = x2;
  shape->data.line.p2.y = y2;

  return shape;
}

shape_t *create_circle(scene_t *scene, int cx, int cy, int r,
                       char *color_stroke, char *color_fill) {
  shape_t *shape = create_base_shape(scene, CIRCLE, color_stroke, color_fill);
  if (shape == NULL)
    return NULL;

  shape->data.circle.cx = cx;
  shape->data.circle.cy = cy;
  shape->data.circle.r = r;

  return shape;
}

shape_t *create_group(scene_t *scene) {
  // Un groupe n'a par défaut pas de couleur propre
  shape_t *shape = create_base_shape(scene, GROUP, NULL, NULL);
  // Pas d'allocation mémoire supplémentaire ici : c'est l'avantage de la liste
  // chaînée !
  return shape;
}

shape_t *find_shape_by_id(shape_t *current, int target_id) {
  if (current == NULL)
    return NULL;

  // On cherche d'abord dans le nœud courant
  if (current->id == target_id)
    return current;

  // Si c'est un groupe, on cherche récursivement dans ses enfants
  if (current->type == GROUP && current->data.group.head != NULL) {
    shape_t *found_in_child =
        find_shape_by_id(current->data.group.head, target_id);
    if (found_in_child != NULL)
      return found_in_child;
  }

  // Sinon, on passe au frère suivant dans la liste
  return find_shape_by_id(current->next, target_id);
}

void delete_shape(scene_t *scene, int target_id) {
  // 1. Trouver la forme (récursif grâce à ta fonction existante)
  shape_t *to_delete = find_shape_by_id(scene->head, target_id);
  if (to_delete == NULL) {
    printf("> ID %d introuvable.\n", target_id);
    return;
  }

  // 2. Déconnexion du maillon précédent
  if (to_delete->prev != NULL) {
    to_delete->prev->next = to_delete->next;
  } else {
    // C'était une tête de liste (soit de la scène, soit d'un groupe)
    if (to_delete->parent != NULL) {
      to_delete->parent->data.group.head = to_delete->next;
    } else {
      scene->head = to_delete->next;
    }
  }

  // 3. Déconnexion du maillon suivant
  if (to_delete->next != NULL) {
    to_delete->next->prev = to_delete->prev;
  } else {
    // C'était une queue de liste
    if (to_delete->parent != NULL) {
      to_delete->parent->data.group.tail = to_delete->prev;
    } else {
      scene->tail = to_delete->prev;
    }
  }

  // 4. Libération de la mémoire (free_shape gère aussi les enfants si c'est un
  // groupe)
  free_shape(to_delete);
  printf("> Forme %d supprimee avec succes.\n", target_id);
}
