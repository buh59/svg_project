#ifndef STORAGE_H
#define STORAGE_H

#include "types.h"

/**
 * @brief Saves the entire scene and its transformations into a plain text file.
 * @param scene The current scene to serialize.
 * @param filename The name of the save file.
 */
void save_project(scene_t *scene, const char *filename);

/**
 * @brief Loads a save file, allocates memory, and reconstructs the entire tree
 * (Algebraic Grammar).
 * @param filename The name of the file to load.
 * @return A pointer to the newly reconstructed scene, or NULL if loading fails.
 */
scene_t *load_project(const char *filename);

#endif
