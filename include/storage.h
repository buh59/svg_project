#ifndef STORAGE_H
#define STORAGE_H

#include "types.h"

/**
 * @brief Sauvegarde la scène entière et ses transformations dans un fichier
 * texte brut.
 * @param scene La scène courante à sérialiser.
 * @param filename Le nom du fichier de sauvegarde.
 */
void save_project(scene_t *scene, const char *filename);

/**
 * @brief Lit un fichier de sauvegarde, alloue la mémoire et reconstruit l'arbre
 * complet (Grammaire Algébrique).
 * @param filename Le nom du fichier à charger.
 * @return Pointeur vers la nouvelle scène reconstruite, ou NULL en cas d'échec.
 */
scene_t *load_project(const char *filename);

#endif
