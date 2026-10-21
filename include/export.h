#ifndef EXPORT_H
#define EXPORT_H

#include "types.h"

/**
 * @brief Traverses the shape tree and generates a text file in standard SVG
 * format. Does NOT open the file anymore (see open_in_viewer).
 * @param scene Pointer to the scene containing the data to export.
 * @param filename The name of the destination file (e.g., "render.svg").
 */
void export_to_svg(scene_t *scene, const char *filename);

/**
 * @brief Opens a file with the system viewer (xdg-open) without using a shell.
 * @param filename The file to open.
 */
void open_in_viewer(const char *filename);

#endif
