#ifndef GROUP_H
#define GROUP_H

#include "types.h"
#include <stdbool.h>

/**
 * @brief Adds a shape to the root of the scene.
 * @param scene Pointer to the target scene.
 * @param shape Pointer to the shape to add.
 */
void add_shape_to_scene(scene_t *scene, shape_t *shape);

/**
 * @brief Detaches a shape from its parent or from the scene without destroying
 * it.
 * @param shape Pointer to the shape to detach.
 * @param scene Pointer to the scene context.
 */
void detach_shape(shape_t *shape, scene_t *scene);

/**
 * @brief Searches for a shape by its ID within the shape tree (recursively).
 * @param current Starting node (usually scene->head).
 * @param target_id The ID to search for.
 * @return shape_t* Pointer to the found shape, or NULL if not found.
 */
shape_t *find_shape_by_id(shape_t *current, int target_id);

/**
 * @brief Creates an empty group shape.
 * @param scene Pointer to the scene context.
 * @return shape_t* A pointer to the newly created group.
 */
shape_t *create_group(scene_t *scene);

/**
 * @brief Adds a child shape inside a parent group.
 * @param group The target group.
 * @param child The child shape to insert.
 */
void add_shape_to_group(shape_t *group, shape_t *child);

/**
 * @brief Creates a group containing a list of existing shape IDs.
 * @param scene Pointer to the scene context.
 * @param ids Array of IDs to group.
 * @param count Number of IDs in the array.
 * @return shape_t* A pointer to the created group.
 */
shape_t *create_group_with(scene_t *scene, int *ids, int count);

/**
 * @brief Removes a shape from its group and places it back into the root of the
 * scene.
 * @param scene Pointer to the target scene.
 * @param target_id The ID of the shape to extract.
 */
void remove_from_group(scene_t *scene, int target_id);

/**
 * @brief Checks if one shape is an ancestor of another shape (Prevents infinite
 * loops).
 * @param potential_ancestor The parent candidate to check.
 * @param shape The child being tested.
 * @return bool true if potential_ancestor is in shape's parent chain.
 */
bool is_ancestor(shape_t *potential_ancestor, shape_t *shape);

#endif
