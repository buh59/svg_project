#ifndef SCENE_H
#define SCENE_H

#include "types.h"

/**
 * @brief Allocates and initializes the main SVG scene structure (viewport).
 * @param width The width of the viewport.
 * @param height The height of the viewport.
 * @return scene_t* A pointer to the newly created scene, or NULL on failure.
 */
scene_t *create_scene(int width, int height);

/**
 * @brief Completely frees the memory occupied by the scene and all its
 * contained shapes recursively.
 * @param scene Pointer to the scene to destroy.
 */
void free_scene(scene_t *scene);

/**
 * @brief Frees the memory of a specific shape (and its children if it is a
 * group).
 * @param shape Pointer to the shape to destroy.
 */
void free_shape(shape_t *shape);

/**
 * @brief Isolates a shape from the tree and frees its memory.
 * @param scene Pointer to the current scene.
 * @param target_id The ID of the shape to delete.
 */
void delete_shape(scene_t *scene, int target_id);

#endif
