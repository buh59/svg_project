#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "types.h"
#include <stdbool.h>

/**
 * @brief Fonction utilitaire interne pour allouer la structure de base d'une
 * forme.
 */
shape_t *create_base_shape(scene_t *scene, shape_type_t type,
                           char *color_stroke, char *color_fill);

/** @brief Crée un rectangle. */
shape_t *create_rectangle(scene_t *scene, int x, int y, int width, int height,
                          char *color_stroke, char *color_fill);
/** @brief Crée un cercle. */
shape_t *create_circle(scene_t *scene, int cx, int cy, int r,
                       char *color_stroke, char *color_fill);
/** @brief Crée une ellipse. */
shape_t *create_ellipse(scene_t *scene, int cx, int cy, int rx, int ry,
                        char *color_stroke, char *color_fill);
/** @brief Crée une ligne. */
shape_t *create_line(scene_t *scene, int x1, int y1, int x2, int y2,
                     char *color_stroke, char *color_fill);

/** @brief Crée une liste de points (Polyligne ou Polygone). */
shape_t *create_point_list_shape(scene_t *scene, shape_type_t type,
                                 char *color_stroke, char *color_fill);
/** @brief Ajoute un point (x, y) à une polyline/polygone. */
bool add_point(shape_t *shape, int x, int y);

#endif
