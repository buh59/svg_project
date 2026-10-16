#ifndef GROUP_H
#define GROUP_H

#include "types.h"
#include <stdbool.h>

/** @brief Ajoute une forme à la racine de la scène. */
void add_shape_to_scene(scene_t *scene, shape_t *shape);

/** @brief Détache une forme de son parent ou de la scène sans la détruire. */
void detach_shape(shape_t *shape, scene_t *scene);

/** @brief Recherche une forme par son ID dans l'arbre. */
shape_t *find_shape_by_id(shape_t *current, int target_id);

/** @brief Crée un groupe vide. */
shape_t *create_group(scene_t *scene);

/** @brief Ajoute une forme enfant dans un groupe parent. */
void add_shape_to_group(shape_t *group, shape_t *child);

/** @brief Crée un groupe contenant une liste de formes existantes. */
shape_t *create_group_with(scene_t *scene, int *ids, int count);

/** @brief Sort une forme de son groupe et la remet dans la scène. */
void remove_from_group(scene_t *scene, int target_id);

/** @brief Empêche un groupe de s'ajouter à lui-même (Prévention de boucle
 * infinie). */
bool is_ancestor(shape_t *potential_ancestor, shape_t *shape);

#endif
