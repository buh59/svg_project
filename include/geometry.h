#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "types.h"
#include <stdbool.h>

/**
 * @brief Calculates the local geometric center (barycenter) of a shape.
 * @param shape Pointer to the shape.
 * @param cx Output pointer for the calculated X center.
 * @param cy Output pointer for the calculated Y center.
 */
void get_local_center(shape_t *shape, int *cx, int *cy);

/**
 * @brief Utility function to allocate the base shape structure.
 *
 * This function initializes a shape with default transforms and style settings.
 *
 * @param scene Pointer to the scene context (for ID assignment).
 * @param type The type of shape to create (RECTANGLE, CIRCLE, etc.).
 * @param color_stroke Initial stroke color (e.g., "black" or "none").
 * @param color_fill Initial fill color (e.g., "red" or "none").
 * @return shape_t* A pointer to the newly created shape, or NULL if allocation
 * fails.
 */
shape_t *create_base_shape(scene_t *scene, shape_type_t type,
                           char *color_stroke, char *color_fill);

/**
 * @brief Creates a Rectangle shape.
 * @param scene Pointer to the scene context.
 * @param x The X coordinate of the top-left corner.
 * @param y The Y coordinate of the top-left corner.
 * @param width The width of the rectangle.
 * @param height The height of the rectangle.
 * @param color_stroke Initial stroke color.
 * @param color_fill Initial fill color.
 * @return shape_t* A pointer to the created Rectangle, or NULL if allocation
 * fails.
 */
shape_t *create_rectangle(scene_t *scene, int x, int y, int width, int height,
                          char *color_stroke, char *color_fill);
/**
 * @brief Creates a Circle shape.
 * @param scene Pointer to the scene context.
 * @param cx The center X coordinate.
 * @param cy The center Y coordinate.
 * @param r The radius of the circle.
 * @param color_stroke Initial stroke color.
 * @param color_fill Initial fill color.
 * @return shape_t* A pointer to the created Circle, or NULL if allocation
 * fails.
 */
shape_t *create_circle(scene_t *scene, int cx, int cy, int r,
                       char *color_stroke, char *color_fill);
/**
 * @brief Creates an Ellipse shape.
 * @param scene Pointer to the scene context.
 * @param cx The center X coordinate.
 * @param cy The center Y coordinate.
 * @param rx The radius along the X-axis.
 * @param ry The radius along the Y-axis.
 * @param color_stroke Initial stroke color.
 * @param color_fill Initial fill color.
 * @return shape_t* A pointer to the created Ellipse, or NULL if allocation
 * fails.
 */
shape_t *create_ellipse(scene_t *scene, int cx, int cy, int rx, int ry,
                        char *color_stroke, char *color_fill);
/**
 * @brief Creates a Line shape.
 * @param scene Pointer to the scene context.
 * @param x1 The X coordinate of the first point.
 * @param y1 The Y coordinate of the first point.
 * @param x2 The X coordinate of the second point.
 * @param y2 The Y coordinate of the second point.
 * @param color_stroke Initial stroke color.
 * @param color_fill Initial fill color (ignored for lines).
 * @return shape_t* A pointer to the created Line, or NULL if allocation fails.
 */
shape_t *create_line(scene_t *scene, int x1, int y1, int x2, int y2,
                     char *color_stroke, char *color_fill);

/**
 * @brief Creates a generic container shape for Polyline or Polygon.
 * @param scene Pointer to the scene context.
 * @param type The specific type (POLYLINE or POLYGON).
 * @param color_stroke Initial stroke color.
 * @param color_fill Initial fill color.
 * @return shape_t* A pointer to the created point list shape.
 */
shape_t *create_point_list_shape(scene_t *scene, shape_type_t type,
                                 char *color_stroke, char *color_fill);
/**
 * @brief Adds a point (x, y) to a Polyline or Polygon shape.
 * @param shape The target shape (must be POLYLINE or POLYGON).
 * @param x The X coordinate to add.
 * @param y The Y coordinate to add.
 * @return bool true if the point was added successfully, false otherwise.
 */
bool add_point(shape_t *shape, int x, int y);

/**
 * @brief Crée une forme complexe "Steamhappy" sous forme de groupe d'ellipses.
 * @param scene Pointeur vers la scène.
 * @param cx Position X du centre.
 * @param cy Position Y du centre.
 * @param size Taille/rayon de référence en pixels (ex: 100).
 * @return Pointeur vers le groupe créé.
 */
shape_t *create_steamhappy(scene_t *scene, int cx, int cy, int size);

#endif
