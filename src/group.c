#include "../include/group.h"
#include "../include/geometry.h"
#include <stdio.h>
#include <stdlib.h>

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

bool is_ancestor(shape_t *potential_ancestor, shape_t *shape) {
  shape_t *curr = shape->parent;
  while (curr != NULL) {
    if (curr == potential_ancestor)
      return true;
    curr = curr->parent;
  }
  return false;
}
