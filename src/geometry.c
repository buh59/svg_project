#include "../include/geometry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

shape_t *create_base_shape(scene_t *scene, shape_type_t type,
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
  shape->transform.scale_factor = 1.0f;

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

shape_t *create_point_list_shape(scene_t *scene, shape_type_t type,
                                 char *color_stroke, char *color_fill) {
  if (type != POLYLINE && type != POLYGON)
    return NULL;

  shape_t *shape = create_base_shape(scene, type, color_stroke, color_fill);
  if (shape != NULL) {
    shape->data.point_list.head = NULL;
    shape->data.point_list.tail = NULL;
    shape->data.point_list.count = 0;
  }
  return shape;
}

bool add_point(shape_t *shape, int x, int y) {
  if (shape == NULL || (shape->type != POLYLINE && shape->type != POLYGON))
    return false;

  point_node_t *new_node = malloc(sizeof(point_node_t));
  if (new_node == NULL)
    return false;

  new_node->p.x = x;
  new_node->p.y = y;
  new_node->next = NULL;

  if (shape->data.point_list.tail == NULL) {
    shape->data.point_list.head = new_node;
    shape->data.point_list.tail = new_node;
  } else {
    shape->data.point_list.tail->next = new_node;
    shape->data.point_list.tail = new_node;
  }
  shape->data.point_list.count++;
  return true;
}
