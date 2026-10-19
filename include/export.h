#ifndef EXPORT_H
#define EXPORT_H

#include "types.h"

/**
 * @brief Traverses the shape tree and generates a text file in standard SVG
 * format.
 * @param scene Pointer to the scene containing the data to export.
 * @param filename The name of the destination file (e.g., "render.svg").
 */
void export_to_svg(scene_t *scene, const char *filename);

#endif
