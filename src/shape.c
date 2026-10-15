#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static shape_t *create_base_shape(scene_t *scene, shape_type_t type,
                                  char *color_stroke, char *color_fill);
shape_t *create_group(scene_t *scene);
shape_t *find_shape_by_id(shape_t *current, int target_id);
void add_shape_to_group(shape_t *group, shape_t *child);
void detach_shape(shape_t *shape, scene_t *scene);

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

void add_shape_to_scene(scene_t *scene, shape_t *shape) {
  if (scene == NULL || shape == NULL)
    return;

  if (scene->tail == NULL) {
    scene->head = shape;
    scene->tail = shape;
  } else {
    scene->tail->next = shape;
    shape->prev = scene->tail;
    scene->tail = shape;
  }
  scene->element_count++;
}

void detach_shape(shape_t *shape, scene_t *scene) {
  if (shape == NULL || scene == NULL)
    return;

  if (shape->prev != NULL) {
    shape->prev->next = shape->next;
  } else {
    if (shape->parent != NULL) {
      shape->parent->data.group.head = shape->next;
    } else {
      scene->head = shape->next;
    }
  }

  if (shape->next != NULL) {
    shape->next->prev = shape->prev;
  } else {
    if (shape->parent != NULL) {
      shape->parent->data.group.tail = shape->prev;
    } else {
      scene->tail = shape->prev;
    }
  }

  shape->prev = NULL;
  shape->next = NULL;
  shape->parent = NULL;
}

void add_shape_to_group(shape_t *group, shape_t *child) {
  if (group == NULL || child == NULL || group->type != GROUP)
    return;

  child->parent = group;

  if (group->data.group.tail == NULL) {
    group->data.group.head = child;
    group->data.group.tail = child;
  } else {
    group->data.group.tail->next = child;
    child->prev = group->data.group.tail;
    group->data.group.tail = child;
  }
}
void remove_from_group(scene_t *scene, int target_id) {
  shape_t *to_remove = find_shape_by_id(scene->head, target_id);
  if (to_remove == NULL) {
    printf("> ID %d introuvable.\n", target_id);
    return;
  }
  if (to_remove->parent == NULL) {
    printf("> La forme %d n'est pas dans un groupe.\n", target_id);
    return;
  }

  detach_shape(to_remove, scene);
  add_shape_to_scene(scene, to_remove);

  printf("> Forme %d retiree de son groupe et remise dans la scene.\n",
         target_id);
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

static shape_t *create_base_shape(scene_t *scene, shape_type_t type,
                                  char *color_stroke, char *color_fill) {
  shape_t *shape = malloc(sizeof(struct shape_s));
  if (shape == NULL) {
    fprintf(stderr, "Error: Failed to allocate shape.\n");
    return NULL;
  }

  shape->prev = NULL;
  shape->next = NULL;
  shape->parent = NULL;

  if (type == GROUP) {
    shape->data.group.head = NULL;
    shape->data.group.tail = NULL;
  }

  shape->id = scene->next_id++;
  shape->type = type;

  shape->transform.move_x = 0;
  shape->transform.move_y = 0;
  shape->transform.angle = 0;
  shape->transform.invert_x = false;
  shape->transform.invert_y = false;

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

  shape->data.polyline.head = NULL;
  shape->data.polyline.tail = NULL;
  shape->data.polyline.count = 0;

  return shape;
}

bool add_point_to_polyline(shape_t *shape, int x, int y) {
  if (shape == NULL || shape->type != POLYLINE)
    return false;

  point_node_t *new_node = malloc(sizeof(point_node_t));
  if (new_node == NULL) {
    fprintf(stderr, "Erreur: échec de l'allocation mémoire pour le point.\n");
    return false;
  }

  new_node->p.x = x;
  new_node->p.y = y;
  new_node->next = NULL;

  if (shape->data.polyline.tail == NULL) {
    shape->data.polyline.head = new_node;
    shape->data.polyline.tail = new_node;
  } else {
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
  shape_t *shape = create_base_shape(scene, GROUP, NULL, NULL);
  if (shape == NULL)
    return NULL;

  shape->data.group.head = NULL;
  shape->data.group.tail = NULL;

  return shape;
}
shape_t *create_group_with(scene_t *scene, int *ids, int count) {
  shape_t *group = create_group(scene);
  if (group == NULL)
    return NULL;
  add_shape_to_scene(scene, group);

  for (int i = 0; i < count; i++) {
    shape_t *to_move = find_shape_by_id(scene->head, ids[i]);
    if (to_move != NULL && to_move != group) {
      detach_shape(to_move, scene);
      add_shape_to_group(group, to_move);
    }
  }
  return group;
}

shape_t *find_shape_by_id(shape_t *current, int target_id) {
  if (current == NULL)
    return NULL;

  if (current->id == target_id)
    return current;

  if (current->type == GROUP && current->data.group.head != NULL) {
    shape_t *found_in_child =
        find_shape_by_id(current->data.group.head, target_id);
    if (found_in_child != NULL)
      return found_in_child;
  }

  return find_shape_by_id(current->next, target_id);
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
