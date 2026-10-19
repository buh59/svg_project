#ifndef SHAPE_H
#define SHAPE_H

#include "types.h"

/**
 * @brief Allocates and initializes a new SVG scene.
 * @param width The width of the viewport.
 * @param height The height of the viewport.
 * @return A pointer to the created scene.
 */
scene_t *create_scene(int width, int height);

/**
 * @brief Completely frees the memory occupied by the scene and its children.
 * @param scene Pointer to the scene to destroy.
 */
void free_scene(scene_t *scene);

/**
 * @brief Adds a shape to the root of the scene.
 * @param scene The target scene.
 * @param shape The shape to add.
 */
void add_shape_to_scene(scene_t *scene, shape_t *shape);

/**
 * @brief Searches recursively for a shape in the tree by its ID.
 * @param current Starting node (usually scene->head).
 * @param target_id The ID to search for.
 * @return Pointer to the found shape, or NULL if not found.
 */
shape_t *find_shape_by_id(shape_t *current, int target_id);

/**
 * @brief Isolates, destroys, and frees the memory of a shape (and its children
 * if it is a group).
 * @param scene The current scene.
 * @param target_id The ID of the shape to delete.
 */
void delete_shape(scene_t *scene, int target_id);

/**
 * @brief Frees the memory of a specific shape recursively.
 * @param shape The shape to destroy.
 */
void free_shape(shape_t *shape);

shape_t *create_rectangle(scene_t *scene, int x, int y, int width, int height,
                          char *color_stroke, char *color_fill);
shape_t *create_circle(scene_t *scene, int cx, int cy, int r,
                       char *color_stroke, char *color_fill);
shape_t *create_ellipse(scene_t *scene, int cx, int cy, int rx, int ry,
                        char *color_stroke, char *color_fill);
shape_t *create_line(scene_t *scene, int x1, int y1, int x2, int y2,
                     char *color_stroke, char *color_fill);

/**
 * @brief Creates a Rectangle shape.
 * @param scene The current scene.
 * @param type The specific type (POLYLINE or POLYGON).
 * @param color_stroke Stroke color.
 * @param color_fill Fill color.
 * @return A pointer to the created shape.
 */
shape_t *create_point_list_shape(scene_t *scene, shape_type_t type,
                                 char *color_stroke, char *color_fill);

/**
 * @brief Adds a point coordinate (x, y) to a list of points.
 * @param shape The target shape (must be POLYLINE or POLYGON).
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return true if the addition was successful, false otherwise.
 */
bool add_point(shape_t *shape, int x, int y);

/**
 * @brief Detaches a shape from its current list without destroying it in
 * memory.
 * @param shape The shape to detach.
 * @param scene The reference scene.
 */
void detach_shape(shape_t *shape, scene_t *scene);

/**
 * @brief Creates a new empty group.
 * @param scene The current scene.
 * @return A pointer to the created group.
 */
shape_t *create_group(scene_t *scene);

/**
 * @brief Adds an existing shape inside a group.
 * @param group The target group.
 * @param child The child shape to insert.
 */
void add_shape_to_group(shape_t *group, shape_t *child);

/**
 * @brief Creates a group and directly inserts a list of existing IDs.
 * @param scene The current scene.
 * @param ids Array of IDs to group.
 * @param count Number of IDs in the array.
 * @return A pointer to the created group.
 */
shape_t *create_group_with(scene_t *scene, int *ids, int count);

/**
 * @brief Removes a shape from its group and places it back into the
 * root of the scene.
 * @param scene The current scene.
 * @param target_id The ID of the shape to extract.
 */
void remove_from_group(scene_t *scene, int target_id);

/**
 * @brief Checks if one shape is an ancestor of another (Prevents infinite
 * loop).
 * @param potential_ancestor The parent candidate to check.
 * @param shape The child being tested.
 * @return true if potential_ancestor is in shape's parent chain.
 */
bool is_ancestor(shape_t *potential_ancestor, shape_t *shape);

#endif
