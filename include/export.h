#ifndef EXPORT_H
#define EXPORT_H

#include "types.h"

/**
 * @brief Parcourt l'arbre des formes et génère un fichier texte au format SVG
 * standard.
 * @param scene Pointeur vers la scène contenant les données à exporter.
 * @param filename Nom du fichier de destination (ex: "rendu.svg").
 */
void export_to_svg(scene_t *scene, const char *filename);

#endif
