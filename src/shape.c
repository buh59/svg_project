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
  if (type == POLYLINE || type == POLYGON || type == PATH) {
    if (type == POLYLINE && shape->data.polyline.points != NULL) {
      free(shape->data.polyline.points);
    } else if (type == POLYGON && shape->data.polygon.points != NULL) {
      free(shape->data.polygon.points);
    } else if (type == PATH && shape->data.path.points != NULL) {
      free(shape->data.path.points);
    }
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
    strncpy(shape->style.color_fill, color_fill, 9);
    shape->style.color_fill[9] = '\0';
  } else {
    shape->style.color_fill[0] = '\0';
  }

  if (color_stroke != NULL) {
    strncpy(shape->style.color_stroke, color_stroke, 9);
    shape->style.color_stroke[9] = '\0';
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

shape_t *create_group(scene_t *scene) {
  // Un groupe n'a par défaut pas de couleur propre
  shape_t *shape = create_base_shape(scene, GROUP, NULL, NULL);
  // Pas d'allocation mémoire supplémentaire ici : c'est l'avantage de la liste
  // chaînée !
  return shape;
}
