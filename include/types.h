#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
  CIRCLE,
  ELLIPSE,
  RECTANGLE,
  SQUARE,
  LINE,
  POLYLINE,
  POLYGON,
  PATH,
  GROUP
} shape_type_t;

typedef struct point_s {
  int x;
  int y;
} point_t;

typedef struct transformation_s {
  int move_x;
  int move_y;
  int angle;
  bool invert_x;
  bool invert_y;
} transformation_t;

typedef struct colors_s {
  char color_stroke[10];
  char color_fill[10];
} colors_t;

typedef struct shape_s {
  int id;
  shape_type_t type;

  union {
    struct {
      int cx, cy, r;
    } circle;
    struct {
      int x, y, width, height;
    } rect;
    struct {
      point_t p1, p2;
    } line;
    struct {
      point_t *points;
      size_t count;
    } polyline;
    struct {
      point_t *points;
      size_t count;
    } polygon;
    struct {
      point_t *points;
      size_t count;
    } path;
    struct {
      struct shape_s **children;
      size_t count;
    } group;
  } data;

  transformation_t transform;
  colors_t style;
} shape_t;

typedef struct scene_s {
  int view_width;
  int view_height;
  shape_t **elements;
  size_t element_count;
  size_t capacity;
  int next_id;
} scene_t;

#endif
