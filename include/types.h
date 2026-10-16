#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Enumération des types de formes géométriques supportées.
 */
typedef enum {
  CIRCLE,
  ELLIPSE,
  RECTANGLE,
  LINE,
  POLYLINE,
  POLYGON,
  GROUP
} shape_type_t;

/**
 * @brief Structure représentant des coordonnées 2D.
 */
typedef struct point_s {
  int x;
  int y;
} point_t;

/**
 * @brief Nœud d'une liste chaînée de points (pour polylignes et polygones).
 */
typedef struct point_node_s {
  point_t p;
  struct point_node_s *next;
} point_node_t;

/**
 * @brief Structure regroupant les transformations applicables à une forme.
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
 * @brief Structure définissant le style visuel d'une forme.
 */
typedef struct colors_s {
  char color_stroke[32];
  char color_fill[32];
} colors_t;

/**
 * @brief Nœud principal de l'arbre (Grammaire Abstraite) représentant une forme
 * ou un groupe.
 */
typedef struct shape_s {
  int id;
  shape_type_t type;

  // Pointeurs de l'arbre N-aire (Liste Doublement Chaînée)
  struct shape_s *prev;   // Frère précédent
  struct shape_s *next;   // Frère suivant
  struct shape_s *parent; // Parent (groupe)

  // Données spécifiques au type de la forme
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
      struct shape_s *head; // Premier enfant du groupe
      struct shape_s *tail; // Dernier enfant du groupe
    } group;
  } data;

  transformation_t transform;
  colors_t style;
} shape_t;

/**
 * @brief Structure racine représentant l'espace de travail SVG.
 */
typedef struct scene_s {
  int view_width;
  int view_height;
  shape_t *head; // Première forme racine
  shape_t *tail; // Dernière forme racine
  size_t element_count;
  int next_id; // Compteur global pour garantir des IDs uniques
} scene_t;

#endif
