#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Enumeration of supported geometric shape types.
 */
typedef enum {
  CIRCLE,
  ELLIPSE,
  RECTANGLE,
  LINE,
  POLYLINE,
  POLYGON,
  GROUP,
  PATH
} shape_type_t;

/**
 * @brief Structure representing 2D coordinates.
 */
typedef struct point_s {
  int x;
  int y;
} point_t;

/**
 * @brief Node in a linked list of points (for polylines and polygons).
 */
typedef struct point_node_s {
  point_t p;
  struct point_node_s *next;
} point_node_t;

/**
 * @brief Structure grouping transformations applicable to a shape.
 */
typedef struct transformation_s {
  int move_x;
  int move_y;
  int angle;
  bool invert_x;
  bool invert_y;
  float scale_factor;
} transformation_t;

/**
 * @brief Structure defining the visual style of a shape.
 */
typedef struct colors_s {
  char color_stroke[32];
  char color_fill[32];
} colors_t;

/**
 * @brief Main tree node (Abstract Grammar) representing a shape
 * or a group.
 */
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
    } point_list;
    struct {
      struct shape_s *head;
      struct shape_s *tail;
    } group;
    struct {
      char *d;
    } path;
  } data;

  transformation_t transform;
  colors_t style;
} shape_t;

/**
 * @brief Structure representing the SVG workspace root.
 */
typedef struct scene_s {
  int view_width;
  int view_height;
  shape_t *head;
  shape_t *tail;
  size_t element_count;
  int next_id;
} scene_t;

#endif
