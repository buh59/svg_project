#include "../include/scene.h"
#include "../include/group.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>

scene_t *create_scene(int width, int height) {
  scene_t *scene = malloc(sizeof(struct scene_s));
  if (scene == NULL) {
    return NULL;
  }
  scene->view_width = width;
  scene->view_height = height;

  scene->head = NULL;
  scene->tail = NULL;
  scene->element_count = 0;
  scene->next_id = 1;
  return scene;
}

void free_shape(shape_t *shape) {
  if (shape == NULL)
    return;

  shape_type_t type = shape->type;

  if (type == GROUP) {
    shape_t *current_child = shape->data.group.head;
    shape_t *next_child;

    while (current_child != NULL) {
      next_child = current_child->next;
      free_shape(current_child);
      current_child = next_child;
    }
    shape->data.group.head = NULL;
    shape->data.group.tail = NULL;
  }
  if (type == POLYLINE || type == POLYGON) {
    point_node_t *current = shape->data.point_list.head;
    point_node_t *next;
    while (current != NULL) {
      next = current->next;
      free(current);
      current = next;
    }
    shape->data.point_list.head = NULL;
    shape->data.point_list.tail = NULL;
  }

  free(shape);
}

void free_scene(scene_t *scene) {
  if (scene == NULL)
    return;

  shape_t *current = scene->head;
  shape_t *next_shape;

  while (current != NULL) {
    next_shape = current->next;
    free_shape(current);
    current = next_shape;
  }

  free(scene);
}

void delete_shape(scene_t *scene, int target_id) {
  shape_t *to_delete = find_shape_by_id(scene->head, target_id);
  if (to_delete == NULL) {
    printf("> ID %d introuvable.\n", target_id);
    return;
  }

  if (to_delete->prev != NULL) {
    to_delete->prev->next = to_delete->next;
  } else {
    if (to_delete->parent != NULL) {
      to_delete->parent->data.group.head = to_delete->next;
    } else {
      scene->head = to_delete->next;
    }
  }

  if (to_delete->next != NULL) {
    to_delete->next->prev = to_delete->prev;
  } else {
    if (to_delete->parent != NULL) {
      to_delete->parent->data.group.tail = to_delete->prev;
    } else {
      scene->tail = to_delete->prev;
    }
  }

  free_shape(to_delete);
  printf("> Forme %d supprimee avec succes.\n", target_id);
}
