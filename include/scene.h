#ifndef SCENE_H
#define SCENE_H

#include "types.h"

/** @brief Alloue la scène principale. */
scene_t *create_scene(int width, int height);

/** @brief Libère intégralement la mémoire de la scène et de ses enfants. */
void free_scene(scene_t *scene);

/** @brief Libère la mémoire d'une forme spécifique (et de ses enfants si c'est
 * un groupe). */
void free_shape(shape_t *shape);

/** @brief Isole une forme de l'arbre et libère sa mémoire. */
void delete_shape(scene_t *scene, int target_id);

#endif
