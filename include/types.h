#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
  CIRCLE,
  ELLIPSE,
  RECTANGLE,
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

typedef struct point_node_s {
  point_t p;
  struct point_node_s *next;
} point_node_t;

typedef struct transformation_s {
  int move_x;
  int move_y;
  int angle;
  bool invert_x;
  bool invert_y;
} transformation_t;

typedef struct colors_s {
  char color_stroke[32];
  char color_fill[32];
} colors_t;

typedef struct shape_s {
  int id;
  shape_type_t type;

  struct shape_s *prev;
  struct shape_s *next;
  struct shape_s *parent;

  union {
    struct {
      int cx, cy, r;
    } circle;
    struct {
      int cx, cy, rx, ry;
    } ellipse;
    struct {
      int x, y, width, height;
    } rect;
    struct {
      point_t p1, p2;
    } line;
    struct {
      struct point_node_s *head;
      struct point_node_s *tail;
      size_t count;
    } polyline;
    struct {
      struct point_node_s *head;
      struct point_node_s *tail;
      size_t count;
    } polygon;
    struct {
      point_t *points;
      size_t count;
    } path;
    struct {
      struct shape_s *head;
      struct shape_s *tail;
    } group;
  } data;

  transformation_t transform;
  colors_t style;
} shape_t;

typedef struct scene_s {
  int view_width;
  int view_height;
  shape_t *head;
  shape_t *tail;
  size_t element_count;
  int next_id;
} scene_t;

#endif
